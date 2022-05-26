using AutoMapper;
using RideSharing.DAL.Entities;
using System.ComponentModel.DataAnnotations;

namespace RideSharing.BL.Models
{
    public record UserDetailModel(
        string Name,
        string Surname,
        string Number
    ) : ModelBase
    {
        public string Name { get; set; } = Name;
        public string Surname { get; set; } = Surname;
        public string Number { get; set; } = Number;
        public string? UserImg { get; set; }

        public List<CarListModel> OwnedCars { get; init; } = new();
        public List<RideListModel> DriverRides { get; init; } = new();
        public List<RideListModel> PassengerRides { get; init; } = new();

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<UserEntity, UserDetailModel>()
                    .ReverseMap()
                    .ForMember(entity => entity.PassengerRides, expression => expression.Ignore())
                    .ForMember(entity => entity.DriverRides, expression => expression.Ignore())
                    .ForMember(entity => entity.OwnedCars, expression => expression.Ignore());
            }
        }

        public static UserDetailModel Empty => new(string.Empty, string.Empty, string.Empty);
    }
}

