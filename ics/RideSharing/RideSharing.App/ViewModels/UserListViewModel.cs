using RideSharing.App.Commands;
using RideSharing.App.Extensions;
using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.Wrappers;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;


namespace RideSharing.App.ViewModels
{
    public class UserListViewModel : ViewModelBase, IUserListViewModel
    {
        private readonly UserFacade _userFacade;
        private readonly IMediator _mediator;

        public UserListViewModel(UserFacade userFacade, IMediator mediator)
        {
            _userFacade = userFacade;
            _mediator = mediator;

            UserSelectedCommand = new RelayCommand<UserListModel>(UserSelected);
            UserNewCommand = new RelayCommand(UserNew);

            mediator.Register<UpdateMessage<UserWrapper>>(UserUpdated);
        }

        public ObservableCollection<UserListModel> Users { get; set; } = new();

        public ICommand UserSelectedCommand { get; }
        public ICommand UserNewCommand { get; }

        private void UserNew() => _mediator.Send(new NewMessage<UserWrapper>());

        private async void UserUpdated(UpdateMessage<UserWrapper> _) => await LoadAsync();

        private void UserSelected(UserListModel? userListModel)
        {
            if (userListModel is not null)
            {
                _mediator.Send(new SelectedMessage<UserWrapper> { Id = userListModel.Id });
            }
        }

        public async Task LoadAsync()
        {
            Users.Clear();
            var allUsers = await _userFacade.GetAsync();
            Users.AddRange(allUsers);
        }
    }
}
