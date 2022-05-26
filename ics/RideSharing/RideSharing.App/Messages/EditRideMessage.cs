using System;
using RideSharing.BL.Models;

namespace RideSharing.App.Messages
{
    public record EditRideMessage
        (Guid RideId)
        : IMessage
    {
        public Guid RideId { get; set; } = RideId;
    }
}
