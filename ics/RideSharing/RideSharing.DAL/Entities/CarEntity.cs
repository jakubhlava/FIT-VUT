using System.ComponentModel.DataAnnotations.Schema;

namespace RideSharing.DAL.Entities;

[Table("Cars")]
public record CarEntity(
        Guid Id,
        string Brand,
        string Model,
        string Type,
        DateTime RegistrationDate,
        string? Img,
        int SeatsCount,
        string? Note,
        Guid CarOwnerId
    ) : IEntity
{
    // Workaround for Automapper's need for parameter-less contructor
#nullable disable
    public CarEntity() : this(default, default, default, default,
        default, default, default, default, default)
    { }
#nullable enable
    public UserEntity? CarOwner { get; init; }
    public ICollection<RideEntity>? Rides { get; init; } = new List<RideEntity>();
}
