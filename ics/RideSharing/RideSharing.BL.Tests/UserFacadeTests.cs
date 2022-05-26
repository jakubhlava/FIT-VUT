using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using RideSharing.Common.Tests;
using RideSharing.Common.Tests.Seeds;
using RideSharing.DAL.Entities;
using System.Linq;
using System.Threading.Tasks;
using Xunit;
using Xunit.Abstractions;

namespace RideSharing.BL.Tests;

public sealed class UserFacadeTests : CRUDFacadeTestsBase
{
    private readonly UserFacade _userFacadeSUT;

    public UserFacadeTests(ITestOutputHelper output) : base(output)
    {
        _userFacadeSUT = new UserFacade(UnitOfWorkFactory, Mapper);
    }

    [Fact]
    public async Task GetAll_Single_SeededFranta()
    {
        var users = await _userFacadeSUT.GetAsync();
        var user = users.Single(i => i.Id == UserSeeds.Franta.Id);

        DeepAssert.Equal(Mapper.Map<UserEntity, UserListModel>(UserSeeds.Franta), user);
    }
}