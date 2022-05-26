using RideSharing.App.Commands;
using RideSharing.App.Extensions;
using RideSharing.App.Factories;
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
    public class MyCarsViewModel : ViewModelBase, IMyCarsViewModel
    {
        private readonly IMediator _mediator;
        private readonly IFactory<IUserEditViewModel> _userEditViewModelFactory;
        public Guid? UserId;

        private readonly CarFacade _carFacade;
        private readonly UserFacade _userFacade;
        public UserWrapper? User { get; private set; }

        public CarWrapper? NewCar { get; private set; }

        public MyCarsViewModel(CarFacade carFacade,
            IFactory<IUserEditViewModel> userEditViewModelFactory,
            UserFacade userFacade,
            IMediator mediator)
        {
            _carFacade = carFacade;
            _mediator = mediator;
            _userFacade = userFacade;

            CarSelectedCommand = new RelayCommand<CarListModel>(CarSelected);
        }


        public ObservableCollection<CarListModel> Cars { get; set; } = new();
        public ICommand CarSelectedCommand { get; }
        
        private void CarSelected(CarListModel? carListModel)
        {
            if (carListModel is not null)
            {
                _mediator.Send(new SelectedMessage<CarWrapper> { Id = carListModel.Id });
            }
        }

        public async Task LoadAsync(Guid id)
        {
            User = await _userFacade.GetAsync(id) ?? UserDetailModel.Empty;
            Cars.Clear();
            var allCars = await _carFacade.GetFilteredAsync(id);
            Cars.AddRange(allCars);
        }


    }
}
