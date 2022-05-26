using System;
using System.Threading.Tasks;

namespace RideSharing.App.ViewModels.Interfaces;

public interface IRideCreateEditViewModel
{
    public Task LoadAsync(Guid userId, Guid rideId);
    public Task SaveAsync();
}