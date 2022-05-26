using AutoMapper;
using RideSharing.DAL.Entities;
using System.ComponentModel.DataAnnotations;

namespace RideSharing.BL.Models
{
    public record CarDetailModel(
        string Brand,
        string Model,
        string Type,
        DateTime RegistrationDate,
        int SeatsCount,
        Guid CarOwnerId,
        string CarOwnerName,
        string CarOwnerSurname,
        string CarOwnerNumber
    ) : ModelBase
    {
        public string Brand { get; set; } = Brand;
        public string Model { get; set; } = Model;
        public string Type { get; set; } = Type;
        public DateTime RegistrationDate { get; set; } = RegistrationDate;
        public int SeatsCount { get; set; } = SeatsCount;
        public Guid CarOwnerId { get; set; } = CarOwnerId;
        public string CarOwnerName { get; set; } = CarOwnerName;
        public string CarOwnerSurname { get; set; } = CarOwnerSurname;
        public string CarOwnerNumber { get; set; } = CarOwnerNumber;
        public string? Img { get; set; }
        public string? Note { get; set; }
        public string? CarOwnerUserImg { get; set; }
        public List<RideListModel> Rides { get; init; } = new();
        private UserDetailModel _carOwner;

        public UserDetailModel CarOwner
        {
            private get => _carOwner;
            set
            {

                if (value != null)
                {
                    _carOwner = value;
                    CarOwnerId = value.Id;
                    CarOwnerName = value.Name;
                    CarOwnerSurname = value.Surname;
                    CarOwnerNumber = value.Number;
                }

            }
        }
        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<CarEntity, CarDetailModel>()
                    .ReverseMap()
                    .ForMember(ent => ent.CarOwner, expression => expression.Ignore())
                    .ForMember(ent => ent.Rides, expression => expression.Ignore());
            }
        }

        public static CarDetailModel Empty = new(String.Empty, String.Empty, String.Empty, DateTime.MinValue, -1, Guid.Empty,
            String.Empty, String.Empty, String.Empty);
    };
}

