from iis_app import db


class Vehicle(db.Model):

    vehicle_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # carrier's indetification of vehicle
    name = db.Column(db.String(64))
    # bus or train
    type = db.Column(db.String(16))
    # last seen GPS coords
    lat = db.Column(db.Float)
    long = db.Column(db.Float)
    carrier = db.Column(db.Integer, db.ForeignKey("user.user_id"), nullable=False)
    carrier_user = db.relationship("User", back_populates="carrier_vehicles")
    sections = db.relationship("VehicleSection", backref="vehicle", lazy=True, passive_deletes=True)
    connections = db.relationship("Connection", back_populates="vehicle_object", lazy=True)
