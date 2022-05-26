using RideSharing.BL.Models;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace RideSharing.App.Wrappers
{
    public class CarWrapper : ModelWrapper<CarDetailModel>
    {
        public CarWrapper(CarDetailModel model)
            : base(model)
        {
        }

        public string? Brand
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? Model
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? Type
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public DateTime RegistrationDate
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }
        public int SeatsCount
        {
            get => GetValue<int>();
            set => SetValue(value);
        }
        public Guid CarOwnerId
        {
            get => GetValue<Guid>();
            set => SetValue(value);
        }
        public string? CarOwnerName
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CarOwnerSurname
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CarOwnerNumber
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? Img
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? Note
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string? CarOwnerUserImg
        {
            get => GetValue<string>();
            set => SetValue(value);
        }

        private void InitializeCollectionProperties(CarDetailModel model)
        {
            //RIDE WRAPPER NEEDED
            //if (model.Rides == null)
            //{
            //    throw new ArgumentException("Rides cannot be null");
            //}
            //Rides.AddRange(model.Rides.Select(e => new IngredientAmountWrapper(e)));

            //RegisterCollection(Rides, model.Rides);
        }

        //public ObservableCollection<IngredientAmountWrapper> Ingredients { get; set; } = new();

        public override IEnumerable<ValidationResult> Validate(ValidationContext validationContext)
        {
            if (string.IsNullOrWhiteSpace(Brand))
            {
                yield return new ValidationResult($"{nameof(Brand)} is required", new[] { nameof(Brand) });
            }

            if (string.IsNullOrWhiteSpace(Model))
            {
                yield return new ValidationResult($"{nameof(Model)} is required", new[] { nameof(Model) });
            }

            if (string.IsNullOrWhiteSpace(Type))
            {
                yield return new ValidationResult($"{nameof(Type)} is required", new[] { nameof(Type) });
            }

            if (RegistrationDate == DateTime.MinValue)
            {
                yield return new ValidationResult($"{nameof(RegistrationDate)} is required", new[] { nameof(RegistrationDate) });
            }

            if (SeatsCount == 0)
            {
                yield return new ValidationResult($"{nameof(SeatsCount)} is required", new[] { nameof(SeatsCount) });
            }

            if (CarOwnerId == Guid.Empty)
            {
                yield return new ValidationResult($"{nameof(CarOwnerId)} is required", new[] { nameof(CarOwnerId) });
            }

            if (string.IsNullOrWhiteSpace(CarOwnerName))
            {
                yield return new ValidationResult($"{nameof(CarOwnerName)} is required", new[] { nameof(CarOwnerName) });
            }

            if (string.IsNullOrWhiteSpace(CarOwnerSurname))
            {
                yield return new ValidationResult($"{nameof(CarOwnerSurname)} is required", new[] { nameof(CarOwnerSurname) });
            }

            if (string.IsNullOrWhiteSpace(CarOwnerNumber))
            {
                yield return new ValidationResult($"{nameof(CarOwnerNumber)} is required", new[] { nameof(CarOwnerNumber) });
            }
        }

        public static implicit operator CarWrapper(CarDetailModel detailModel)
            => new(detailModel);

        public static implicit operator CarDetailModel(CarWrapper wrapper)
            => wrapper.BaseModel;
    }
}
