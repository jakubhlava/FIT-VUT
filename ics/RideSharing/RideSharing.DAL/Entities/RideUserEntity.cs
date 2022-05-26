namespace RideSharing.DAL.Entities;

public record RideUserEntity (Guid? UserId, Guid? RideId)
{
    public RideUserEntity() : this(default, default)
    {
    }

    public UserEntity? User { get; init; }
    public RideEntity? Ride { get; init; }
}