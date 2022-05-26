using AutoMapper;
using Microsoft.EntityFrameworkCore;
using RideSharing.BL.Models;
using RideSharing.DAL.Entities;
using RideSharing.DAL.UnitOfWork;

namespace RideSharing.BL.Facades;

public class RideFacade : CRUDFacade<RideEntity, RideListModel, RideDetailModel>
{
    public RideFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
    {

    }

    public async Task<IEnumerable<RideListModel>> GetFilteredAsync(string _from, string _to, DateTime after)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<RideEntity>()
            .Get()
            .Where(ent => EF.Functions.Like(ent.CityFrom, $"%{_from}%"))
            .Where(ent => EF.Functions.Like(ent.CityTo, $"%{_to}%"))
            .Where(ent => ent.DepartureTime >= after)
            .Include(ent => ent.Passengers)
            .Include(ent => ent.Car)
            .ThenInclude(carent => carent.CarOwner);
        return await _mapper.ProjectTo<RideListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }

    public async Task<IEnumerable<RideListModel>> GetRidesAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<RideEntity>()
            .Get()
            .Where(ent => ent.DriverId == id);
        return await _mapper.ProjectTo<RideListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }

    public async Task<IEnumerable<RideListModel>> GetPassengerRidesAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<RideEntity>()
            .Get()
            .Where(ent => ent.Passengers.Any(y => y.Id == id));
        return await _mapper.ProjectTo<RideListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }

    public async Task<RideDetailModel?> GetFullDetail(Guid rideId)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<RideEntity>()
            .Get()
            .Where(ride => ride.Id == rideId)
            .Include(ent => ent.Passengers)
            .Include(ent => ent.Driver)
            .Include(ent => ent.Car)
            .ThenInclude(ent => ent.CarOwner);
        return await _mapper.ProjectTo<RideDetailModel>(query).SingleOrDefaultAsync().ConfigureAwait(false);
    }

    public async Task ForceRemoveRideUserRelations(Guid rideId)
    {
        await using var uow = _unitOfWorkFactory.Create();
        await uow.GetRepository<RideEntity>()
            .Context.Database.ExecuteSqlRawAsync($"DELETE FROM RideUser WHERE RideId='{rideId}'").ConfigureAwait(false);
        await uow.CommitAsync();
    }

}