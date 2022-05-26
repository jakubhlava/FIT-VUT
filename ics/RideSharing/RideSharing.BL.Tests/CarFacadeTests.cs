using Microsoft.EntityFrameworkCore;
using RideSharing.BL.Facades;
using RideSharing.BL.Models;
using RideSharing.Common.Tests;
using RideSharing.Common.Tests.Seeds;
using RideSharing.DAL.Entities;
using System;
using System.Linq;
using System.Threading.Tasks;
using Xunit;
using Xunit.Abstractions;

namespace RideSharing.BL.Tests
{
    public sealed class CarFacadeTests : CRUDFacadeTestsBase
    {
        private readonly CarFacade _carFacadeSUT;

        public CarFacadeTests(ITestOutputHelper output) : base(output)
        {
            _carFacadeSUT = new CarFacade(UnitOfWorkFactory, Mapper);
        }

        [Fact]
        public async Task Create_WithNonExistingItem_DoesNotThrow()
        {
            var model = new CarDetailModel
            (
                Brand: @"CarBrand 1",
                Model: @"Test model",
                Type: "",
                RegistrationDate: new DateTime(2022, 06, 15, 15, 00, 00),
                SeatsCount: 5,
                CarOwnerNumber: UserSeeds.Petr.Number,
                CarOwnerId: UserSeeds.Petr.Id,
                CarOwnerName: UserSeeds.Petr.Name,
                CarOwnerSurname: UserSeeds.Petr.Surname);

            var _ = await _carFacadeSUT.SaveAsync(model);
        }

        [Fact]
        public async Task GetAll_Single_SeededOctavia()
        {
            var cars = await _carFacadeSUT.GetAsync();
            var car = cars.Single(i => i.Id == CarSeeds.Octavia.Id);

            DeepAssert.Equal(Mapper.Map<CarEntity, CarListModel>(CarSeeds.Octavia), car);
        }

        [Fact]
        public async Task GetById_SeededOctavia()
        {
            var car = await _carFacadeSUT.GetAsync(CarSeeds.Octavia.Id);
            DeepAssert.Equal(Mapper.Map<CarDetailModel>(CarSeeds.Octavia), car, nameof(car.Rides));
        }


        [Fact]
        public async Task SeededBMW_DeleteById_Deleted()
        {
            await _carFacadeSUT.DeleteAsync(CarSeeds.Octavia.Id);

            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            Assert.False(await dbxAssert.Cars.AnyAsync(i => i.Id == CarSeeds.Octavia.Id));
        }


        [Fact]
        public async Task NewCar_InsertOrUpdate_CarAdded()
        {
            //Arrange
            var car = new CarDetailModel(
                Brand: "BMW",
                Model: "X9",
                Type: "SUV",
                RegistrationDate: new DateTime(2022, 06, 15, 15, 00, 00),
                SeatsCount: 5,
                CarOwnerNumber: UserSeeds.Karel.Number,
                CarOwnerId: UserSeeds.Karel.Id,
                CarOwnerName: UserSeeds.Karel.Name,
                CarOwnerSurname: UserSeeds.Karel.Surname
            );

            //Act
            car = await _carFacadeSUT.SaveAsync(car);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var carFromDb = await dbxAssert.Cars.Include(i => i.CarOwner).SingleAsync(i => i.Id == car.Id);
            DeepAssert.Equal(car, Mapper.Map<CarDetailModel>(carFromDb));
        }

        [Fact]
        public async Task SeededBMW_InsertOrUpdate_CarUpdated()
        {
            //Arrange
            var car = new CarDetailModel
            (
                Brand: CarSeeds.Octavia.Brand,
                Model: CarSeeds.Octavia.Model,
                Type: "",
                RegistrationDate: new DateTime(2022, 06, 15, 15, 00, 00),
                SeatsCount: 5,
                CarOwnerNumber: UserSeeds.Petr.Number,
                CarOwnerId: UserSeeds.Petr.Id,
                CarOwnerName: UserSeeds.Petr.Name,
                CarOwnerSurname: UserSeeds.Petr.Surname
            )
            {
                Id = CarSeeds.Octavia.Id
            };
            car.Brand += "updated";
            car.Model += "updated";

            //Act
            await _carFacadeSUT.SaveAsync(car);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var carFromDb = await dbxAssert.Cars.Include(i => i.CarOwner).SingleAsync(i => i.Id == car.Id);
            DeepAssert.Equal(car, Mapper.Map<CarDetailModel>(carFromDb));
        }
    }
}