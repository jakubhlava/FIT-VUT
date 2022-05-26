using System.ComponentModel.DataAnnotations.Schema;

namespace RideSharing.DAL.Entities;

[Table("Rides")]
public record RideEntity(
    Guid Id,
    string CityFrom,
    string CityTo,
    DateTime DepartureTime,
    DateTime ArrivalTime,
    string? Note,
    Guid? DriverId,
    Guid CarId
    ) : IEntity
{

    public RideEntity() : this(default, default, default, default, default, default, default, default) { }
    public UserEntity? Driver { get; init; }
    public CarEntity? Car { get; init; }
    public ICollection<UserEntity>? Passengers { get; init; } = new List<UserEntity>();
}
