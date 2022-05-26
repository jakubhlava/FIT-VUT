using System;
using System.Threading.Tasks;

namespace RideSharing.App.ViewModels.Interfaces;

public interface IRideFinderListViewModel : IViewModel
{
    Task LoadAsync(string from_, string to_, DateTime date);
}