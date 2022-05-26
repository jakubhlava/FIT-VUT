using AutoMapper;
using RideSharing.DAL.Entities;

namespace RideSharing.BL.Models
{
    public record RideListModel(
        string CityFrom,
        string CityTo,
        DateTime DepartureTime,
        DateTime ArrivalTime,
        Guid DriverId,
        string DriverName,
        Guid CarId,
        int CarSeatsCount
    ) : ModelBase
    {
        public string CityFrom { get; set; } = CityFrom;
        public string CityTo { get; set; } = CityTo;
        public DateTime DepartureTime { get; set; } = DepartureTime;
        public DateTime ArrivalTime { get; set; } = ArrivalTime;
        public Guid DriverId { get; set; } = DriverId;
        public string DriverName { get; set; } = DriverName;
        public string? DriverUserImg { get; set; }
        public Guid CarId { get; set; } = CarId;
        public int CarSeatsCount { get; set; } = CarSeatsCount;
        public string? CarImg { get; set; }

        public List<UserListModel> Passengers { get; init; } = new();

        public int FreeSpace => CarSeatsCount - Passengers.Count - 1;

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<RideEntity, RideListModel>()
                    .ForMember(model => model.Passengers, opt => opt.MapFrom(entity => entity.Passengers))
                    .ForMember(model => model.DriverName, opt => opt.MapFrom(entity => entity.Car.CarOwner.Name))
                    .ForMember(model => model.DriverUserImg, opt => opt.MapFrom(entity => entity.Car.CarOwner.UserImg))
                    .ReverseMap();
            }
        }
    }
}

