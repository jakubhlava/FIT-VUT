using RideSharing.App.ViewModels.Interfaces;
using System.Windows.Controls;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for RideCreateEditView.xaml
    /// </summary>
    public partial class RideCreateEditView : UserControl
    {
        public RideCreateEditView(IRideCreateEditViewModel rideCreateEditViewModel)
        {
            InitializeComponent();
            DataContext = rideCreateEditViewModel;
        }
    }
}
