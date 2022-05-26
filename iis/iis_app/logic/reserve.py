from flask import flash
from iis_app import db
from iis_app.model import User, Reservation, VehicleSectionSeat
from flask_login import login_user, logout_user, current_user
from sqlalchemy.exc import IntegrityError
from datetime import timedelta
from werkzeug.security import generate_password_hash, check_password_hash
from iis_app.model import Connection, Stop, StopTimed, VehicleSectionSeat
from iis_app.model.Vehicle import Vehicle
from iis_app.model.VehicleSection import VehicleSection
from sqlalchemy import or_
from sqlalchemy import and_



def get_price(connection, frm_stop, to_stop, quantity):
    """
    Calculates the total price of reserving a connection in range (frm_stop, to_stop>.
    :param connection:  connection from which the price is to be calculated
    :param frm_stop:    stop object which serves as a starting point of calculation
    :param to_stop:     stop object which serves as an ending point of calculation
    :return:            an integer representing the total price of the reservation
    """
    result = 0

    do_count = False
    for tstop in connection.stops:
        if tstop.stop_id == frm_stop.stop_id:
            do_count = True
            continue

        if do_count:
            result += tstop.price

        if(tstop.stop_id == to_stop.stop_id):
            do_count = False

    return result * int(quantity)

def stop_to_index(connection, frm_stop, to_stop):
    """
    Gets a dictionary which contains indexes based on specified arguments.
    :param connection:  connection from which indexes are to be extracted
    :param frm_stop     stop object soon to be an index in relation to connection
    :param to_sto       stop object soon to be an index in relation to connection
    :return:            dict with "frm_index" and "to_index" keys, both hold an integer
                        value representing an index in the path of stops (e.i. connection)
                        this integer can be '-1' if no match was found
    """
    frm_index = -1
    to_index = -1

    for tstop in connection.stops:
        if tstop.stop_id == frm_stop.stop_id:
            frm_index = tstop.index
        elif tstop.stop_id == to_stop.stop_id:
            to_index = tstop.index

    return {
        "frm_index": frm_index,
        "to_index": to_index
    }


def index_to_stop(connection, frm_index, to_index):

    for tstop in connection.stops:
        if tstop.index == frm_index:
            frm_stop = Stop.query.filter(Stop.stop_id == tstop.stop_id).first()
        elif tstop.index == to_index:
            to_stop = Stop.query.filter(Stop.stop_id == tstop.stop_id).first()

    return {
        "frm_stop": frm_stop,
        "to_stop": to_stop
    }

def id_to_seat(seat_id):
    return db.session.query(VehicleSectionSeat).filter(VehicleSectionSeat.vehicle_section_seat_id == seat_id).first()


def get_empty_seats(connection, frm_stop, to_stop, date):
    """
    Gets a list of empty seats constrained by specified arguments.
    :param connection:  connection of which empty seats are to be found
    :param frm_stop:    stop from which the user wishes to departure
    :param to_stop:     stop to which the user wishes to arrive
    :param date:        date (e.g. "2021-11-11")
    :return:            list of seat objects which are not reserved
                        in the specified connection on the specified date,
                        where neither frm_stop or to_stop are in range <frm_stop, to_stop)
    """
    indexes = stop_to_index(connection, frm_stop, to_stop)

    all_reserved_seats = list() #all reserved seats for that day WHERE Reservation.start_index <= frm_index < Reservation.start_index OR Reservation.start_index <= to_index <= Reservation.start_index
    q = db.session.query(
        Reservation.seats
    ).filter(
        Reservation.reservation_date == date,
        or_(
            and_(Reservation.start_index <= indexes["frm_index"], indexes["frm_index"] < Reservation.end_index),
            and_(Reservation.start_index <= indexes["to_index"], indexes["to_index"] <= Reservation.end_index)
        )
    ).all()
    for seat_group in q:
        for seat in seat_group[0]:
            all_reserved_seats.append(seat)

    return db.session.query(
        VehicleSectionSeat,
    ).filter(
        VehicleSectionSeat.vehicle_section_seat_id.not_in(all_reserved_seats),
        Connection.connection_id == connection.connection_id,
        Connection.vehicle_id == Vehicle.vehicle_id,
        VehicleSection.vehicle_id == Vehicle.vehicle_id,
        VehicleSection.vehicle_section_id == VehicleSectionSeat.vehicle_section_id,
    ).all()

def add_reservation(user_id, connection_id, reservation_date, cost, seats, start_index, end_index):
    """
    Adds a new pending reservation to the pool of reservations.
    :param user_id:             user ID of the user making the reservation
    :param connection_id:       connection ID of the reservation
    :param reservation_date:    date (e.g. "2021-11-11")
    :param cost:                total cost of the reservation
    :param seats:               list of seat IDs being reserved
    :param start_index:         starting index of connection
    :param end_index:           ending index of connection
    :return:                    dict with "status" which holds either
                                "ok" or "error" depending on reservation result
    """
    new_reservation = Reservation(status="pending", user_id=user_id, connection_id=connection_id, reservation_date=reservation_date, cost=cost, seats=seats, start_index=start_index, end_index=end_index)

    db.session.add(new_reservation)
    try:
        db.session.commit()
        return {
            "status": "ok"
        }
    except IntegrityError as e:
        db.session.rollback()
        return {
            "status": "error"
        }

def delete_reservation(reservation_id):
    reservation = db.session.get(Reservation, reservation_id)
    if reservation is None:
        return {
            "status": "error",
        }
    else:
        db.session.delete(reservation)
        db.session.commit()
        return {
            "status": "ok",
        }
