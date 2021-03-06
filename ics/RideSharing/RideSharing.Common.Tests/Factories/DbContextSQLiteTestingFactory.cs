using Microsoft.EntityFrameworkCore;
using RideSharing.DAL;

namespace RideSharing.Common.Tests.Factories;

public class DbContextLocalDBTestingFactory : IDbContextFactory<RideSharingDbContext>
{
    private readonly string _databaseName;
    private readonly bool _seedTestingData;

    public DbContextLocalDBTestingFactory(string databaseName, bool seedTestingData = false)
    {
        _databaseName = databaseName;
        _seedTestingData = seedTestingData;
    }
    public RideSharingDbContext CreateDbContext()
    {
        DbContextOptionsBuilder<RideSharingDbContext> builder = new();
        builder.UseSqlServer($"Data Source=(LocalDB)\\MSSQLLocalDB;Initial Catalog = {_databaseName};MultipleActiveResultSets = True;Integrated Security = True;");
        builder.EnableSensitiveDataLogging();

        return new RideSharingTestingDbContext(builder.Options, _seedTestingData);
    }
}