using AutoMapper;
using Microsoft.EntityFrameworkCore;
using RideSharing.DAL.Entities;

namespace RideSharing.DAL.UnitOfWork;

public interface IRepository<TEntity> where TEntity : class, IEntity
{
    IQueryable<TEntity> Get();
    void Delete(Guid entityId);
    public DbContext Context { get; set; }

    Task<TEntity> InsertOrUpdateAsync<TModel>(
        TModel model,
        IMapper mapper,
        CancellationToken cancellationToken = default) where TModel : class;
}