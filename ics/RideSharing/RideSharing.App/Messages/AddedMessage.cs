using RideSharing.BL.Models;

namespace RideSharing.App.Messages
{
    public record AddedMessage<T> : Message<T>
        where T : IModel
    {
    }
}