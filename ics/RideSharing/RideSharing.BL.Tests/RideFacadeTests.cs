using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using RideSharing.Common.Tests;
using RideSharing.Common.Tests.Seeds;
using RideSharing.DAL.Entities;
using System;
using System.Linq;
using System.Threading.Tasks;
using Xunit;
using Xunit.Abstractions;

namespace RideSharing.BL.Tests;

public class RideFacadeTests : CRUDFacadeTestsBase
{
    private readonly RideFacade _rideFacadeSUT;

    public RideFacadeTests(ITestOutputHelper output) : base(output)
    {
        _rideFacadeSUT = new RideFacade(UnitOfWorkFactory, Mapper);
    }

    [Fact]
    public async Task GetFilteredRide()
    {
        var rides = await _rideFacadeSUT.GetFilteredAsync("Brno", "Prague", new DateTime(2022, 05, 01, 00, 00, 00));
        var ride = rides.Single(i => i.Id == RideSeeds.Ride1.Id);
        DeepAssert.Equal(Mapper.Map<RideEntity, RideListModel>(RideSeeds.Ride1), ride);
    }
}