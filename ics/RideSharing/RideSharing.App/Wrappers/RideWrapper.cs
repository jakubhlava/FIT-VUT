using RideSharing.BL.Models;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace RideSharing.App.Wrappers
{
    public class RideWrapper : ModelWrapper<RideDetailModel>
    {
        public RideWrapper(RideDetailModel model)
            : base(model)
        {
        }

        public string? CityFrom
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CityTo
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public DateTime DepartureTime
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }
        public DateTime ArrivalTime
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }
        public string? Note
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public Guid DriverUserId
        {
            get => GetValue<Guid>();
            set => SetValue(value);
        }
        public string? DriverName
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? DriverSurname
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? DriverNumber
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? DriverUserImg
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public Guid CarId
        {
            get => GetValue<Guid>();
            set => SetValue(value);
        }
        public string? CarBrand
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CarModel
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CarType
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public DateTime CarRegistrationDate
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }
        public int CarSeatsCount
        {
            get => GetValue<int>();
            set => SetValue(value);
        }
        public string? CarImg
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CarNote
        {
            get => GetValue<string>();
            set => SetValue(value);
        }

        public override IEnumerable<ValidationResult> Validate(ValidationContext validationContext)
        {
            if (string.IsNullOrWhiteSpace(CityFrom))
            {
                yield return new ValidationResult($"{nameof(CityFrom)} is required", new[] { nameof(CityFrom) });
            }

            if (string.IsNullOrWhiteSpace(CityTo))
            {
                yield return new ValidationResult($"{nameof(CityTo)} is required", new[] { nameof(CityTo) });
            }

            if (DepartureTime == DateTime.MinValue)
            {
                yield return new ValidationResult($"{nameof(DepartureTime)} is required", new[] { nameof(DepartureTime) });
            }

            if (ArrivalTime == DateTime.MinValue)
            {
                yield return new ValidationResult($"{nameof(ArrivalTime)} is required", new[] { nameof(ArrivalTime) });
            }

            if (DriverUserId == Guid.Empty)
            {
                yield return new ValidationResult($"{nameof(DriverUserId)} is required", new[] { nameof(DriverUserId) });
            }

            if (string.IsNullOrWhiteSpace(DriverName))
            {
                yield return new ValidationResult($"{nameof(DriverName)} is required", new[] { nameof(DriverName) });
            }

            if (string.IsNullOrWhiteSpace(DriverSurname))
            {
                yield return new ValidationResult($"{nameof(DriverSurname)} is required", new[] { nameof(DriverSurname) });
            }

            if (string.IsNullOrWhiteSpace(DriverNumber))
            {
                yield return new ValidationResult($"{nameof(DriverNumber)} is required", new[] { nameof(DriverNumber) });
            }

            if (CarId == Guid.Empty)
            {
                yield return new ValidationResult($"{nameof(CarId)} is required", new[] { nameof(CarId) });
            }

            if (string.IsNullOrWhiteSpace(CarBrand))
            {
                yield return new ValidationResult($"{nameof(CarBrand)} is required", new[] { nameof(CarBrand) });
            }

            if (string.IsNullOrWhiteSpace(CarModel))
            {
                yield return new ValidationResult($"{nameof(CarModel)} is required", new[] { nameof(CarModel) });
            }

            if (string.IsNullOrWhiteSpace(CarType))
            {
                yield return new ValidationResult($"{nameof(CarType)} is required", new[] { nameof(CarType) });
            }

            if (CarRegistrationDate == DateTime.MinValue)
            {
                yield return new ValidationResult($"{nameof(CarRegistrationDate)} is required", new[] { nameof(CarRegistrationDate) });
            }

            if (CarSeatsCount == 0)
            {
                yield return new ValidationResult($"{nameof(CarSeatsCount)} is required", new[] { nameof(CarSeatsCount) });
            }
        }

        public static implicit operator RideWrapper(RideDetailModel detailModel)
            => new(detailModel);

        public static implicit operator RideDetailModel(RideWrapper wrapper)
            => wrapper.BaseModel;
    }
}
