from flask_login import current_user
from functools import wraps
from flask import abort


def restrict_roles(roles):
    """
    Decorator for restricting access to pages depending on user role
    :param roles:  string with role name or list of allowed roles
    """
    def decorator(f):
        @wraps(f)
        def wrapper(*args, **kwargs):
            if not current_user.is_authenticated:
                ulevel = "unregistered"
            else:
                ulevel = current_user.role
            if type(roles) is list:
                if ulevel not in roles:
                    abort(403)
            else:
                if ulevel != roles:
                    abort(403)
            return f(*args, **kwargs)
        return wrapper
    return decorator
