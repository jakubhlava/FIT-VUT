using AutoMapper;
using RideSharing.DAL.Entities;

namespace RideSharing.BL.Models
{
    public record CarListModel(
        string Brand,
        string Model,
        string Type,
        DateTime RegistrationDate,
        int SeatsCount,
        string Note,
        string CarOwnerName,
        string CarOwnerSurname
    ) : ModelBase
    {
        public string Brand { get; set; } = Brand;
        public string Model { get; set; } = Model;
        public string Type { get; set; } = Type;
        public DateTime RegistrationDate { get; set; } = RegistrationDate;
        public int SeatsCount { get; set; } = SeatsCount;

        public string Note { get; set; } = Note;
        public Guid? CarOwnerId { get; set; }
        public string CarOwnerName { get; set; } = CarOwnerName;
        public string CarOwnerSurname { get; set; } = CarOwnerSurname;
        public string? Img { get; set; }

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<CarEntity, CarListModel>();
            }
        }
    }
}

