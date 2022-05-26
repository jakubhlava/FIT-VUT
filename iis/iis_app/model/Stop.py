from iis_app import db


class Stop(db.Model):

    stop_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # stop name
    name = db.Column(db.String(64), unique=True)
    # stop type (bus, train)
    type = db.Column(db.String(16), nullable=False)
    # GPS coords
    lat = db.Column(db.Float, nullable=False)
    long = db.Column(db.Float, nullable=False)
    timetable = db.relationship("StopTimed", back_populates="static_stop", lazy=True)