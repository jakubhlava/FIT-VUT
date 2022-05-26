using System;

namespace RideSharing.App.Messages;


public record ChangedRideParametersMessage(string from_, string to_, DateTime date) : IMessage
{
    public Guid? TargetId { get; init; }
}
