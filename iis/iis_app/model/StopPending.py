from iis_app import db


# pending request for stop creation/change/deletion made by carrier
class StopPending(db.Model):

    change_id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    # type of change ("create", "change", "delete")
    change_type = db.Column(db.String(16), nullable=False)
    # state of change ("pending", "accepted", "rejected")
    change_state = db.Column(db.String(16), nullable=False, default="pending")
    # new stop name (or None if there is no proposed change)
    name = db.Column(db.String(64))
    # new stop type
    type = db.Column(db.String(16))
    # new stop coords
    lat = db.Column(db.Float)
    long = db.Column(db.Float)
    # carrier who proposed the change
    user_id = db.Column(db.Integer, db.ForeignKey("user.user_id"))
    stop_id = db.Column(db.Integer)
