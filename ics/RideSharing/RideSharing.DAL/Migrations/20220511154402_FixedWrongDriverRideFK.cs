using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace RideSharing.DAL.Migrations
{
    public partial class FixedWrongDriverRideFK : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "DriverUserId",
                table: "Rides");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<Guid>(
                name: "DriverUserId",
                table: "Rides",
                type: "uniqueidentifier",
                nullable: true);
        }
    }
}
