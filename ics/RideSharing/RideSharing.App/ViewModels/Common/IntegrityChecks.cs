using System.Linq;
using RideSharing.BL.Models;

namespace RideSharing.App.ViewModels.Common;

public class IntegrityChecks
{
    public static bool CheckCollisionsBetweenUserAndRide(UserDetailModel user, RideDetailModel checkedRide)
    {
        var passengerCollisions =
            from ride in user.PassengerRides
            where (checkedRide.DepartureTime < ride.DepartureTime && ride.DepartureTime < checkedRide.ArrivalTime)
                  || (checkedRide.DepartureTime < ride.ArrivalTime && ride.ArrivalTime < checkedRide.ArrivalTime)
                  || (checkedRide.DepartureTime < ride.DepartureTime && ride.ArrivalTime < checkedRide.ArrivalTime)
                  || (checkedRide.DepartureTime >= ride.DepartureTime && ride.ArrivalTime >= checkedRide.ArrivalTime)
                  && checkedRide.Id != ride.Id
            select ride;
        if (passengerCollisions.Any()) return false;
        var driverCollisions =
            from ride in user.DriverRides
            where (checkedRide.DepartureTime < ride.DepartureTime && ride.DepartureTime < checkedRide.ArrivalTime)
                  || (checkedRide.DepartureTime < ride.ArrivalTime && ride.ArrivalTime < checkedRide.ArrivalTime)
                  || (checkedRide.DepartureTime < ride.DepartureTime && ride.ArrivalTime < checkedRide.ArrivalTime)
                  || (checkedRide.DepartureTime >= ride.DepartureTime && ride.ArrivalTime >= checkedRide.ArrivalTime)
                  && checkedRide.Id != ride.Id
            select ride;
        if (driverCollisions.Any()) return false;
        return true;
    }
}