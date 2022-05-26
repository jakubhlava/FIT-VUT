using RideSharing.App.ViewModels.Interfaces;
using System.Windows.Controls;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for CarEditView.xaml
    /// </summary>
    public partial class CarEditView : UserControl
    {
        public CarEditView(IMyCarsEditViewModel carEditViewModel)
        {
            InitializeComponent();
            DataContext = carEditViewModel;
        }
    }
}
