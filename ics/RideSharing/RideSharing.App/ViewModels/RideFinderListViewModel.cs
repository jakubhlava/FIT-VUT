using RideSharing.App.Commands;
using RideSharing.App.Extensions;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.App.Wrappers;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;

namespace RideSharing.App.ViewModels
{
    public class RideFinderListViewModel : ViewModelBase, IRideFinderListViewModel
    {
        private readonly RideFacade _rideFacade;
        private readonly IMediator _mediator;

        public ObservableCollection<RideListModel> AvailableRides { get; set; } = new();

        public RideFinderListViewModel(RideFacade rideFacade, IMediator mediator)
        {
            _rideFacade = rideFacade;
            _mediator = mediator;

            _mediator.Register<ChangedRideParametersMessage>(RidesFoundHandler);
            RideSelectedCommand = new RelayCommand<RideListModel>(RideSelected);
        }

        private void RidesFoundHandler(ChangedRideParametersMessage message)
        {
            LoadAsync(message.from_, message.to_, message.date);
        }

        public ObservableCollection<RideListModel> Rides { get; set; } = new();

        public ICommand RideSelectedCommand { get; }

        private void RideSelected(RideListModel? rideListModel)
        {
            if (rideListModel is not null)
            {
                _mediator.Send(new SelectedMessage<RideWrapper> { Id = rideListModel.Id });
            }
        }

        public async Task LoadAsync(string from, string to, DateTime date)
        {
            var foundRides = await _rideFacade.GetFilteredAsync(from, to, date);
            AvailableRides.Clear();
            AvailableRides.AddRange(foundRides);
            OnPropertyChanged(nameof(AvailableRides));
        }
    }
}
