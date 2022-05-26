using Microsoft.EntityFrameworkCore;
using RideSharing.Common.Tests.Seeds;
using RideSharing.DAL;

namespace RideSharing.Common.Tests
{
    public class RideSharingTestingDbContext : RideSharingDbContext
    {
        private readonly bool _seedTestingData;

        public RideSharingTestingDbContext(DbContextOptions contextOptions, bool seedTestingData = false)
            : base(contextOptions, seedDemoData: false)
        {
            _seedTestingData = seedTestingData;
        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            if (_seedTestingData)
            {
                UserSeeds.Seed(modelBuilder);
                CarSeeds.Seed(modelBuilder);
                RideSeeds.Seed(modelBuilder);
            }
        }
    }
}
