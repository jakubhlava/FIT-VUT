from flask import flash
from iis_app import db
from iis_app.model import User
from flask_login import login_user, logout_user, current_user
from sqlalchemy.exc import IntegrityError, DataError
from datetime import timedelta
from werkzeug.security import generate_password_hash, check_password_hash


def login(username, pwd, remember=False):
    """
    Logs user in
    :param username:    username of user, who tries to log in
    :param pwd:         password from login form
    :param remember:    optional, if user wants "remembered" login
    :return:            dict with "status" and optional "reason" field
                        status holds either "ok" or "error" depending on registration result
                        when status is "error", "reason" field holds reasoning for that error
    """
    user = User.query.filter_by(username=username).first()
    if user is None:
        return {
            "status": "error",
            "reason": "incorrect username or password"
        }
    else:
        if check_password_hash(user.password, pwd):
            login_user(user, remember=remember, duration=timedelta(days=1))
            return {
                "status": "ok"
            }
        else:
            return {
                "status": "error",
                "reason": "incorrect username or password"
            }


def logout():
    """
    Logs user out
    :return:    dict with "status" and optional "reason " field
                status holds either "ok" or "error" depending on registration result
                when status is "error", "reason" field holds reasoning for that error
    """
    if not current_user.is_authenticated:
        return {
            "status": "error",
            "reason": "user not logged in"
        }
    logout_user()
    return {
        "status": "ok"
    }


def register(username, pwd, email, role="user", **kwargs):
    """
    Registers new user to system.
    :param username:    username of new user
    :param pwd:         user's password
    :param email:       user's email
    :param role:        user role if it is known (default is user)
                        valid options: "admin", "carrier", "staff", "user", "unregistered"
    :param kwargs:      optional args "full_name" and "phone" - user's full name and phone number
    :return:            dict with "status" and optional "reason", "user_id" fields
                        status holds either "ok" or "error" depending on registration result
                        when status is "error", "reason" field holds reasoning for that error,
                        "user_id" field holds users id if registration was successfull
    """
    new_user = User(username=username, password=generate_password_hash(pwd) if pwd != None else None, email=email, role=role)
    for opt_arg in ["full_name", "phone"]:
        if opt_arg in kwargs.keys() and kwargs[opt_arg] is not None:
            setattr(new_user, opt_arg, kwargs[opt_arg])
    db.session.add(new_user)
    if "carrier" in kwargs.keys() and kwargs["carrier"] is not None:
        new_user.carrier = kwargs["carrier"]
    try:
        db.session.commit()

        return {
            "status": "ok",
            "user_id": User.query.filter_by(email=email).first().user_id,
            "role": User.query.filter_by(email=email).first().role
        }
    except IntegrityError as e:
        db.session.rollback()
        if "user_email_key" in str(e):
            return {
                "status": "error",
                "reason": f"User with this email already exists!"
            }
        else:
            return {
                "status": "error",
                "reason": f"User with this username already exists!"
            }
    except DataError as e:
        db.session.rollback()
        return {
            "status": "error",
            "reason": """<br>Your inputs are not valid probably because some of them are too long!<br>
                      Limits are:<br>
                      <ul>
                      <li>Username: 64 characters</li>
                      <li>Full name: 64 characters</li>
                      <li>Email: 64 characters</li>
                      <li>Telephone: 16 characters</li>
                      </ul>
                      We thought it was enough, but you proved us wrong! :( Please report this to developers!"""
        }

