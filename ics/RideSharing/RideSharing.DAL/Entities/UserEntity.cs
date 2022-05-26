using System.ComponentModel.DataAnnotations.Schema;

namespace RideSharing.DAL.Entities;

[Table("Users")]
public record UserEntity(
    Guid Id,
    string Name,
    string Surname,
    string? UserImg,
    string Number
    ) : IEntity
{
    // Workaround for Automapper's need for parameter-less contructor
#nullable disable
    public UserEntity() : this(default, default, default, default, default) { }
#nullable enable
    public ICollection<CarEntity>? OwnedCars { get; init; } = new List<CarEntity>();
    public ICollection<RideEntity>? DriverRides { get; init; } = new List<RideEntity>();
    public ICollection<RideEntity>? PassengerRides { get; init; } = new List<RideEntity>();
}
