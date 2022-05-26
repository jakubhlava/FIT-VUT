using Microsoft.Toolkit.Mvvm.Input;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.App.Wrappers;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using System;
using System.Threading.Tasks;
using System.Windows.Input;


namespace RideSharing.App.ViewModels
{
    public class MyCarsEditViewModel : ViewModelBase, IMyCarsEditViewModel
    {

        private readonly IMediator _mediator;
        private readonly UserFacade _userFacade;
        private readonly CarFacade _carFacade;

        public MyCarsEditViewModel(
            CarFacade carFacade,
            UserFacade userFacade,
            IMediator mediator)
        {
            _userFacade = userFacade;
            _carFacade = carFacade;
            _mediator = mediator;

            SaveCommand = new AsyncRelayCommand(SaveAsync, CanSave);
            DeleteCarCommand = new AsyncRelayCommand(DeleteCarAsync);
        }

        public ICommand SaveCommand { get; }
        public ICommand DeleteCarCommand { get; }
        public UserDetailModel? UserDetail { get; private set; }
        public CarDetailModel? NewCar { get; private set; }


        public async Task LoadAsync(Guid UserId, Guid CarId)
        {


            if (CarId == Guid.Empty)
            {
                UserDetail = await _userFacade.GetUserForAddCarAsync(UserId) ?? UserDetailModel.Empty;
                NewCar = new CarDetailModel(String.Empty, String.Empty, String.Empty, DateTime.Today, 0, Guid.Empty, String.Empty, String.Empty, String.Empty);
                NewCar.CarOwner = UserDetail;
            }
            else
            {
                NewCar = await _carFacade.GetAsync(CarId) ?? CarDetailModel.Empty;
            }

        }
        public async Task DeleteCarAsync()
        {


            if (NewCar.Id == Guid.Empty)
            {
                _mediator.Send(new CarSavedCloseMessage());
            }
            else
            {
                await _carFacade.DeleteAsync(NewCar.Id);
                _mediator.Send(new CarSavedCloseMessage());
            }
        }

        public async Task SaveAsync()
        {


            if (NewCar == null)
            {
                throw new InvalidOperationException("Null model cannot be saved");
            }
            else
            {
                await _carFacade.SaveAsync(NewCar);
                _mediator.Send(new CarSavedCloseMessage());
            }
        }
        private bool CanSave() => true;

    }
}
