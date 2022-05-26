from iis_app import db


# smaller part of vehicle, e.g. first and second class parts or train wagon
class VehicleSection(db.Model):

    vehicle_section_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # carrier's identification for part
    name = db.Column(db.String(64))
    type = db.Column(db.String(16))
    # index inside of vehicle
    index = db.Column(db.Integer, nullable=False)
    vehicle_id = db.Column(db.Integer, db.ForeignKey("vehicle.vehicle_id", ondelete="CASCADE"), nullable=False)
    seats = db.relationship("VehicleSectionSeat", backref="vehicle_section", lazy=True, passive_deletes=True)
