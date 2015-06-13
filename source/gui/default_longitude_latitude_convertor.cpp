#include <limits>

#include "default_longitude_latitude_convertor.hpp"

namespace hs
{
namespace recon
{
namespace gui
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

  Convertor convertor;
  return convertor.CoordinateSystemToCoordinateSystem(
    longitude_latitude_system, cartisian_system,
    coordinate_longitude_latitude, coordinate_cartisian);
}

int DefaultLongitudeLatitudeConvertor::GetDefaultCoordinateSystem(
  const CoordinateSystem& longitude_latitude_system,
  const CoordinateContainer& coordinates_longitude_latitude,
  CoordinateSystem& cartisian_system) const
{
  if (longitude_latitude_system.projection().projection_type() !=
      Projection::TYPE_LAT_LONG) return -1;
  Coordinate min, max;
  min << std::numeric_limits<Scalar>::max(),
         std::numeric_limits<Scalar>::max(),
         std::numeric_limits<Scalar>::max();
  max << -std::numeric_limits<Scalar>::max(),
         -std::numeric_limits<Scalar>::max(),
         -std::numeric_limits<Scalar>::max();
  auto itr_coord = coordinates_longitude_latitude.begin();
  auto itr_coord_end = coordinates_longitude_latitude.end();
  for (; itr_coord != itr_coord_end; ++itr_coord)
  {
    min[0] = std::min(min[0], (*itr_coord)[0]);
    min[1] = std::min(min[1], (*itr_coord)[1]);
    max[0] = std::max(max[0], (*itr_coord)[0]);
    max[1] = std::max(max[1], (*itr_coord)[1]);
  }
  Coordinate center = (min + max) * 0.5;
  Projection projection(Projection::TYPE_TRANSVERSE_MERCATOR);
  projection.SetParameter(
    Projection::PARAMETER_SCALING_FACTOR, Scalar(1));
  projection.SetParameter(
    Projection::PARAMETER_CENTRAL_MERIDIAN, std::floor(center[0]));
  projection.SetParameter(
    Projection::PARAMETER_FALSE_EASTING, Scalar(500000));
  cartisian_system.SetProjection(projection);
  cartisian_system.SetDatum(longitude_latitude_system.datum());
  cartisian_system.SetGeneral(
    longitude_latitude_system.general_parameters());

  return 0;
}

}
}
}