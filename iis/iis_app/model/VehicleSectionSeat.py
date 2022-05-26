from iis_app import db


class VehicleSectionSeat(db.Model):

    vehicle_section_seat_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # index in vehicle section
    index = db.Column(db.Integer, nullable=False)
    vehicle_section_id = db.Column(db.Integer, db.ForeignKey("vehicle_section.vehicle_section_id", ondelete="CASCADE"), nullable=False)