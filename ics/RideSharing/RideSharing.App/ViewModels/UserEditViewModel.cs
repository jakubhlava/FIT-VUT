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
    public class UserEditViewModel : ViewModelBase, IUserEditViewModel
    {
        private readonly IMediator _mediator;
        private readonly UserFacade _userFacade;

        public UserEditViewModel(
            UserFacade userFacade,
            IMediator mediator)
        {
            _userFacade = userFacade;
            _mediator = mediator;

            SaveCommand = new AsyncRelayCommand(SaveAsync);
            DeleteCommand = new AsyncRelayCommand(DeleteAsync);
        }

        public UserWrapper? User { get; private set; }
        public ICommand SaveCommand { get; }
        public ICommand DeleteCommand { get; }


        public async Task LoadAsync(Guid id)
        {
            if (id == Guid.Empty)
            {
                User = new UserWrapper(new UserDetailModel(String.Empty, String.Empty, String.Empty));
            }
            else
            {
                User = await _userFacade.GetAsync(id) ?? UserDetailModel.Empty;
            }

        }
        public async Task SaveAsync()
        {
            if (User == null)
            {
                throw new InvalidOperationException("Null model cannot be saved");
            }

            User = await _userFacade.SaveAsync(User.BaseModel);
            _mediator.Send(new UserSavedCloseMessage());
        }

        public async Task DeleteAsync()
        {
            await _userFacade.DeleteAsync(User.BaseModel);
            _mediator.Send(new LogoutMessage());
        }


        public override void LoadInDesignMode()
        {
            base.LoadInDesignMode();
            User = new UserWrapper(new UserDetailModel(
                Name: "Design",
                Surname: "Nadesignovaný",
                Number: "+420 696 666 420")
            {
                UserImg = "https://miro.medium.com/max/1400/1*1yjbmb_mTngFxZ5qA3RLOw.jpeg"
            });
        }

    }
}
