from iis_app import db
from sqlalchemy.dialects.postgresql import ARRAY


class Reservation(db.Model):

    reservation_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # status of reservation ("pending", "approved", "rejected")
    status = db.Column(db.String(16), nullable=False)
    # sum cost of reservation
    cost = db.Column(db.Integer, nullable=False)
    # starting and ending indexes of stop in connection (for reservation of portion of route)
    start_index = db.Column(db.Integer, nullable=False)
    end_index = db.Column(db.Integer, nullable=False)
    reservation_date = db.Column(db.Date, nullable=False)
    # array of seat ids related to this reservation
    seats = db.Column(ARRAY(db.Integer))
    connection = db.relationship("Connection", back_populates="reservations")
    connection_id = db.Column(db.Integer, db.ForeignKey("connection.connection_id", ondelete="CASCADE"), nullable=False)
    user = db.relationship("User", back_populates="reservations")
    user_id = db.Column(db.Integer, db.ForeignKey("user.user_id", ondelete="CASCADE"), nullable=False)