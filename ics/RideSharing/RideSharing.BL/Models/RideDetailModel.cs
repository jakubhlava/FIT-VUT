using AutoMapper;
using RideSharing.DAL.Entities;
using System.ComponentModel.DataAnnotations;

namespace RideSharing.BL.Models
{
    public record RideDetailModel(
        string CityFrom,
        string CityTo,
        DateTime DepartureTime,
        DateTime ArrivalTime,
        Guid DriverId,
        string DriverName,
        string DriverSurname,
        string DriverNumber,
        Guid CarId,
        string CarBrand,
        string CarModel,
        string CarType,
        DateTime CarRegistrationDate,
        int CarSeatsCount
    ) : ModelBase
    {
        public string CityFrom { get; set; } = CityFrom;
        public string CityTo { get; set; } = CityTo;
        public DateTime DepartureTime { get; set; } = DepartureTime;
        public DateTime ArrivalTime { get; set; } = ArrivalTime;
        public string? Note { get; set; }
        public Guid DriverId { get; set; } = DriverId;
        public string DriverName { get; set; } = DriverName;
        public string DriverSurname { get; set; } = DriverSurname;
        public string DriverNumber { get; set; } = DriverNumber;
        public string? DriverUserImg { get; set; }
        public Guid CarId { get; set; } = CarId;
        public string CarBrand { get; set; } = CarBrand;
        public string CarModel { get; set; } = CarModel;
        public string CarType { get; set; } = CarType;
        public DateTime CarRegistrationDate { get; set; } = CarRegistrationDate;
        public int CarSeatsCount { get; set; } = CarSeatsCount;
        public string? CarImg { get; set; }
        public string? CarNote { get; set; }

        public int FreeSpace => CarSeatsCount - Passengers.Count - 1;

        public List<UserDetailModel> Passengers { get; init; } = new();
        private CarListModel _car;
        private UserDetailModel _driver;
        public CarListModel Car
        {
            get => _car;
            set
            {
                if (value != null)
                {
                    _car = value;
                    CarId = value.Id;
                    CarBrand = value.Brand;
                    CarRegistrationDate = value.RegistrationDate;
                    CarSeatsCount = value.SeatsCount;
                    CarImg = value.Img;
                    CarNote = value.Note;
                }
            }
        }

        public UserDetailModel Driver
        {
            get => _driver;
            set
            {

                if (value != null)
                {
                    _driver = value;
                    DriverId = value.Id;
                    DriverName = value.Name;
                    DriverSurname = value.Surname;
                    DriverNumber = value.Number;
                    DriverUserImg = value.UserImg;
                }

            }
        }

        public static RideDetailModel Empty = new(String.Empty, String.Empty, DateTime.MinValue, DateTime.MinValue, Guid.Empty,
            String.Empty, String.Empty, String.Empty, Guid.Empty, String.Empty, String.Empty, String.Empty,
            DateTime.MinValue, -1);

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<RideEntity, RideDetailModel>()
                    .ForMember(model => model.FreeSpace, expression => expression.Ignore())
                    .ReverseMap()
                    .ForMember(ent => ent.Driver, expression => expression.Ignore())
                    .ForMember(ent => ent.Car, expression => expression.Ignore());
            }
        }
    };
}

