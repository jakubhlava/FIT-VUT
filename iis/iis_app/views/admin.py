from flask import Blueprint, render_template, Flask, redirect, url_for, request, flash
from werkzeug.security import generate_password_hash
from iis_app.utils.decorators import restrict_roles
from iis_app.logic import auth, user, stop


bp = Blueprint("admin", __name__, url_prefix="/admin/")


@bp.route("/", methods=['GET', 'POST'])
@restrict_roles("admin")
def main_page():
    return render_template("admin_manage_users.html", users_list=user.get_user_pages())


@bp.route("/manage_users")
@restrict_roles("admin")
def manage_users_page():
    page = int(request.args.get("page", 1))
    return render_template("admin_manage_users.html", users_list=user.get_user_pages(page))


def get_stops():
    dict = {
        'stop00':['Brno - hl.n.', 'train', '11', '12'],
        'stop01':['Praha - hl.n.', 'bus', '11', '12']
    }
    return dict


@bp.route("/approve_reject_stops")
@restrict_roles("admin")
def approve_reject_stops_page():
    page = int(request.args.get("page", 1))
    stops = stop.paginate_pending_changes(page)
    old_stops = {i.stop_id: stop.get_stop_data(i.stop_id) for i in stops.items if i.change_type == "change"}
    return render_template("admin-approve_reject_stops.html", stops_list=stops, old_stops=old_stops, extend_base="admin_base.html")


@bp.route("/approve_reject_stops/decide/<int:change_id>", methods=["POST"])
@restrict_roles("admin")
def approve_reject_decide(change_id):
    decision = request.form.get("decision", None)
    if decision is None:
        flash("Error while performing decision.", "danger")
    else:
        result = stop.finish_pending(change_id, decision == "approve")
        if result["status"] == "error":
            flash(f"Error while performing decision: {result['reason']}", "danger")
        else:
            flash(f"Change successfully {'approved' if decision == 'approve' else 'rejected'}", "success")
    return redirect(url_for("admin.approve_reject_stops_page"))


@bp.route("/add_stop", methods=['GET', 'POST'])
@restrict_roles("admin")
def add_stop_page():
    return render_template("add_stop.html", form_action=url_for("admin.add_stop_page"), back_btn=url_for("admin.approve_reject_stops_page"), add_stop_btn=" Create new stop")


@bp.route("/add_user", methods=['GET', 'POST'])
@restrict_roles("admin")
def add_user_page():
    if request.method == "POST":
        username = request.form.get("username", None)
        password = request.form.get("password", None)
        email = request.form.get("email", None)
        role = request.form.get("role", None)
        phone = request.form.get("phone", None)
        fullname = request.form.get("fullname", None)
        carrier = request.form.get("carrier", None)
        if username is None or password is None or email is None or role is None:
            flash("Error while creating new user. Please fill all required fields.", "danger")
            return render_template("admin-add_user.html", username=username, email=email, full_name=fullname, phone=phone, role=role)
        else:
            result = auth.register(username, password, email, role, full_name=fullname, phone=phone, carrier=carrier)
            if result["status"] == "ok":
                flash(f"User {username} created successfully!", "success")
            else:
                flash(f"Error while creating new user: {result['reason']}", "danger")
                render_template("admin-add_user.html", username=username, email=email, full_name=fullname, phone=phone, role=role)
    return render_template("admin-add_user.html")


@bp.route("/get_carriers")
@restrict_roles("admin")
def get_carriers():
    return str(user.get_carriers())


@bp.route("/edit_user/<int:user_id>", methods=['GET', 'POST'])
@restrict_roles("admin")
def edit_user_page(user_id):
    userdata = user.get(user_id)
    if request.method == "POST":
        username = request.form.get("username", None)
        password = request.form.get("password", None)
        email = request.form.get("email", None)
        role = request.form.get("role", None)
        phone = request.form.get("phone", None)
        fullname = request.form.get("fullname", None)
        result = user.edit(user_id, username=username, password=password, email=email,
                      role=role, full_name=fullname, phone=phone)
        if result["status"] == "ok":
            flash(f"User {username} edited successfully!", "success")
            return redirect(url_for("admin.manage_users_page", page=1))
        else:
            flash(f"Error while editing user: {result['reason']}", "danger")
            return redirect(url_for("admin.manage_users_page", page=1))
    return render_template("admin-edit_user.html", user=userdata)


@bp.route("/delete_user/<int:user_id>")
@restrict_roles("admin")
def delete_user(user_id):
    result = user.remove(user_id)
    if result["status"] == "ok":
        flash("User successfully deleted!", "success")
        return redirect(url_for("admin.manage_users_page", page=1))
    else:
        flash(f"Error while deleting user {user_id}. {result['reason']}")
        return redirect(url_for("admin.manage_users_page", page=1))

