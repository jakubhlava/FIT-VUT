from iis_app import db
from iis_app.model import User
from werkzeug.security import generate_password_hash
import json


def get_user_pages(page=1, per_page=10, carrier=None):
    """
    Returns one page of user object for rendering ón frontend
    :param page:        page number
    :param per_page:    number of users to show on one page
    :return:            list of Users
    """
    q = User.query.filter(User.username.is_not(None))
    if carrier is not None:
        q = q.filter_by(carrier=carrier)
    return q.paginate(page, per_page)


def get(user_id):
    """
    Helper function for getting user mased on his ID
    :param user_id:     ID of queried user
    :return:            User object or None if ID does not belong to any user
    """
    return db.session.get(User, user_id)


def remove(user_id):
    """
    Removes user from system
    :param user_id:     ID of removed user
    :return:            status dict message with status key, in case of status: error, also reason field
    """
    user = db.session.get(User, user_id)
    if user is None:
        return {
            "status": "error",
            "reason": "User with given ID does not exist."
        }
    else:
        db.session.delete(user)
        db.session.commit()
        return {
            "status": "ok"
        }


def edit(user_id, **fields):
    """
    Edits user information
    :param user_id:     ID of user
    :param fields:      fields which should be edited as per iis_app.model.User
    :return:            status message with status field, and in case of status: error olso reason field
    """
    user = db.session.get(User, user_id)
    if user is None:
        return {
            "status": "error",
            "reason": "User with given ID does not exist."
        }
    for f in fields.keys():
        if fields[f] is not None:
            if f == "password":
                setattr(user, f, generate_password_hash(fields[f]))
            else:
                setattr(user, f, fields[f])
    db.session.commit()
    return {
        "status": "ok"
    }


def get_carriers(list=False):
    """
    Returns list of carrier users for further usage in admin panel
    :param list:    if True returns users as list, if False returns json.dumps string of said list
    :return:        list of carrier users
    """
    carriers = [{"user_id": c.user_id, "full_name": c.full_name} for c in User.query.filter_by(role="carrier").all()]
    if list:
        return carriers
    else:
        return json.dumps(carriers)
