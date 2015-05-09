#include "default_longitude_latitude_convertor.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

int DefaultLongitudeLatitudeConvertor::ConvertToCartisian(
  const CoordinateSystem& longitude_latitude_system,
  const Coordinate& coordinate_longitude_latitude,
  Coordinate& coordinate_cartisian) const
{
  Projection projection(Projection::TYPE_GEOCENTRIC);
  CoordinateSystem cartisian_system(
    projection,
    longitude_latitude_system.datum(),
    longitude_latitude_system.general_parameters());

  DefaultConvertor convertor;
  return convertor.CoordinateSystemToCoordinateSystem(
    longitude_latitude_system, cartisian_system,
    coordinate_longitude_latitude, coordinate_cartisian);
}

}
}
}