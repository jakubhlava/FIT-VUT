using Microsoft.EntityFrameworkCore;
using RideSharing.Common.Tests;
using RideSharing.DAL.Entities;
using System;
using System.Threading.Tasks;
using Xunit;
using Xunit.Abstractions;

namespace RideSharing.DAL.Tests;

public class DbContextUserTests : DbContextTestsBase
{
    public DbContextUserTests(ITestOutputHelper output) : base(output)
    {
    }

    [Fact]
    public async Task AddNew_User_Persisted()
    {
        UserEntity Vasek = new(
            Guid.Parse("574d81ee-abd4-4605-900f-c2f3af497024"),
            Name: "Vašek",
            Surname: "Programátor",
            UserImg: String.Empty,
            Number: "+420608987654"
            );

        RideSharingDbContextSUT.Users.Add(Vasek);
        await RideSharingDbContextSUT.SaveChangesAsync();

        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualVasek = await dbx.Users.SingleAsync(i => i.Id == Vasek.Id);
        DeepAssert.Equal(Vasek, actualVasek);
    }


    [Fact]
    public async Task ManyToMany_Test()
    {

    }
}