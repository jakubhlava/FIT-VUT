using Microsoft.EntityFrameworkCore;
using RideSharing.DAL.Entities;

namespace RideSharing.Common.Tests.Seeds;

public static class RideSeeds
{
    public static readonly RideEntity Ride1 = new(
        Id: Guid.Parse(input: "91ea928b-6495-4fb5-bc6a-6ab31ae6dd03"),
        CityFrom: "Brno",
        CityTo: "Praha",
        DepartureTime: new DateTime(2022, 06, 15, 15, 00, 00),
        ArrivalTime: new DateTime(2022, 06, 15, 18, 00, 00),
        Note: String.Empty,
        DriverId: UserSeeds.Petr.Id,
        CarId: CarSeeds.Octavia.Id)
    {
        Driver = UserSeeds.Petr,
        Car = CarSeeds.Octavia
    };

    static RideSeeds()
    {
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<RideEntity>().HasData(
            Ride1 with { Driver = null, Car = null }
        );
    }
}