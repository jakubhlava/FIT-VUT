from iis_app import db
from flask_login import UserMixin


class User(db.Model, UserMixin):

    user_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # login credentials
    username = db.Column(db.String(64), unique=True)
    password = db.Column(db.String(128))
    # contact email
    email = db.Column(db.String(64), nullable=False, unique=True)
    # phone is not required but user can fill in one
    phone = db.Column(db.String(16))
    # users full name, for better communication
    full_name = db.Column(db.String(64))
    # users role - valid roles are "admin", "carrier", "personnel", "user", "unregistered"
    role = db.Column(db.String(16), nullable=False)
    reservations = db.relationship("Reservation", back_populates="user", lazy=True)
    proposed_changes = db.relationship("StopPending", backref="user", lazy=True)
    carrier_connections = db.relationship("Connection", back_populates="carrier_user", lazy=True)
    carrier_vehicles = db.relationship("Vehicle", back_populates="carrier_user", lazy=True)

    # workaround for flask-login
    def get_id(self):
        return self.user_id


User.carrier = db.Column(db.Integer, db.ForeignKey(User.user_id))
User.staff = db.relationship(User, backref="user", lazy=True, remote_side=User.user_id)
