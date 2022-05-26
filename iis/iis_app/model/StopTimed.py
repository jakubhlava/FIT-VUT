from iis_app import db


# link between concrete day, time and repeating connection
class StopTimed(db.Model):

    stop_timed_id = db.Column(db.Integer, nullable=False, primary_key=True, autoincrement=True)
    # link to permanent stop
    stop_id = db.Column(db.Integer, db.ForeignKey("stop.stop_id", ondelete="CASCADE"), nullable=False)
    static_stop = db.relationship("Stop", back_populates="timetable")
    # departure time
    departure = db.Column(db.Time, nullable=False)
    # index in connection
    index = db.Column(db.Integer, nullable=False)

    price = db.Column(db.Integer, nullable=False)
    
    connection_id = db.Column(db.Integer, db.ForeignKey("connection.connection_id", ondelete="CASCADE"), nullable=False)
    
    connection = db.relationship("Connection", back_populates="stops")
