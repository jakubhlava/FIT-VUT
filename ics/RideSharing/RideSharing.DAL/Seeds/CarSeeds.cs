using Microsoft.EntityFrameworkCore;
using RideSharing.DAL.Entities;

namespace RideSharing.DAL.Seeds;

public static class CarSeeds
{
    public static readonly CarEntity EmptyCar = new(
        Id: Guid.Empty,
        Brand: default!,
        Model: default!,
        Type: default!,
        RegistrationDate: default,
        Img: default,
        SeatsCount: default,
        Note: default,
        CarOwnerId: Guid.Empty
    );

    public static readonly CarEntity Octavia = new(
        Id: Guid.Parse("4ad19510-bfc5-440c-9e91-5c25c1959759"),
        Brand: "Škoda",
        Model: "Octavia III",
        Type: "Combi",
        RegistrationDate: new DateTime(2015, 07, 01),
        Img: "https://img.auto.cz/foto/nizsi-stredni-trida/NjkweDQyNS9jZW50ZXIvbWlkZGxlL3NtYXJ0L2ZpbHRlcnM6cXVhbGl0eSg4NSkvaW1n/3965913.jpg?v=0&st=NrnvC-jx8EcY40a7xcZ9IHOWeJg4Rt-nj1frhLk7u-E&ts=1600812000&e=0",
        SeatsCount: 5,
        Note: String.Empty,
        CarOwnerId: UserSeeds.Petr.Id)
    {
        CarOwner = UserSeeds.Petr
    };

    public static readonly CarEntity OctaviaUpdate = Octavia with { Id = Guid.Parse("95024646-4626-4a64-9eaa-c33161d2b70a") };
    public static readonly CarEntity OctaviaDelete = Octavia with { Id = Guid.Parse("f3b7678c-bca9-467b-8e3f-402aa55ea3e1") };

    public static readonly CarEntity Corsa = new(
        Id: Guid.Parse("475c3961-0807-4471-9f7c-4a9eb6a080d1"),
        Brand: "Opel",
        Model: "Corsa C",
        Type: "Hatchback",
        RegistrationDate: new DateTime(2001, 04, 25),
        Img: String.Empty,
        SeatsCount: 5,
        Note: String.Empty,
        CarOwnerId: UserSeeds.Franta.Id);

    static CarSeeds()
    {
        Octavia.Rides.Add(RideSeeds.Ride1);
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<CarEntity>().HasData(
            Octavia with { CarOwner = null, Rides = Array.Empty<RideEntity>() },
            Corsa
        );
    }
}