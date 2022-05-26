using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using RideSharing.App.Commands;
using RideSharing.App.Extensions;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.App.Wrappers;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;


namespace RideSharing.App.ViewModels
{
    public class DriverRidesViewModel : ViewModelBase, IDriverRidesViewModel
    {
        private readonly RideFacade _rideFacade;
        private readonly UserFacade _userFacade;
        private readonly IMediator _mediator;
        public ICommand RideSelectedCommand { get; }

        public Guid? UserId;

        public UserWrapper? User { get; private set; }

        public ObservableCollection<RideListModel> DriverRides { get; set; } = new();

        public DriverRidesViewModel(RideFacade rideFacade, UserFacade userFacade, IMediator mediator)
        {
            _userFacade = userFacade;
            _rideFacade = rideFacade;
            _mediator = mediator;

            RideSelectedCommand = new RelayCommand<RideListModel>(RideSelected);
        }

        public async Task LoadAsync(Guid id)
        {
            User = await _userFacade.GetAsync((Guid)id) ?? UserDetailModel.Empty;
            
            var allRides = await _rideFacade.GetRidesAsync(id);
            DriverRides.Clear();
            DriverRides.AddRange(allRides);
            OnPropertyChanged(nameof(DriverRides));
        }

        private void RideSelected(RideListModel? rideListModel)
        {
            if (rideListModel is not null)
            {
                _mediator.Send(new SelectedMessage<RideWrapper> { Id = rideListModel.Id });
            }
        }
    }
}
