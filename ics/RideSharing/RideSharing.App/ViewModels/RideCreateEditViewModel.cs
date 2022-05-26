using Microsoft.Toolkit.Mvvm.Input;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using System;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using RideSharing.App.ViewModels.Common;
using MessageBox = Xceed.Wpf.Toolkit.MessageBox;

namespace RideSharing.App.ViewModels;

public class RideCreateEditViewModel : ViewModelBase, IRideCreateEditViewModel
{
    private readonly IMediator _mediator;
    private readonly UserFacade _userFacade;
    private readonly RideFacade _rideFacade;
    private bool editMode = false;
    private CarListModel _selectedCar;

    public RideCreateEditViewModel(UserFacade userFacade,
        RideFacade rideFacade,
        IMediator mediator)
    {
        _userFacade = userFacade;
        _rideFacade = rideFacade;
        _mediator = mediator;

        SaveRideCommand = new AsyncRelayCommand(SaveAsync);
    }

    public UserDetailModel? User { get; private set; }
    public RideDetailModel? Ride { get; private set; }

    public CarListModel? SelectedCar
    {
        get
        {
            return _selectedCar;
        }
        set
        {
            _selectedCar = value;
            if (Ride != null)
            {
                Ride.Car = _selectedCar;
                OnPropertyChanged(nameof(Ride));
            }

        }
    }

    public ICommand SaveRideCommand { get; set; }

    public async Task LoadAsync(Guid userId, Guid rideId)
    {
        User = await _userFacade.GetUserWithCarsAndRidesAsync(userId) ?? UserDetailModel.Empty;
        if (rideId == Guid.Empty)
        {
            Ride = new RideDetailModel(String.Empty, String.Empty, DateTime.Today, DateTime.Today, Guid.Empty, String.Empty,
            String.Empty, String.Empty, Guid.Empty, String.Empty, String.Empty, String.Empty, DateTime.MinValue, -1);
            Ride.Driver = User;
        }
        else
        {
            Ride = await _rideFacade.GetFullDetail(rideId) ?? RideDetailModel.Empty;
            SelectedCar = Ride.Car;
            editMode = true;
        }
        
    }

    private bool CheckForCollisions()
    {
        if (editMode)
        {
            foreach (var passenger in Ride.Passengers)
            {
                if (!IntegrityChecks.CheckCollisionsBetweenUserAndRide(passenger, Ride)) return false;
            }
        }
        return IntegrityChecks.CheckCollisionsBetweenUserAndRide(User, Ride);
    }

    


    public async Task SaveAsync()
    {
        if (Ride == null)
        {
            throw new InvalidOperationException("Null model cannot be saved");
        }

        if (!CheckForCollisions())
        {
            if (editMode)
            {
                MessageBox.Show("Nelze editovat jízdu, protože nové nastavení koliduje s jinou jízdou některého s účastníků.",
                    "Kolize jízd", MessageBoxButton.OK, MessageBoxImage.Error);

            }
            else
            {
                MessageBox.Show("Nelze vytvořit jízdu, která koliduje s jinou jízdou registrovanou pro uživatele.",
                    "Kolize jízd", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        } 
        else if (Ride.ArrivalTime <= Ride.DepartureTime)
        {
            MessageBox.Show("Nelze cestovat časem ani nadsvětelnou rychlostí, omlouváme se. (Čas odjezdu musí předcházet času příjezdu).",
                "Nelze", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        else
        {
            var passengerIds = (from passenger in Ride.Passengers select passenger.Id).ToList();
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
            Ride = await _rideFacade.SaveAsync(Ride);
            _mediator.Send(new RideAddedCloseMessage());
        }
    }
}