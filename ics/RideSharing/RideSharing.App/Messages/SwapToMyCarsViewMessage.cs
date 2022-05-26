using RideSharing.BL.Models;

namespace RideSharing.App.Messages
{
    public record SwapToMyCarsViewMessage<T> : Message<T>
        where T : IModel
    {
    }
}
