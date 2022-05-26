using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using System;


namespace RideSharing.App.ViewModels
{
    public class RideFinderViewModel : ViewModelBase, IRideFinderViewModel
    {
        private string _from = String.Empty;
        private string _to = String.Empty;
        private DateTime _time = DateTime.Today;
        private readonly IMediator _mediator;

        public IRideFinderListViewModel RideFinderListViewModel { get; }

        public RideFinderViewModel(IRideFinderListViewModel rideFinderListViewModel, IMediator mediator)
        {
            _mediator = mediator;
            RideFinderListViewModel = rideFinderListViewModel;
            RideFinderListViewModel.LoadAsync(String.Empty, String.Empty, DateTime.Today);
        }


        public string From
        {
            get { return _from; }
            set
            {
                _from = value;
                Search();
            }
        }

        public string To
        {
            get { return _to; }
            set
            {
                _to = value;
                Search();
            }
        }

        public DateTime Time
        {
            get { return _time; }
            set
            {
                _time = value;
                Search();
            }
        }

        public void Search()
        {
            _mediator.Send(new ChangedRideParametersMessage(_from, _to, _time));
            OnPropertyChanged(nameof(RideFinderListViewModel));
        }
    }
}
