using System;
using System.Threading.Tasks;

namespace RideSharing.App.ViewModels.Interfaces
{
    public interface IMyCarsEditViewModel : IViewModel
    {
        Task LoadAsync(Guid UserId, Guid CarId);
        Task SaveAsync();
    }
}
