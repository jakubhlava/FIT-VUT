from iis_app.model import Connection, Vehicle, Stop, StopTimed, User
from iis_app import db
from sqlalchemy.dialects.postgresql import BIT
from datetime import date, time

weekdays = {
    0: 0b10000000,
    1: 0b01000000,
    2: 0b00100000,
    3: 0b00010000,
    4: 0b00001000,
    5: 0b00000100,
    6: 0b00000010
}


def paginate_connections(page=1, per_page=10, day=None, carrier=None):
    """
    Gets paginated connection from database, can be filtered by carrier parameter,
    or left None if we want to query all connections
    :param day:         weekday number (e.g. from datetime.weekday()) 0 = Monday, 6 = Sunday,
                        or None if this filter should be disabled
    :param page:        page number
    :param per_page:    number of connections per page
    :param carrier:     user id of carrier or None if all connections should be returned
    :return:            paginated connections or None if there are no connections matching requirements
    """
    q = Connection.query
    if day is not None and (day < 0 or day > 6):
        day = None
    if day is not None:
        q = q.filter(db.text(f"{weekdays[day]}::bit(8) & flags > B'00000000'"))
    if carrier is not None:
        q = q.filter_by(carrier=carrier)
    return q.order_by(db.asc(Connection.connection_id)).paginate(page, per_page)


def make_connection(deptime, depstop, arrtime, arrstop, arrprice, vehicle, carrier, stops, days):
    """
    Builds a connection from form data and saves it into database
    :param deptime:     time of departure from first stop
    :param depstop:     name of departure stop
    :param arrtime:     time of arrival
    :param arrstop:     name of arrival stop
    :param vehicle:     id of vehicle
    :param carrier:     id of carrier user
    :param stops:       list of dicts with 'stop' (name of stop) and 'time' (departure time) fields
    :param days:        list of numbers of weekdays as in datetime.weekday() method in which connection is active
    :return:            dict with status field and in case of status: error also with reason field
    """
    if not days:
        return {
            "status": "error",
            "reason": "You must choose at least one day in week."
        }
    if db.session.get(Vehicle, vehicle) is None:
        return {
            "status": "error",
            "reason": "This vehicle does not exist."
        }
    if db.session.get(User, carrier) is None:
        return {
            "status": "error",
            "reason": "This carrier does not exist."
        }
    flags = 0x00000000
    for day in days:
        flags |= weekdays[day]
    conn = Connection(flags=db.cast(flags, BIT(8)), carrier=carrier, vehicle_id=vehicle)
    db.session.add(conn)
    db.session.commit()
    s = Stop.query.filter_by(name=depstop).first()
    if s is None:
        db.session.delete(conn)
        db.session.commit()
        return {
            "status": "error",
            "reason": "Departure stop does not exist."
        }
    ds = StopTimed(stop_id=s.stop_id, departure=time.fromisoformat(deptime), index=0, price=0, connection_id=conn.connection_id)
    db.session.add(ds)
    index = 1
    for k in stops.keys():
        s = Stop.query.filter_by(name=stops[k]["stop"]).first()
        if s is None:
            db.session.rollback()
            db.session.delete(conn)
            db.session.commit()
            return {
                "status": "error",
                "reason": f"Stop {stops[k]['stop']} does not exist."
            }
        ds = StopTimed(stop_id=s.stop_id, departure=time.fromisoformat(stops[k]['time']),
                       index=index, price=int(stops[k]["price"]), connection_id=conn.connection_id)
        db.session.add(ds)
        index += 1
    s = Stop.query.filter_by(name=arrstop).first()
    if s is None:
        db.session.rollback()
        db.session.delete(conn)
        db.session.commit()
        return {
            "status": "error",
            "reason": "Arrival stop does not exist."
        }
    ds = StopTimed(stop_id=s.stop_id, departure=time.fromisoformat(arrtime), index=index, price=int(arrprice), connection_id=conn.connection_id)
    db.session.add(ds)
    db.session.commit()
    return {
        "status": "ok"
    }


def delete_connection(conn_id):
    """
    Removes connection from database (all other parts of connection should be set to CASCADE ON DELETE, so it does destruct itself)
    :param conn_id:     id of connection
    :return:            dict with status field and in case of status: error also with reason field
    """
    conn = db.session.get(Connection, conn_id)
    if conn is None:
        return {
            "status": "error",
            "reason": "Connection does not exist."
        }
    db.session.delete(conn)
    db.session.commit()
    return {
        "status": "ok"
    }
