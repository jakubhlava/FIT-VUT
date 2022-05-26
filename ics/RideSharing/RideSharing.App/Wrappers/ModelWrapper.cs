using RideSharing.App.ViewModels;
using RideSharing.BL.Models;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Runtime.CompilerServices;

namespace RideSharing.App.Wrappers
{
    /// <typeparam name="T"></typeparam>
    public abstract class ModelWrapper<T> : ViewModelBase, IModel, IValidatableObject
        where T : IModel
    {
        protected ModelWrapper(T? model)
        {
            if (model == null)
            {
                throw new ArgumentNullException(nameof(model));
            }

            BaseModel = model;
        }

        public Guid Id
        {
            get => GetValue<Guid>();
            set => SetValue(value);
        }

        public T BaseModel { get; }

        protected TValue? GetValue<TValue>([CallerMemberName] string? propertyName = null)
        {
            var propertyInfo = BaseModel.GetType().GetProperty(propertyName ?? string.Empty);
            return propertyInfo?.GetValue(BaseModel) is TValue
                ? (TValue?)propertyInfo.GetValue(BaseModel)
                : default;
        }

        protected void SetValue<TValue>(TValue value, [CallerMemberName] string? propertyName = null)
        {
            var propertyInfo = BaseModel.GetType().GetProperty(propertyName ?? string.Empty);
            var currentValue = propertyInfo?.GetValue(BaseModel);
            if (!Equals(currentValue, value))
            {
                propertyInfo?.SetValue(BaseModel, value);
                OnPropertyChanged(propertyName);
            }
        }

        protected void RegisterCollection<TWrapper, TModel>(
            ObservableCollection<TWrapper> wrapperCollection,
            ICollection<TModel> modelCollection)
            where TWrapper : ModelWrapper<TModel>, IModel
            where TModel : IModel
        {
            wrapperCollection.CollectionChanged += (s, e) =>
            {
                modelCollection.Clear();
                foreach (var model in wrapperCollection.Select(i => i.BaseModel))
                {
                    modelCollection.Add(model);
                }
            };
        }

        public bool IsValid => !Validate(new ValidationContext(this)).Any();

        public virtual IEnumerable<ValidationResult> Validate(ValidationContext validationContext)
        {
            yield break;
        }
    }
}
