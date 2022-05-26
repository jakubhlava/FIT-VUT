from flask import Blueprint, render_template, Flask, redirect, url_for, request, flash, jsonify
from flask_bootstrap import Bootstrap
from flask_wtf import FlaskForm
from iis_app.utils.decorators import restrict_roles
from iis_app.logic import auth, user, connection, stop, reserve, personnel
from flask_login import current_user

bp = Blueprint("personnel", __name__, url_prefix="/personnel/")

def get_res():
    dict = {
        "123": ["pending", "user22", "12.12.2021","start-stop1", "end-stop1", "connection1", ["1","2","3"]], 
        "456": ["approved", "user4", "14.1.2021","start-stop144", "end-stop144", "connection144", ["4","5","6","7"]]
    }  
    return dict

@bp.route("/manage_reservations", methods=['GET', 'POST'])
@restrict_roles(["personnel", "admin"])
def manage_reservations_page():
    if request.method == "POST":
        decision = request.form.get("decision", None)
        reservation = request.args.get("res", None)
        state = request.args.get("show", "new")
        category = request.args.get("cat", "pending")
        if decision and reservation:
            result = personnel.decide_reservation(reservation, decision)
            if result["status"] == "ok":
                flash(f"Reservation {reservation} successfully {result['message']}", "success")
            else:
                flash(f"Error while performing action: {result['reason']}", "danger")
            return redirect(url_for("personnel.manage_reservation_page", state=state, category=category))
        else:
            flash("Internal error, please refresh your page and try again.", "danger")
            return redirect(url_for("personnel.manage_reservation_page", state=state, category=category))
    category = request.args.get("cat", "pending")
    state = request.args.get("show", "new")
    page = int(request.args.get("page", 1))
    if current_user.role == "personnel":
        reservations = personnel.paginate_reservations(page, 10, category, state, current_user.carrier)
    else:
        reservations = personnel.paginate_reservations(page, 10, category, state)
    stops = {}
    for r in reservations.items:
        start, end = None, None
        for s in r.connection.stops:
            if s.index == r.start_index:
                start = s
            if s.index == r.end_index:
                end = s
        stops[r.reservation_id] = {"start": start, "end": end}
    return render_template("personnel-manage_reservations.html", res_list=reservations, stops=stops, category=category, state=state)


def get_cons():
    dict = {
        "con_1": ["12:00 London, m.s.", "14:00 Sacred Lake, m.s.", "14:56 Hogwards, Academy of Magic"], 
        "con_2": ["22:00 Seoul, m.s.", "22:13 Drugged Van, m.s.", "23:56 Squid Island, m.s."] 
    }  
    return dict


@bp.route("/update_position", methods=['GET', 'POST'])
@restrict_roles(["admin", "personnel"])
def update_position_page():
    if current_user.role == "personnel":
        conns = personnel.get_conns_for_update(current_user.carrier)
    else:
        conns = personnel.get_conns_for_update()
    if request.method == "POST":
        lat = request.form.get("lat")
        long = request.form.get("long")
        conn = int(request.form.get("conn"))
        result = personnel.update_position(conn, lat, long)
        if result["status"] == "ok":
            flash("Position successfully updated!", "success")
            render_template("personnel-update_position.html", con_list=conns, curconn=conn, lat=lat, long=long)
        else:
            flash(f"Error while updating position: {result['reason']}")
            return redirect(url_for("personnel.update_position_page"))
    return render_template("personnel-update_position.html", con_list=conns)


@bp.route("/get_connection_vehicle_pos")
@restrict_roles(["admin", "personnel"])
def get_cvpos():
    conn_id = int(request.args.get("conn_id", None))
    return personnel.get_conn_vehicle_pos(conn_id)


# Remove approve/reject ?
# @bp.route("/approve_reject_reservations", methods=['GET', 'POST'])
# @restrict_roles(["admin", "personnel"])
# def approve_reject_reservations_page():
#     return render_template("personnel-approve_reject_reservations.html", res_list=get_res())


