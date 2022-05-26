using RideSharing.App.Factories;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.App.Wrappers;
using System;
using System.Threading.Tasks;

namespace RideSharing.App.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        private readonly IFactory<IMainPageViewModel> _mainPageViewModelFactory;
        private readonly IMediator _mediator;

        public MainViewModel(IMediator mediator,
            IUserListViewModel userListViewModel,
            IFactory<IMainPageViewModel> mainPageViewModelFactory)
        {
            UserListViewModel = userListViewModel;
            _mainPageViewModelFactory = mainPageViewModelFactory;
            _mediator = mediator;
            _mediator.Register<SelectedMessage<UserWrapper>>(OnUserSelected);
            _mediator.Register<LogoutMessage>(LogoutUser);

        }

        public IUserListViewModel UserListViewModel { get; }
        public IMainPageViewModel? SelectedMainPageViewModel { get; set; }

        private void OnUserSelected(SelectedMessage<UserWrapper> message)
        {
            SelectUser(message.Id);
        }

        private async Task SelectUser(Guid? id)
        {
            if (id == null)
            {
                SelectedMainPageViewModel = null;
            }
            else
            {
                if (SelectedMainPageViewModel == null)
                {
                    SelectedMainPageViewModel = _mainPageViewModelFactory.Create();
                    await SelectedMainPageViewModel.LoadAsync(id.Value);
                }

                _mediator.Send(new SwapToFindRideViewMessage(SelectedMainPageViewModel));
            }
        }

        private void LogoutUser(LogoutMessage message)
        {
            SelectedMainPageViewModel = null;
        }
    }
}
