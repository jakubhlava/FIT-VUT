using RideSharing.App.ViewModels.Interfaces;
using System.Windows.Controls;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for MyCars.xaml
    /// </summary>
    public partial class MyCars : UserControl
    {
        public MyCars(IMyCarsViewModel myCarsViewModel)
        {
            InitializeComponent();
            DataContext = myCarsViewModel;
        }
    }
}
