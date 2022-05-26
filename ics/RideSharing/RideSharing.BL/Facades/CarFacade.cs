using AutoMapper;
using Microsoft.EntityFrameworkCore;
using RideSharing.BL.Models;
using RideSharing.DAL.Entities;
using RideSharing.DAL.UnitOfWork;

namespace RideSharing.BL.Facades;

public class CarFacade : CRUDFacade<CarEntity, CarListModel, CarDetailModel>
{
    public CarFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
    {

    }


    public async Task<IEnumerable<CarListModel>> GetFilteredAsync(Guid? user)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<CarEntity>()
            .Get()
            .Where(ent => ent.CarOwnerId == user);
        return await _mapper.ProjectTo<CarListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }
}