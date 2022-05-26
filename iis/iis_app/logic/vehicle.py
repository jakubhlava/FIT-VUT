from iis_app.model import Vehicle, VehicleSection, VehicleSectionSeat
from iis_app import db


def paginate_vehicles(page=1, per_page=10, carrier=None):
    """
    Returns paginated list of vehicles with filter for carrier
    :param page:        page number
    :param per_page:    number of entries per page
    :param carrier:     carrier id or None to disable filter
    :return:            one page of entries (list)
    """
    q = Vehicle.query
    if carrier is not None:
        q = q.filter_by(carrier=carrier)
    return q.order_by(db.asc(Vehicle.name)).paginate(page, per_page)


def create_vehicle(name, type_, carrier, sections):
    """
    Creates vehicle, vehiclesection and vehiclesectionseat entries in database
    :param name:        name of vehicle
    :param type_:       type of vehicle ("train", "bus", "tram", "plane")
    :param carrier:     ID of carrier, who owns this vehicle
    :param sections:    list fo sections in format {"name": name of section, "index": order in vehicle, "seats": seat count}
    :return:            result with status field, in case of status: error also with reason field
    """
    new_vehicle = Vehicle(name=name, type=type_, carrier=carrier)
    db.session.add(new_vehicle)
    db.session.commit()
    for s in sections:
        new_sec = VehicleSection(name=s["name"], type="basic", index=s["index"], vehicle_id=new_vehicle.vehicle_id)
        db.session.add(new_sec)
        db.session.commit()
        for seat in range(s["seats"]):
            new_seat = VehicleSectionSeat(index=seat, vehicle_section_id=new_sec.vehicle_section_id)
            db.session.add(new_seat)
        db.session.commit()
    return {
        "status": "ok"
    }


def delete_vehicle(vehicle_id):
    """
    Removes vehicle from database, cascade deletion deletes rest of vehicle automatically
    :param vehicle_id:  id of vehicle
    :return:            result with status field, in case of status: error also with reason field
    """
    vehicle = db.session.get(Vehicle, vehicle_id)
    if vehicle is None:
        return {
            "status": "error",
            "reason": "This vehicle does not exist."
        }
    db.session.delete(vehicle)
    db.session.commit()
    return {
        "status": "ok"
    }