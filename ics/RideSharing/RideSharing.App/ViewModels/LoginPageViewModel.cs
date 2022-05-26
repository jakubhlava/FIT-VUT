using RideSharing.App.Services;

namespace RideSharing.App.ViewModels
{
    public class LoginPageViewModel : ViewModelBase
    {
        public LoginPageViewModel(IUserListViewModel userListViewModel, IMediator mediator)
        {
            UserListViewModel = userListViewModel;
        }
        public IUserListViewModel UserListViewModel { get; }


    }
}
