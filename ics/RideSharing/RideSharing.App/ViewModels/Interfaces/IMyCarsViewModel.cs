using RideSharing.App.Wrappers;
using System;
using System.Threading.Tasks;

namespace RideSharing.App.ViewModels.Interfaces
{
    public interface IMyCarsViewModel : IViewModel
    {
        UserWrapper User { get; }
        Task LoadAsync(Guid id);
    }
}
