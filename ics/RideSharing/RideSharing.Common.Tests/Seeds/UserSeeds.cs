//using RideSharing.Common.Enums;

using Microsoft.EntityFrameworkCore;
using RideSharing.DAL.Entities;

namespace RideSharing.Common.Tests.Seeds;

public static class UserSeeds
{

    public static readonly UserEntity EmptyUserEntity = new(
        Id: default,
        Name: default!,
        Surname: default!,
        Number: default!,
        UserImg: default);

    public static readonly UserEntity Petr = new(
        Id: Guid.Parse("335b5aee-6f04-4699-a1b4-76f07222f039"),
        Name: "Petr",
        Surname: "Uživatel",
        Number: "+420777777123",
        UserImg: String.Empty);

    public static readonly UserEntity PetrUpdate = Petr with { Id = Guid.Parse("6755fcb5-85df-4896-9e57-bf53dec8d71b") };
    public static readonly UserEntity PetrDelete = Petr with { Id = Guid.Parse("7459a94c-bd88-484c-bfb9-5e2d4e1aed97") };

    public static readonly UserEntity Franta = new(
        Id: Guid.Parse("acbe58cc-bea9-4955-a10d-bead1a0b9224"),
        Name: "Franta",
        Surname: "Testovací",
        Number: "+420790123456",
        UserImg: String.Empty
    );

    public static readonly UserEntity Karel = new(
        Id: Guid.Parse(input: "fabde0cd-eefe-443f-baf6-3d96cc2cbf2e"),
        Name: "Karel",
        Surname: "Novák",
        Number: "603456789",
        UserImg: String.Empty);

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<UserEntity>().HasData(
            Petr,
            Franta,
            Karel
        );
    }
}
