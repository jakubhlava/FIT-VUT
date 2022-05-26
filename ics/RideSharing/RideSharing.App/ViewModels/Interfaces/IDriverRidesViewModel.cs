using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RideSharing.App.Wrappers;

namespace RideSharing.App.ViewModels
{
    public interface IDriverRidesViewModel : IViewModel
    {
        Task LoadAsync(Guid id);
    }
}
