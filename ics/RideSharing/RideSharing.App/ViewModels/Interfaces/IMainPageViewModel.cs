using RideSharing.App.Wrappers;
using System;
using System.Threading.Tasks;
using RideSharing.BL.Models;

namespace RideSharing.App.ViewModels.Interfaces
{
    public interface IMainPageViewModel : IViewModel
    {
        UserDetailModel? User { get; }
        Task LoadAsync(Guid userId);
    }
}
