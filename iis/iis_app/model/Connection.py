from iis_app import db
from sqlalchemy.dialects.postgresql import BIT

class Connection(db.Model):

    # id of connection
    connection_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # flags for repeating connections TODO flag specification
    flags = db.Column(BIT(8))
    # carrier pretty-name
    carrier = db.Column(db.Integer, db.ForeignKey("user.user_id", ondelete="CASCADE"), nullable=False)
    carrier_user = db.relationship("User", back_populates="carrier_connections")
    # vehicle, which is used in a connection
    vehicle_id = db.Column(db.Integer, db.ForeignKey("vehicle.vehicle_id"), nullable=False)
    vehicle_object = db.relationship("Vehicle", back_populates="connections")
    # list of stops related to this connection in order
    stops = db.relationship("StopTimed", back_populates="connection", passive_deletes=True)
    # list of reservations related to this connection independent of date
    reservations = db.relationship("Reservation", back_populates="connection", lazy=True, passive_deletes=True)

