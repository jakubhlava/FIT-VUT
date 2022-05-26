using Microsoft.EntityFrameworkCore;
using RideSharing.DAL.Entities;
using RideSharing.DAL.Seeds;

namespace RideSharing.DAL
{
    public class RideSharingDbContext : DbContext
    {
        private readonly bool _seedDemoData;
        public RideSharingDbContext(DbContextOptions contextOptions, bool seedDemoData = false)
            : base(contextOptions)
        {
            _seedDemoData = seedDemoData;
        }

        public DbSet<CarEntity> Cars => Set<CarEntity>();
        public DbSet<RideEntity> Rides => Set<RideEntity>();
        public DbSet<UserEntity> Users => Set<UserEntity>();

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.Entity<CarEntity>()
                .HasOne(i => i.CarOwner)
                .WithMany(i => i.OwnedCars);

            modelBuilder.Entity<RideEntity>()
                .HasOne(i => i.Car)
                .WithMany(i => i.Rides);

            modelBuilder.Entity<RideEntity>()
                .HasOne(i => i.Driver)
                .WithMany(i => i.DriverRides);

            modelBuilder.Entity<RideEntity>()
                .HasMany(i => i.Passengers)
                .WithMany(i => i.PassengerRides)
                .UsingEntity<RideUserEntity>(
                    ride => ride.HasOne(ru => ru.User)
                        .WithMany().HasForeignKey(ru => ru.UserId).OnDelete(DeleteBehavior.NoAction),
                    user => user.HasOne(ru => ru.Ride)
                        .WithMany(),
                    join => join.ToTable("RideUser"));

            if (_seedDemoData)
            {
                UserSeeds.Seed(modelBuilder);
                CarSeeds.Seed(modelBuilder);
                RideSeeds.Seed(modelBuilder);
            }
        }
    }
}

