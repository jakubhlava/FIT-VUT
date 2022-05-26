using RideSharing.App.ViewModels.Interfaces;
using System.Windows.Controls;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for RideFinderView.xaml
    /// </summary>
    public partial class RideFinderView : UserControl
    {
        public RideFinderView(IRideFinderViewModel rideFinderViewModel)
        {
            InitializeComponent();
            DataContext = rideFinderViewModel;
        }
    }
}
