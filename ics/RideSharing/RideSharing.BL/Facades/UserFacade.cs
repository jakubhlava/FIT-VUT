using AutoMapper;
using Microsoft.EntityFrameworkCore;
using RideSharing.BL.Models;
using RideSharing.DAL.Entities;
using RideSharing.DAL.UnitOfWork;

namespace RideSharing.BL.Facades;

public class UserFacade : CRUDFacade<UserEntity, UserListModel, UserDetailModel>
{
    public UserFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
    {

    }

    public async Task<UserDetailModel?> GetUserForAddCarAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<UserEntity>()
            .Get()
            .Where(e => e.Id == id);
        return await _mapper.ProjectTo<UserDetailModel>(query).SingleOrDefaultAsync().ConfigureAwait(false);

    }

    public async Task<UserDetailModel?> GetUserWithCarsAndRidesAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<UserEntity>()
            .Get()
            .Where(e => e.Id == id)
            .Include(ent => ent.PassengerRides)
            .Include(ent => ent.DriverRides)
            .Include(ent => ent.OwnedCars)
            .ThenInclude(carEnt => carEnt.Rides);
            
        return await _mapper.ProjectTo<UserDetailModel>(query).SingleOrDefaultAsync().ConfigureAwait(false);
    }

    public async Task<UserDetailModel?> GetUserWithRidesAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<UserEntity>()
            .Get()
            .Where(e => e.Id == id)
            .Include(ent => ent.PassengerRides)
            .Include(ent => ent.DriverRides);

        return await _mapper.ProjectTo<UserDetailModel>(query).SingleOrDefaultAsync().ConfigureAwait(false);
    }

}