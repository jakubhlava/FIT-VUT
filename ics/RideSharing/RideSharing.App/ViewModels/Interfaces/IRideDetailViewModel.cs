using System;
using System.Threading.Tasks;

namespace RideSharing.App.ViewModels.Interfaces
{
    public interface IRideDetailViewModel : IViewModel
    {
        //RideWrapper  Ride { get; }
        Task LoadAsync(Guid? rideId, Guid userId);
    }
}
