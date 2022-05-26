import datetime

from iis_app.model import Reservation, Connection
from iis_app.logic.connection import weekdays
from iis_app.logic.stop import parse_lat, parse_long
from iis_app import db
from datetime import date
from flask import jsonify


def paginate_reservations(page, per_page, category=None, state="new", carrier=None):
    """
    Returns paginated reservations for specific carrier and connection
    :param page:            page number
    :param per_page:        number of entries per page
    :param category:        category of entries ("pending", "approved", "rejected")
    :param state:           "new" if only future reservations should be shown, "all" if all reservations
    :param carrier:         carrier id
    :return:
    """
    q = db.session.query(Reservation).join(Connection).order_by(db.asc(Reservation.reservation_date)).order_by(db.asc(Connection.connection_id))
    if carrier is not None:
        q = q.filter(Connection.carrier == carrier)
    if category is not None:
        q = q.filter(Reservation.status == category)
    if state == "new":
        q = q.filter(Reservation.reservation_date >= date.today())
    return q.paginate(page, per_page)


def decide_reservation(reservation_id, decision):
    """
    Changes state of reservation
    :param reservation_id:  reservation id
    :param decision:        new state of reservation
    :return:                dict with status fiend, in case of statsu: ok with message field, in case of error with reason field
    """
    res = db.session.get(Reservation, reservation_id)
    if res is None:
        return {
            "status": "error",
            "reason": "This reservation does not exist!"
        }
    if decision == "approve":
        res.status = "approved"
        db.session.commit()
        return {
            "status": "ok",
            "message": "confirmed"
        }
    elif decision == "reject":
        res.status = "rejected"
        db.session.commit()
        return {
            "status": "ok",
            "message": "rejected"
        }
    else:
        return {
            "status": "error",
            "reason": f"unknown action {decision}"
        }


def get_conns_for_update(carrier=None):
    """
    Returns relevant connections for today (and carrier)
    :param carrier:     id of carrier to filter connections (or None to disable this filter)
    :return:            connections for today (filtered on unfiltered by carrier)
    """
    day = datetime.datetime.now().weekday()
    q = Connection.query.filter(db.text(f"{weekdays[day]}::bit(8) & flags > B'00000000'")).order_by(db.asc(Connection.connection_id))
    if carrier is not None:
        q = q.filter_by(carrier=carrier)
    return q.all()


def update_position(connection, lat, long):
    """
    Updates position of vehicle linked to connection
    :param connection:  connectino to update
    :param lat:         WSG84 format latitude
    :param long:        WSG84 format longitude
    :return:
    """
    connection = db.session.get(Connection, connection)
    if connection is None:
        return {
            "status": "error",
            "reason": "This connection does not exist."
        }
    connection.vehicle_object.lat = parse_lat(lat)
    connection.vehicle_object.long = parse_long(long)
    db.session.commit()
    return {
        "status": "ok"
    }


def get_conn_vehicle_pos(connection):
    """
    Returns current position for vehicle linked to connection
    :param connection:  connection ID
    :return:            JSONified object with lat and long
    """
    connection = db.session.get(Connection, connection)
    return jsonify({
        "lat": connection.vehicle_object.lat,
        "long": connection.vehicle_object.long
    })