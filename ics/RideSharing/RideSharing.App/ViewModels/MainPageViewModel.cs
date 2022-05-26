using RideSharing.App.Factories;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.App.Views;
using RideSharing.App.Wrappers;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using System;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Input;
using RelayCommand = RideSharing.App.Commands.RelayCommand;

namespace RideSharing.App.ViewModels
{
    public class MainPageViewModel : ViewModelBase, IMainPageViewModel
    {
        private readonly IMediator _mediator;
        private readonly IFactory<IUserEditViewModel> _userEditViewModelFactory;
        private readonly IFactory<IRideFinderViewModel> _rideFinderViewModelFactory;
        private readonly IFactory<IMySharedRidesViewModel> _rideMySharedRidesViewModelFactory;
        private readonly IFactory<IDriverRidesViewModel> _driverRidesViewModelFactory;

        private readonly IFactory<IMyCarsViewModel> _myCarsViewModelFactory;
        private readonly IFactory<IMyCarsEditViewModel> _myCarsEditViewModelFactory;
        private readonly IFactory<IRideCreateEditViewModel> _rideCreateEditViewModelFactory;
        private readonly IFactory<IRideDetailViewModel> _rideDetailViewModelFactory;
        private readonly UserFacade _userFacade;
        private readonly CarFacade _carFacade;
        private UserControl _currentView;

        public MainPageViewModel(UserFacade userFacade,
            IFactory<IUserEditViewModel> userEditViewModelFactory,
            IFactory<IRideFinderViewModel> rideFinderViewModel,
            IFactory<IMySharedRidesViewModel> rideMySharedRidesViewModel,
            IFactory<IRideDetailViewModel> rideDetailViewModelFactory,
            IFactory<IMyCarsViewModel> myCarsViewModel,
            IFactory<IMyCarsEditViewModel> myCarsEditViewModel,
            IFactory<IRideCreateEditViewModel> rideCreateEditViewModelFactory,
            IFactory<IDriverRidesViewModel> driverRidesViewModel,
            IMediator mediator)
        {
            FindRideCommand = new RelayCommand(SwapToFindRideView);
            MyCarsCommand = new RelayCommand(SwapToMyCarsView);
            UserEditCommand = new RelayCommand(SwapToEditUser);
            CreateUserCommand = new RelayCommand(SwapToCreateUser);
            CreateCarCommand = new RelayCommand(SwapToCreateCar);
            CreateRideCommand = new RelayCommand(SwapToCreateRide);
            LogoutCommand = new RelayCommand(Logout);
            SharedRidesCommand = new RelayCommand(SwapToMySharedRidesView);
            DriverRidesCommand = new RelayCommand(SwapToDriverRidesView);


            _userEditViewModelFactory = userEditViewModelFactory;
            _rideFinderViewModelFactory = rideFinderViewModel;
            _myCarsViewModelFactory = myCarsViewModel;
            _myCarsEditViewModelFactory = myCarsEditViewModel;
            _rideCreateEditViewModelFactory = rideCreateEditViewModelFactory;
            _rideDetailViewModelFactory = rideDetailViewModelFactory;
            _rideMySharedRidesViewModelFactory = rideMySharedRidesViewModel;
            _driverRidesViewModelFactory = driverRidesViewModel;


            RideFinderViewModel = _rideFinderViewModelFactory.Create();
            CurrentView = new RideFinderView(RideFinderViewModel);

            _mediator = mediator;
            _mediator.Register<UserSavedCloseMessage>(UserSaved);
            _mediator.Register<CarSavedCloseMessage>(CarSaved);
            _mediator.Register<SelectedMessage<RideWrapper>>(ShowRideDetail);
            _mediator.Register<RideAddedCloseMessage>(RideAdded);
            _mediator.Register<SelectedMessage<CarWrapper>>(OnCarSelected);
            _mediator.Register<EditRideMessage>(SwapToEditRide); 
            _userFacade = userFacade;
        }

        public ICommand FindRideCommand { get; }
        public ICommand MyCarsCommand { get; }
        public ICommand CreateRideCommand { get; }
        public ICommand SharedRidesCommand { get; }

        public ICommand UserEditCommand { get; }
        public ICommand CreateUserCommand { get; }
        public ICommand CreateCarCommand { get; }
        public ICommand DriverRidesCommand { get; }

        public ICommand LogoutCommand { get;  }

        public IMyCarsEditViewModel? CarEditViewModel { get; set; }
        public IMyCarsViewModel? MyCarsViewModel { get; set; }
        public IUserEditViewModel? UserEditViewModel { get; set; }
        public IRideCreateEditViewModel RideCreateEditViewModel { get; set; }
        public IRideFinderViewModel RideFinderViewModel { get; set; }
        public IMySharedRidesViewModel MySharedRidesViewModel { get; set; }
        public IDriverRidesViewModel DriverRidesViewModel { get; set; }
        public IRideDetailViewModel RideDetailViewModel { get; set; }


        public UserDetailModel? User { get; set; }

        public UserControl CurrentView
        {
            get { return _currentView; }
            set
            {
                _currentView = value;
                ViewChanged();
            }
        }

        private void OnCarSelected(SelectedMessage<CarWrapper> message)
        {
            SelectCar(message.Id);
        }

        private async Task SelectCar(Guid? id)
        {
            CarEditViewModel = _myCarsEditViewModelFactory.Create();
            await CarEditViewModel.LoadAsync(id.Value, (Guid)id);

            CurrentView = new CarEditView(CarEditViewModel);
        }

        private void ViewChanged()
        {
            OnPropertyChanged(nameof(CurrentView));
        }
        private void SwapToFindRideView()
        {
            RideFinderViewModel = _rideFinderViewModelFactory.Create();
            CurrentView = new RideFinderView(RideFinderViewModel);
        }

        private void SwapToMyCarsView()
        {
            MyCarUser();
        }

        private async Task MyCarUser()
        {
            MyCarsViewModel = _myCarsViewModelFactory.Create();
            await MyCarsViewModel.LoadAsync(User.Id);
            CurrentView = new MyCars(MyCarsViewModel);
        }

        private async Task showRideDetailTask(SelectedMessage<RideWrapper> message)
        {
            RideDetailViewModel = _rideDetailViewModelFactory.Create();
            await RideDetailViewModel.LoadAsync(message.Id, User.Id); 
            CurrentView = new RideDetailView(RideDetailViewModel);
        }

        private void ShowRideDetail(SelectedMessage<RideWrapper> message)
        {
            showRideDetailTask(message);
        }

        private void Logout()
        {
            _mediator.Send(new LogoutMessage());
        }

        private void UserSaved(UserSavedCloseMessage message)
        {
            SwapToFindRideView();
            UserEditViewModel = null;
            LoadAsync(User.Id);
        }

        private void RideAdded(RideAddedCloseMessage message)
        {
            SwapToFindRideView();
            RideCreateEditViewModel = null;
            LoadAsync(User.Id);
        }

        private async Task EditRide(EditRideMessage message)
        {
            RideCreateEditViewModel = _rideCreateEditViewModelFactory.Create();
            await RideCreateEditViewModel.LoadAsync(User.Id, message.RideId);
            CurrentView = new RideCreateEditView(RideCreateEditViewModel);
        }
        private void SwapToEditRide(EditRideMessage message)
        {
            EditRide(message);
        }

        private async Task EditUser()
        {
            UserEditViewModel = _userEditViewModelFactory.Create();
            await UserEditViewModel.LoadAsync(User.Id);

            CurrentView = new UserEditView(UserEditViewModel);
            OnPropertyChanged(nameof(User));
        }

        private void SwapToEditUser()
        {
            EditUser();
        }

        private async Task CreateUser()
        {
            UserEditViewModel = _userEditViewModelFactory.Create();
            await UserEditViewModel.LoadAsync(Guid.Empty); // editing empty user creates a new one
            CurrentView = new UserEditView(UserEditViewModel);
        }

        private void CarSaved(CarSavedCloseMessage message)
        {
            SwapToMyCarsView();
            CarEditViewModel = null;
            LoadAsync(User.Id);
        }

        private void SwapToCreateUser()
        {
            CreateUser();
        }

        private async Task CreateRide()
        {
            RideCreateEditViewModel = _rideCreateEditViewModelFactory.Create();
            await RideCreateEditViewModel.LoadAsync(User.Id, Guid.Empty);
            CurrentView = new RideCreateEditView(RideCreateEditViewModel);
        }

        private void SwapToCreateRide()
        {
            CreateRide();
        }

        private async Task MyRides()
        {
            MySharedRidesViewModel = _rideMySharedRidesViewModelFactory.Create();
            await MySharedRidesViewModel.LoadAsync(User.Id);
            CurrentView = new MySharedRidesView(MySharedRidesViewModel);
        }

        private void SwapToMySharedRidesView()
        {
            MyRides();
        }

        private async Task DriverRides()
        {
            DriverRidesViewModel = _driverRidesViewModelFactory.Create();
            await DriverRidesViewModel.LoadAsync(User.Id);
            CurrentView = new DriverRidesView(DriverRidesViewModel);
        }

        private void SwapToDriverRidesView()
        {
            DriverRides();
        }

        private async Task CreateCar()
        {
            CarEditViewModel = _myCarsEditViewModelFactory.Create();
            await CarEditViewModel.LoadAsync(User.Id, Guid.Empty);
            CurrentView = new CarEditView(CarEditViewModel);
        }


        private void SwapToCreateCar()
        {
            CreateCar();
        }


        public async Task LoadAsync(Guid userId)
        {
            User = await _userFacade.GetAsync(userId) ?? UserDetailModel.Empty;
        }
    }
}
