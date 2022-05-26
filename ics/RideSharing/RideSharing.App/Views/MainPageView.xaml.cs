using RideSharing.App.ViewModels.Interfaces;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for MainPageView.xaml
    /// </summary>
    public partial class MainPageView
    {
        public MainPageView(IMainPageViewModel mainPageViewModel)
        {
            InitializeComponent();
            DataContext = mainPageViewModel;
        }
    }
}
