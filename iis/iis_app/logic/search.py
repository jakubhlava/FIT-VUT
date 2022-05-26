from iis_app import db
from iis_app.model import Connection, Stop, StopTimed
import datetime
from iis_app.logic.connection import weekdays

def get_connection_list(frm, to, date, time):
    """
    Returns a list of valid connections.
    :param frm:         name of starting station
    :param to:          name of ending station
    :param date:        date
    :param time:        time
    :return:            returns list of Connection objects that comply to restrictions specified by previous parameters
    """
    frm_stop = Stop.query.filter_by(name=frm).first()
    to_stop = Stop.query.filter_by(name=to).first()
    date = datetime.datetime.strptime(date, "%Y-%m-%d")
    day = date.weekday()
    conns = Connection.query.filter(db.text(f"{weekdays[day]}::bit(8) & flags > B'00000000'"))\
    .filter(db.text(f"EXISTS(SELECT stop_id from stop_timed st WHERE connection.connection_id=st.connection_id "
                    f"AND st.stop_id = {frm_stop.stop_id} AND departure >= '{time}' AND EXISTS("
                    f"SELECT stop_id FROM stop_timed st2 "
                    f"WHERE connection.connection_id=st2.connection_id "
                    f"AND st2.stop_id = {to_stop.stop_id} AND st2.index>st.index))")).all()
    return conns, frm_stop, to_stop, date
