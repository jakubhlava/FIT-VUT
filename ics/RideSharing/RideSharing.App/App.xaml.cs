using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;
using RideSharing.App.Extensions;
using RideSharing.App.Services;
using RideSharing.App.Services.MessageDialog;
using RideSharing.App.Settings;
using RideSharing.App.ViewModels;
using RideSharing.App.ViewModels.Interfaces;
using RideSharing.App.Views;
using RideSharing.BL;
using RideSharing.DAL;
using RideSharing.DAL.Factories;
using System;
using System.Globalization;
using System.Threading;
using System.Windows;

namespace RideSharing.App
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private readonly IHost _host;


        public App()
        {
            Thread.CurrentThread.CurrentCulture = new CultureInfo("cs");
            Thread.CurrentThread.CurrentUICulture = new CultureInfo("cs");

            _host = Host.CreateDefaultBuilder()
                .ConfigureAppConfiguration(ConfigureAppConfiguration)
                .ConfigureServices((context, services) => { ConfigureServices(context.Configuration, services); })
                .Build();
        }

        private static void ConfigureAppConfiguration(HostBuilderContext context, IConfigurationBuilder builder)
        {
            builder.AddJsonFile(@"AppSettings.json", false, false);
        }

        private static void ConfigureServices(IConfiguration configuration,
            IServiceCollection services)
        {
            services.AddBLServices();

            services.Configure<DALSettings>(configuration.GetSection("RideSharing:DAL"));

            services.AddSingleton<IDbContextFactory<RideSharingDbContext>>(provider =>
            {
                var dalSettings = provider.GetRequiredService<IOptions<DALSettings>>().Value;
                return new SqlServerDbContextFactory(dalSettings.ConnectionString!,
                    dalSettings.SkipMigrationAndSeedDemoData);
            });

            services.AddSingleton<MainWindow>();

            services.AddSingleton<IMessageDialogService, MessageDialogService>();
            services.AddSingleton<IMediator, Mediator>();

            services.AddSingleton<MainViewModel>();
            services.AddSingleton<IUserListViewModel, UserListViewModel>();
            services.AddSingleton<IRideFinderListViewModel, RideFinderListViewModel>();
            services.AddFactory<IMainPageViewModel, MainPageViewModel>();
            services.AddFactory<IUserEditViewModel, UserEditViewModel>();
            services.AddFactory<IRideFinderViewModel, RideFinderViewModel>();
            services.AddFactory<IMySharedRidesViewModel, MySharedRidesViewModel>();
            services.AddFactory<IDriverRidesViewModel, DriverRidesViewModel>();
            services.AddFactory<IRideDetailViewModel, RideDetailViewModel>();
            services.AddFactory<IMyCarsViewModel, MyCarsViewModel>();
            services.AddFactory<IMyCarsEditViewModel, MyCarsEditViewModel>();
            services.AddFactory<IRideCreateEditViewModel, RideCreateEditViewModel>();

        }

        protected override async void OnStartup(StartupEventArgs e)
        {
            await _host.StartAsync();

            var dbContextFactory = _host.Services.GetRequiredService<IDbContextFactory<RideSharingDbContext>>();

            var dalSettings = _host.Services.GetRequiredService<IOptions<DALSettings>>().Value;

            await using (var dbx = await dbContextFactory.CreateDbContextAsync())
            {
                if (dalSettings.SkipMigrationAndSeedDemoData)
                {
                    await dbx.Database.EnsureDeletedAsync();
                    await dbx.Database.EnsureCreatedAsync();
                }
                else
                {
                    try
                    {
                        await dbx.Database.MigrateAsync();
                    }
                    catch
                    {
                    }
                }
            }

            var mainWindow = _host.Services.GetRequiredService<MainWindow>();
            mainWindow.Show();

            base.OnStartup(e);
        }

        protected override async void OnExit(ExitEventArgs e)
        {
            using (_host)
            {
                await _host.StopAsync(TimeSpan.FromSeconds(5));
            }

            base.OnExit(e);
        }
    }
}