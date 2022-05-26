from iis_app import db
from iis_app.model import StopPending, Stop


def paginate_stops(page=1, per_page=10, query=None):
    """
    Returns one page of stops for rendering in frontend
    :param page:        page number
    :param per_page:    number of stops per page
    :param query:       optional filtering string, query is filtered based on name field
    :return:            list of stops
    """
    q = Stop.query
    if query is not None:
        q = q.filter(Stop.name.like(f"%{query}%"))
    if q is not None:
        return q.paginate(page, per_page)
    else:
        return []


def paginate_pending_changes(page=1, per_page=10, query=None, user_id=None):
    """
    Returns one page of pending changes to stops (pending change is change which was not solved by any admin)
    :param page:        page number
    :param per_page:    number of stops per page
    :param query:       optional filtering string, query is filtered based on name field
    :return:            list of pending changes
    """
    q = StopPending.query.filter_by(change_state="pending")
    if query is not None:
        q = q.filter(Stop.name.like(f"%{query}%"))
    if user_id is not None:
        q = q.filter_by(user_id=user_id)
    if q is not None:
        return q.paginate(page, per_page)
    else:
        return []


def paginate_old_changes(page=1, per_page=10, query=None, user_id=None):
    """
    Returns one page of old changes to stops (old change is change which was either approved or declined by admin)
    :param page:        page number
    :param per_page:    number of stops per page
    :param query:       optional filtering string, query is filtered based on name field
    :return:            list of pending changes
    """
    q = StopPending.query.filter(StopPending.change_state != "pending")
    if query is not None:
        q = q.filter(Stop.name.like(f"%{query}%"))
    if user_id is not None:
        q = q.filter_by(user_id=user_id)
    if q is not None:
        return q.paginate(page, per_page)
    else:
        return []


def create_stop(name, stop_type, lat, long):
    """
    Creates stop
    :param name:        unique name of stop
    :param stop_type:   type of stop
    :param lat:         GPS latitude
    :param long:        GPS longitude
    :return:
    """
    if type(lat) != float:
        lat = parse_lat(lat)
    if type(long) != float:
        long = parse_long(long)
    stop = Stop(name=name, type=stop_type, lat=lat, long=long)
    db.session.add(stop)
    try:
        db.session.commit()
    except Exception as e:
        return {
            "status": "error",
            "reason": "Stop already exists.",
            "data": e
        }
    else:
        return {
            "status": "ok"
        }


def create_pending(user_id, p_type="change", name=None, stop_type=None, lat=None, long=None, stop_id=None):
    """
    Creates pending change
    :param p_type:      type of change: "create", "change", "remove"
    :param name:        new name or None
    :param stop_type:   new stop type or None
    :param lat:         new GPS latitude or None
    :param long:        new GPS longitude or None
    :param stop_id:     ID if stop exists, in case of p_type: "create", this field is ignored
    :return:
    """
    print(f"Creating pending for stop {stop_id}")
    if p_type == "change":
        if stop_id is None:
            stop = None
        else:
            stop = db.session.get(Stop, stop_id)
        if stop is None:
            return {
                "status": "error",
                "reason": "Stop does not exist."
            }
        pending = StopPending(change_type="change", change_state="pending", name=name,
                              type=stop_type, lat=parse_lat(lat), long=parse_long(long), stop_id=stop_id, user_id=user_id)
        db.session.add(pending)
        db.session.commit()
    elif p_type == "create":
        if Stop.query.filter_by(name=name).first() is not None:
            return {
                "status": "error",
                "reason": "Stop already exists."
            }
        else:
            if name is None or stop_type is None or lat is None or long is None:
                return {
                    "status": "error",
                    "reason": "All fields are required!"
                }
            pending = StopPending(change_type="create", change_state="pending", name=name,
                                  type=stop_type, lat=parse_lat(lat), long=parse_long(long), stop_id=None, user_id=user_id)
            db.session.add(pending)
            db.session.commit()
    elif p_type == "delete":
        stop = db.session.get(Stop, stop_id)
        if stop is None:
            return {
                "status": "error",
                "reason": "Stop does not exiset."
            }
        pending = StopPending(change_type="delete", change_state="pending", name=stop.name,
                              type=stop.type, lat=stop.lat, long=stop.long, stop_id=stop_id, user_id=user_id)
        db.session.add(pending)
        db.session.commit()
    else:
        return {
            "status": "error",
            "reason": "Internal error: unknown p_type"
        }
    return {
        "status": "ok"
    }


def finish_pending(pending_id, approval):
    """
    Finish pending change with approval or deny
    :param pending_id:  ID of pending change
    :param approval:    True or False
    :return:
    """
    pending = db.session.get(StopPending, pending_id)
    if pending is None:
        return {
            "status": "error",
            "reason": "This pending request does not exist."
        }
    else:
        if approval:
            pending.change_state = "accepted"
            if pending.change_type == "create":
                result = create_stop(pending.name, pending.type, pending.lat, pending.long)
                return result
            elif pending.change_type == "delete":
                stop = db.session.get(Stop, pending.stop_id)
                if stop is None:
                    return {
                        "status": "error",
                        "result": "Stop does not exist."
                    }
                else:
                    db.session.delete(stop)
                    db.session.commit()
                    expired_pendings = StopPending.query.filter_by(stop_id=pending.stop_id).all()
                    print(expired_pendings)
                    for p in expired_pendings:
                        db.session.delete(p)
                        db.session.commit()
                    return {
                        "status": "ok"
                    }

            elif pending.change_type == "change":
                stop = db.session.get(Stop, pending.stop_id)
                if stop is None:
                    return {
                        "status": "error",
                        "result": "Stop does not exist."
                    }
                else:
                    stop.name = pending.name if pending.name is not None else stop.name
                    stop.type = pending.type if pending.type is not None else stop.type
                    stop.long = pending.long if pending.long is not None else stop.long
                    stop.lat = pending.lat if pending.lat is not None else stop.lat
                    db.session.commit()
                    return {
                        "status": "ok"
                    }
        else:
            pending.change_state = "rejected"
            db.session.commit()
            return {
                "status": "ok"
            }


def edit_stop(stop_id, name=None, stop_type=None, lat=None, long=None):
    """
    Edits not-None fields of stop
    :param stop_id:
    :param name:
    :param stop_type:
    :param lat:
    :param long:
    :return:
    """
    stop = db.session.get(Stop, stop_id)
    if stop is None:
        return {
            "status": "error",
            "reason": "Stop does not exist."
        }
    stop.name = name if name is not None else stop.name
    stop.type = stop_type if stop_type is not None else stop.type
    stop.long = parse_long(long) if long is not None else stop.long
    stop.lat = parse_lat(lat) if lat is not None else stop.lat
    db.session.commit()
    return {
        "status": "ok"
    }

def get_stop_data(stop_id):
    """
    Returns stop object based on stops id
    :param stop_id:    id of stop
    :return:
    """
    return db.session.get(Stop, stop_id)


def delete_stop(stop_id):
    """
    Deletes stop based on stops id
    :param stop_id:    id of stop
    :return:
    """
    stop = get_stop_data(stop_id)
    if stop is None:
        return {
            "status": "error",
            "reason": "This stop does not exist."
        }
    else:
        db.session.delete(stop)
        db.session.commit()
        expired_pendings = StopPending.query.filter_by(stop_id=stop_id).all()
        for p in expired_pendings:
            db.session.delete(p)
            db.session.commit()
        return {
            "status": "ok"
        }


def parse_lat(lat_str):
    """
    Conversion between user-friendly and database-friendly format of GPS latitude
    :param lat_str:     user-format of latitude
    :return:            db-format of latitude
    """
    latf = float(lat_str[:-2])
    if lat_str[-1] == "S":
        latf = latf * -1
    return latf


def parse_long(long_str):
    """
    Conversion between user-friendly and database-friendly format of GPS longitude
    :param long_str:     user-format of longitude
    :return:            db-format of longitude
    """
    longf = float(long_str[:-2])
    if long_str[-1] == "W":
        longf = longf * -1
    return longf
