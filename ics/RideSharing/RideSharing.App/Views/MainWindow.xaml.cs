using RideSharing.App.Messages;
using RideSharing.App.Services;
using RideSharing.App.ViewModels;
using System;
using System.Windows;

namespace RideSharing.App.Views
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public readonly IMediator _mediator;
        public Guid? _userId;

        public MainWindow(MainViewModel mainViewModel, IMediator mediator)
        {
            InitializeComponent();
            DataContext = mainViewModel;
            _mediator = mediator;
            this.Content = new LoginPage();
            mediator.Register<SwapToFindRideViewMessage>(ChangeToFindRideView);
            mediator.Register<LogoutMessage>(LogoutUser);
        }

        private void ChangeToFindRideView(SwapToFindRideViewMessage message)
        {
            this.Content = new MainPageView(message.mpvm);
        }

        private void LogoutUser(LogoutMessage message)
        {
            this.Content = new LoginPage();
        }
    }
}
