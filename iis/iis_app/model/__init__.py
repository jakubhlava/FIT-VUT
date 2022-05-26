# those imports are needed for auto-creation of database model on first start
# it basically imports all model classes during import "from iis_app.model import *"

from .Connection import Connection
from .Reservation import Reservation
from .Stop import Stop
from .StopTimed import StopTimed
from .StopPending import StopPending
from .User import User
from .Vehicle import Vehicle
from .VehicleSection import VehicleSection
from .VehicleSectionSeat import VehicleSectionSeat
