using RideSharing.App.ViewModels.Interfaces;
using System.Windows.Controls;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for UserEditView.xaml
    /// </summary>
    public partial class UserEditView : UserControl
    {
        public UserEditView(IUserEditViewModel userEditViewModel)
        {
            InitializeComponent();
            DataContext = userEditViewModel;
        }
    }
}
