using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data.OleDb;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using RideSharing.App.Commands;
using RideSharing.App.Extensions;
using RideSharing.App.Factories;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Common;
using RideSharing.App.ViewModels.Enums;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using MessageBox = Xceed.Wpf.Toolkit.MessageBox;
using System;
using System.Threading.Tasks;


namespace RideSharing.App.ViewModels
{
    public class RideDetailViewModel : ViewModelBase, IRideDetailViewModel
    {
        private readonly RideFacade _rideFacade;
        private readonly UserFacade _userFacade;
        private readonly IMediator _mediator;

        public String CmdButtonText { get; set; } = "Přidat se k jízdě";
        private RideDetailModes _mode { get; set; } = RideDetailModes.NotParticipating;
        public RideDetailModel Ride { get; set; }
        public UserDetailModel User { get; set; }

        public ICommand RideActionCommand { get; }
        public ICommand RideDeleteCommand { get; }
        public ICommand KickPassengerCommand { get; }

        public RideDetailViewModel(RideFacade rideFacade, UserFacade userFacade, IMediator mediator)
        {
            _rideFacade = rideFacade;
            _userFacade = userFacade;
            _mediator = mediator;

            RideActionCommand = new AsyncRelayCommand(RideAction, CanMakeAction);
            RideDeleteCommand = new AsyncRelayCommand(RideDelete, IsOwner);
            KickPassengerCommand = new AsyncRelayCommand<Guid>(KickPassenger, IsOwner);
        }

        public async Task LoadAsync(Guid? rideId, Guid userId)
        {
            if (rideId == null)
            {
                Ride = RideDetailModel.Empty;
            }
            else
            {
                Ride = await _rideFacade.GetFullDetail(rideId.Value) ?? RideDetailModel.Empty;
            }

            User = await _userFacade.GetUserWithCarsAndRidesAsync(userId) ?? UserDetailModel.Empty;
            var passengerIds = (from passenger in Ride.Passengers select passenger.Id).ToList();
            Ride.Passengers.Clear();
            foreach (var id in passengerIds)
            {
                UserDetailModel u = await _userFacade.GetUserWithRidesAsync(id) ?? UserDetailModel.Empty;
                if (u != UserDetailModel.Empty)
                {
                    Ride.Passengers.Add(u);
                }
            }
            UpdateMode();
            OnPropertyChanged(nameof(Ride));
            OnPropertyChanged(nameof(User));
            OnPropertyChanged(nameof(_mode));
        }

        private void UpdateMode()
        {
            if (Ride.Car.CarOwnerId == User.Id)
            {
                _mode = RideDetailModes.Owner;
                CmdButtonText = "Upravit jízdu";
            }
            else if (IsUserPassengerOfRide(User, Ride))
            {
                _mode = RideDetailModes.Passenger;
                CmdButtonText = "Opustit jízdu";
            }
            else
            {
                _mode = RideDetailModes.NotParticipating;
                CmdButtonText = "Přidat se";
            }
            OnPropertyChanged(nameof(CmdButtonText));
        }

        private bool IsUserPassengerOfRide(UserDetailModel user, RideDetailModel ride)
        {
            return (from u in ride.Passengers
                where (u.Id == user.Id)
                select u).Any();
        }

        private async Task RideDelete()
        {
            if (_mode == RideDetailModes.Owner)
            {
                await _rideFacade.DeleteAsync(Ride);
                _mediator.Send(new RideAddedCloseMessage());
            }
        }

        private async Task KickPassenger(Guid userId)
        {
            var passengerIds = (from passenger in Ride.Passengers where passenger.Id != userId select passenger.Id).ToList();
            await _rideFacade.ForceRemoveRideUserRelations(Ride.Id);
            Ride.Passengers.Clear();
            foreach (Guid passengerId in passengerIds)
            {
                UserDetailModel u = await _userFacade.GetUserWithRidesAsync(passengerId) ?? UserDetailModel.Empty;
                if (u != UserDetailModel.Empty)
                {
                    Ride.Passengers.Add(u);
                }
            }
            await _rideFacade.SaveAsync(Ride);
            await LoadAsync(Ride.Id, User.Id);
        }

        private bool IsOwner() => _mode == RideDetailModes.Owner;
        private bool IsOwner(Guid id) => IsOwner();

        private bool CanMakeAction()
        {
            return (_mode == RideDetailModes.NotParticipating && Ride.FreeSpace > 0) ||
                   _mode != RideDetailModes.NotParticipating;
        }

        private async Task RideAction()
        {
            if (_mode == RideDetailModes.Owner)
            {
                _mediator.Send(new EditRideMessage(Ride.Id));
            } 
            else if (_mode == RideDetailModes.Passenger)
            {
                var passengerIds = (from passenger in Ride.Passengers where passenger.Id != User.Id select passenger.Id).ToList();
                await _rideFacade.ForceRemoveRideUserRelations(Ride.Id);
                Ride.Passengers.Clear();
                foreach (Guid passengerId in passengerIds)
                {
                    UserDetailModel u = await _userFacade.GetUserWithRidesAsync(passengerId) ?? UserDetailModel.Empty;
                    if (u != UserDetailModel.Empty)
                    {
                        Ride.Passengers.Add(u);
                    }
                }
                await _rideFacade.SaveAsync(Ride);
                await LoadAsync(Ride.Id, User.Id);
            }
            else if (_mode == RideDetailModes.NotParticipating)
            {
                if (!IntegrityChecks.CheckCollisionsBetweenUserAndRide(User, Ride))
                {
                    MessageBox.Show("Nelze se přidat do jízdy, která koliduje s jízdou, které se již učastníš.",
                        "Kolize jízd", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    var passengerIds = from passenger in Ride.Passengers select passenger.Id;
                    Ride.Passengers.Clear();
                    foreach (Guid passengerId in passengerIds)
                    {
                        UserDetailModel u = await _userFacade.GetUserWithRidesAsync(passengerId) ?? UserDetailModel.Empty;
                        if (u != UserDetailModel.Empty)
                        {
                            Ride.Passengers.Add(u);
                        }
                    }
                    Ride.Passengers.Add(User);
                    await _rideFacade.SaveAsync(Ride);
                    await LoadAsync(Ride.Id, User.Id);
                }
                
            }
            
        }


    }
}