from flask import Blueprint, render_template, Flask, redirect, url_for, request, flash
from iis_app.model.Reservation import Reservation
from iis_app.model.VehicleSection import VehicleSection
from iis_app.model.VehicleSectionSeat import VehicleSectionSeat
from iis_app.utils.decorators import restrict_roles
from iis_app.logic import auth, user, stop, reserve
from iis_app.model import Connection, Stop
from flask_login import current_user
import  datetime as dt
from iis_app import db


bp = Blueprint("reservation", __name__, url_prefix="/reservation/")

@bp.route("/<connection_id>?<quantity>?<frm_stop_id>?<to_stop_id>?<datetime>", methods=['GET', 'POST'])
def reservation_page(connection_id, quantity, frm_stop_id, to_stop_id, datetime):
    connection = Connection.query.filter(Connection.connection_id == connection_id).first()
    frm_stop = Stop.query.filter(Stop.stop_id == frm_stop_id).first()
    to_stop = Stop.query.filter(Stop.stop_id == to_stop_id).first()
    ditim = dt.datetime.strptime(datetime, "%Y-%m-%d %H:%M:%S")

    if request.method == "POST": # the next button was pressed
        fullname = request.form.get("fullname", None)
        email = request.form.get("email", None)
        phone = request.form.get("phone_number", None)

        result = auth.register(username=None, pwd=None, full_name=fullname, email=email, phone=phone, role="unregistered")
        if result["role"] == "unregistered":
            user.edit(result["user_id"], full_name=fullname, phone=phone)
        elif result["role"] != "unregistered": # email entered is already in use by registered user
            flash(f"Email already in use.", "danger")
            return render_template("reservation-unreg.html", connection=connection, quantity=quantity, frm_stop=frm_stop, to_stop=to_stop, datetime=datetime)

        seats = reserve.get_empty_seats(connection, frm_stop, to_stop, ditim.date())
        if len(seats) < int(quantity):
            seats = None
            flash(f"Unable to find enough empty seats.", "danger")
            return redirect(url_for("main_page.main_page"))

        seat_ids = list()
        for seat in seats[:int(quantity)]:
            seat_ids.append(seat.vehicle_section_seat_id)

        return render_template("reservation-auth.html", connection=connection,
            quantity=quantity, frm_stop=frm_stop,
            to_stop=to_stop, datetime=datetime,
            user_id=result["user_id"], seat_ids=seat_ids,
            seats=seats[:int(quantity)], dateusr=ditim.strftime("%a %d %B %Y"), datereq=ditim.strftime("%Y-%m-%d"))

    if current_user.is_authenticated: #registered user
        seats = reserve.get_empty_seats(connection, frm_stop, to_stop, ditim.date())
        if len(seats) < int(quantity):
            seats = None
            flash(f"Unable to find enough empty seats.", "danger")
            return redirect(url_for("main_page.main_page"))


        seat_ids = list()
        for seat in seats[:int(quantity)]:
            seat_ids.append(seat.vehicle_section_seat_id)


        return render_template("reservation-auth.html", connection=connection,
            quantity=quantity, frm_stop=frm_stop,
            to_stop=to_stop, datetime=datetime,
            user_id=current_user.get_id(), seat_ids=seat_ids,
            seats=seats[:int(quantity)], dateusr=ditim.strftime("%a %d %B %Y"), datereq=ditim.strftime("%Y-%m-%d"))
    else: #unregistered user
        return render_template("reservation-unreg.html", connection=connection, quantity=quantity, frm_stop=frm_stop, to_stop=to_stop, datetime=datetime)

@bp.route("/<connection_id>?<quantity>?<frm_stop_id>?<to_stop_id>?<datetime>?<user_id>?<cost>?<seat_ids>", methods=['GET', 'POST'])
def create_reservation(connection_id, quantity, frm_stop_id, to_stop_id, datetime, user_id, cost, seat_ids):
    connection = Connection.query.filter(Connection.connection_id == connection_id).first()
    frm_stop = Stop.query.filter(Stop.stop_id == frm_stop_id).first()
    to_stop = Stop.query.filter(Stop.stop_id == to_stop_id).first()
    ditim = dt.datetime.strptime(datetime, "%Y-%m-%d %H:%M:%S")
    indexes = reserve.stop_to_index(connection, frm_stop, to_stop)
    print(seat_ids)
    print(type(seat_ids))
    if seat_ids[0] == '[':
        seat_ids = seat_ids[:-1]
        seat_ids = seat_ids[1:].split(", ")
    else:
        seat_ids = [int(seat_ids)]
    print(seat_ids)

    result = reserve.add_reservation(int(user_id), connection.connection_id, ditim.date(), int(cost), seat_ids, indexes["frm_index"], indexes["to_index"])
    if result["status"] == "error":
        flash(f"Unable to create new reservation.", "danger")
        return redirect(url_for("main_page.main_page"))

    else:
        flash(f"New reservation created successfully.", "success")
        return redirect(url_for("main_page.main_page"))


@bp.route("/my_reservations/", methods=['GET', 'POST'])
def my_reservations_page():
    with db.session.no_autoflush:
        res_con_list = db.session.query(
            Reservation, Connection,
        ).filter(
            Reservation.user_id == current_user.user_id,
            Reservation.connection_id == Connection.connection_id,
        ).all()

        for pair in res_con_list: #replacing indexes in reservation to stop objects
            pair_stop = reserve.index_to_stop(pair[1], pair[0].start_index, pair[0].end_index)
            pair[0].start_index = pair_stop["frm_stop"]
            pair[0].end_index = pair_stop["to_stop"]

            seats = list()
            for seat_id in pair[0].seats:
                seats.append(reserve.id_to_seat(seat_id))

            pair[0].seats = seats
            
        return render_template("my_reservations.html", res_con_list=res_con_list)

@bp.route("/my_reservations/<reservation_id>", methods=['GET', 'POST'])
def cancel_reservation(reservation_id):
    reservation = Reservation.query.filter(Reservation.reservation_id == reservation_id).first()

    if reservation.status == "pending":
        result = reserve.delete_reservation(reservation_id)
        if result["status"] == "ok":
            flash(f"Reservation canceled successfully.", "success")
            return redirect(url_for("reservation.my_reservations_page"))

    flash(f"Cancelation of reservation was unsuccessfull.", "danger")
    return redirect(url_for("reservation.my_reservations_page"))
