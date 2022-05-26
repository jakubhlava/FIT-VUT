using RideSharing.App.ViewModels.Interfaces;
using System;

namespace RideSharing.App.Messages
{
    public record SwapToFindRideViewMessage(
        IMainPageViewModel mpvm) : IMessage
    {
        public Guid? TargetId { get; init; }
    }
}
