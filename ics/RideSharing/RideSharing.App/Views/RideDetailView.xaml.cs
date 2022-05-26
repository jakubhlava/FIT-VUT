using RideSharing.App.ViewModels.Interfaces;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for RideDetailView.xaml
    /// </summary>
    public partial class RideDetailView
    {
        public RideDetailView(IRideDetailViewModel rideDetailViewModel)
        {
            InitializeComponent();
            DataContext = rideDetailViewModel;
        }
    }
}
