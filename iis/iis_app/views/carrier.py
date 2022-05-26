from os import name
from flask import Blueprint, render_template, Flask, redirect, url_for, flash, request
from iis_app.utils.decorators import restrict_roles
from flask_login import current_user
from iis_app.logic import stop, connection, vehicle, user
from iis_app import db
from iis_app.model import Stop, User, Vehicle, Connection
from iis_app.logic import auth

from iis_app.model.VehicleSection import VehicleSection
from iis_app.model import VehicleSection

bp = Blueprint("carrier", __name__, url_prefix="/carrier/")


def get_cons():
    dict = {
        'con00': ['12:00', 'Brno - hl.n.', '14:00', 'Praha, hl.n.', 'EC 279 Metropolitan', 'Ceske drahy, a.s.',
                  ['12:00 Brno, hl.n.', '13:05 Pardubice, hl.n.', '13:50 Random, hl.n.', '14:00 Praha, hl.n.']],
        'con01': ['14:00', 'Praha, hl.n.', '14:00', 'Brno - hl.n.', 'EC 279 Metropolitan', 'Ceske drahy, a.s.',
                  ['12:00 Praha, hl.n.', '13:05 Pardubice, hl.n.', '13:50 Random, hl.n.', '14:00 Brno - hl.n.']]
    }
    return dict


def get_stops():
    dict = {
        'stop00': ['Brno - hl.n.', 'train', '11', '12'],
        'stop01': ['Praha - hl.n.', 'bus', '11', '12']
    }
    return dict


def get_vehicles():
    dict = {
        've00': ['EC 531', 'train'],
        've01': ['Regiobus', 'bus']
    }
    return dict


@bp.route("/manage_stops")
@restrict_roles(["admin", "carrier"])
def manage_stops_page():
    query = request.args.get("query", None)
    page = int(request.args.get("page", 1))
    return render_template("carrier-manage_stops.html",
                           stops_list=stop.paginate_stops(page, query=query))


@bp.route("/add_stop", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def add_stop_page():
    if request.method == "POST":
        return pending_helper("create")
    return render_template("add_stop.html")


@bp.route("/edit_stop/<int:stop_id>", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def edit_stop_page(stop_id):
    if request.method == "POST":
        return pending_helper("change", stop_id)
    return render_template("edit_stop.html", stop=stop.get_stop_data(stop_id))


def pending_helper(req_type, stop_id=None):
    name = request.form.get("name", None)
    type_ = request.form.get("type", None)
    lat = request.form.get("lat", None)
    long = request.form.get("long", None)
    skip = request.form.get("skip", False)
    delete = request.form.get("delete", "no")
    if delete == "yes":
        req_type = "delete"
    if skip == "false":  # conversion of special case when request is sent via jQuery
        skip = False
    if skip:
        if current_user.role != "admin":
            flash(f"You don't have permission for this!", "danger")
            return redirect(url_for("carrier.manage_stops_page"))
        else:
            if req_type == "create":
                result = stop.create_stop(name, type_, lat, long)
            elif req_type == "change":
                result = stop.edit_stop(stop_id, name, type_, lat, long)
            elif req_type == "delete":
                result = stop.delete_stop(stop_id)
    else:
        result = stop.create_pending(current_user.user_id, req_type, name, type_, lat, long, stop_id)
    if result["status"] == "error":
        flash(f"Error while creating request: {result['reason']}", "danger")
    else:
        if skip:
            flash(f"Request created successfully", "success")
        else:
            flash(
                f"Stop {'created' if req_type == 'create' else 'edited' if req_type == 'change' else 'deleted'} successfully!",
                "success")
    return redirect(url_for("carrier.manage_stops_page"))


@bp.route("/manage_connections", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def manage_connections_page():
    page = int(request.args.get("page", 1))
    if current_user.role == "admin":
        conns = connection.paginate_connections(page, 10)
    else:
        conns = connection.paginate_connections(page, 10, None, current_user.user_id)
    return render_template("carrier-manage_connections.html", conn_list=conns)


@bp.route("/add_connection", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def add_connection_page():
    if request.method == "POST":
        dep_time = request.form.get("dep_time")
        dep_stop = request.form.get("dep_stop")
        arr_time = request.form.get("arr_time")
        arr_stop = request.form.get("arr_stop")
        arr_price = request.form.get("last_stop_price")
        vehicle = request.form.get("vehicle")
        if current_user.role == "admin":
            carrier = request.form.get("carrier")
        else:
            carrier = current_user.user_id
        stops = {int(x[0][7:]): {"time": x[1]} for x in request.form.items() if x[0].startswith("ID_time")}
        stop_names = {int(x[0][7:]): x[1] for x in request.form.items() if x[0].startswith("ID_stop")}
        stop_prices = {int(x[0][10:]): x[1] for x in request.form.items() if x[0].startswith("stop_price")}
        for k, v in stop_names.items():
            stops[k]["stop"] = v
        for k, v in stop_prices.items():
            stops[k]["price"] = v
        days = [int(x[0][3]) for x in request.form.items() if x[0].startswith("day")]
        result = connection.make_connection(dep_time, dep_stop, arr_time, arr_stop, arr_price, vehicle, carrier, stops, days)
        if result["status"] == "ok":
            flash("Connection added successfully!", "success")
            return redirect(url_for("carrier.manage_connections_page"))
        else:
            flash(f"Error when adding connection: {result['reason']}", "danger")
            return redirect(url_for("carrier.add_connection_page"))

    stop_names = list()

    if current_user.role == "admin":
        carriers = User.query.filter_by(role="carrier").all()
    else:
        carriers = [current_user]

    for stop in Stop.query.all():
        stop_names.append(stop.name)

    if current_user.role == "carrier":
        vehicles = current_user.carrier_vehicles
    else:
        vehicles = Vehicle.query.order_by(db.asc(Vehicle.vehicle_id)).all()

    return render_template("add_connection.html", vehicles=vehicles, carriers=carriers,
                           stop_names=stop_names, form_action=url_for("carrier.add_connection_page"),
                           back_btn=url_for("carrier.manage_connections_page"), add_stop_btn=" Send for approval")


@bp.route("/delete_connection/<int:conn_id>")
@restrict_roles(["admin", "carrier"])
def delete_connection(conn_id):
    result = connection.delete_connection(conn_id)
    if result["status"] == "error":
        flash(f"Error occured during connection removal: {result['reason']}", "danger")
    else:
        flash(f"Connection {conn_id} removed successfully", "success")
    return redirect(url_for("carrier.manage_connections_page"))


def get_con_for_edit():
    dict = {
        'con00': ['12:00 AM', 'Brno - hl.n.', '02:00 PM', 'Praha, hl.n.', 'EC 279 Metropolitan', 'Ceske drahy, a.s.',
                  ['12:00 AM', 'Brno, hl.n.', '01:05 PM', 'Pardubice, hl.n.', '01:50 PM', 'Random, hl.n.', '02:00 PM',
                   'Praha, hl.n.']],
    }
    return dict


@bp.route("/edit_connection", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def edit_connection_page():
    stop_names = list()
    carrier_names = list()
    vehicle_names = list()

    for carrier in User.query.filter(User.role == 'carrier').all():
        carrier_names.append(carrier.username)

    for stop in Stop.query.all():
        stop_names.append(stop.name)

    for vehicle in Vehicle.query.all():
        vehicle_names.append(vehicle.name)
    return render_template("edit_connection.html", vehicle_names=vehicle_names, carrier_names=carrier_names,
                           stop_names=stop_names, con_list=get_con_for_edit())


@bp.route("/manage_vehicles")
@restrict_roles(["admin", "carrier"])
def manage_vehicles_page():
    page = request.args.get("page", 1)
    if current_user.role == "admin":
        carrier = None
    else:
        carrier = current_user.user_id
    return render_template("carrier-manage_vehicles.html", vehicles_list=vehicle.paginate_vehicles(page, 10, carrier))


@bp.route("/add_vehicle", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def add_vehicle_page():
    if current_user.role == "admin":
        carriers = user.get_carriers(list=True)
    else:
        carriers = None
    if request.method == "POST":
        name = request.form.get("name")
        type = request.form.get("type")
        section_count = int(request.form.get("sect_num"))
        carrier = request.form.get("carrier", current_user.user_id)
        sections = []
        for i in range(1, section_count + 1):
            section = {
                "name": request.form.get(f"sect_name{i}"),
                "seats": int(request.form.get(f"seats_num{i}")),
                "index": i
            }
            sections.append(section)
        result = vehicle.create_vehicle(name, type, carrier, sections)
        if result["status"] == "error":
            flash(f"Error occured while creating vehicle: {result['reason']}. Try again.", "danger")
            return redirect(url_for("carrier.add_vehicle_page"))
        else:
            flash("Vehicle added successfully!", "success")
            return redirect(url_for("carrier.manage_vehicles_page"))

    return render_template("add_vehicle.html", carriers=carriers)


@bp.route("/delete_vehicle/<int:vehicle_id>")
@restrict_roles(["admin", "carrier"])
def delete_vehicle(vehicle_id):
    result = vehicle.delete_vehicle(vehicle_id)
    if result["status"] == "error":
        flash(f"Error occured while deleting vehicle: {result['reason']}", "danger")
    else:
        flash(f"Vehicle was successfully deleted!", "success")
    return redirect(url_for("carrier.manage_vehicles_page"))


def get_pers():
    dict = {
        '1': ['pers00', 'Jan Novak', 'email@email.cz', '111222333'],
        '2': ['pers01', 'Jana Novakova', 'emaila@email.cz', '123123123']
    }
    return dict


@bp.route("/manage_personnel", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def manage_personnel_page():
    page = int(request.args.get("page", 1))
    return render_template("carrier-manage_personnel.html",
                           pers_list=user.get_user_pages(page, 10, current_user.user_id))


@bp.route("/add_personnel", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def add_personnel_page():
    if current_user.role == "admin":
        return redirect(url_for("admin.add_user_page"))
    if request.method == "POST":
        username = request.form.get("username", None)
        password = request.form.get("password", None)
        email = request.form.get("email", None)
        phone = request.form.get("phone", None)
        fullname = request.form.get("fullname", None)
        if username is None or password is None or email is None:
            flash("Error while creating new user. Please fill all required fields.", "danger")
            return render_template("add_personnel_acc.html", username=username, email=email, full_name=fullname,
                                   phone=phone)
        else:
            result = auth.register(username, password, email, "personnel", carrier=current_user.user_id,
                                   full_name=fullname, phone=phone)
            if result["status"] == "ok":
                flash(f"User {username} created successfully!", "success")
                return redirect(url_for("carrier.manage_personnel_page"))
            else:
                flash(f"Error while creating new user: {result['reason']}", "danger")
                return render_template("add_personnel_acc.html", username=username, email=email, full_name=fullname,
                                       phone=phone)
    return render_template("add_personnel_acc.html")


@bp.route("/edit_personnel/<int:user_id>", methods=['GET', 'POST'])
@restrict_roles(["admin", "carrier"])
def edit_personnel_page(user_id):
    u = user.get(user_id)
    if request.method == "POST":
        username = request.form.get("username", None)
        password = request.form.get("password", None)
        email = request.form.get("email", None)
        phone = request.form.get("phone", None)
        fullname = request.form.get("fullname", None)
        result = user.edit(user_id, username=username, password=password, email=email,
                           role="personnel", full_name=fullname, phone=phone)
        if result["status"] == "ok":
            flash(f"User {username} edited successfully!", "success")
            return redirect(url_for("carrier.manage_personnel_page", page=1))
        else:
            flash(f"Error while editing user: {result['reason']}", "danger")
            return redirect(url_for("carrier.manage_personnel_page", page=1))
    return render_template("edit_personnel_acc.html", user=u)


@bp.route("/delete_user/<int:user_id>")
@restrict_roles(["admin", "carrier"])
def delete_user(user_id):
    result = user.remove(user_id)
    if result["status"] == "ok":
        flash("User successfully deleted!", "success")
        return redirect(url_for("carrier.manage_personnel_page", page=1))
    else:
        flash(f"Error while deleting user {user_id}. {result['reason']}")
        return redirect(url_for("carrier.manage_personnel_page", page=1))
