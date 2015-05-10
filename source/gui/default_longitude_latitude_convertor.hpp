#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_COMMON_DEFAULT_LONGITUDE_LATITUDE_CONVERTOR_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_COMMON_DEFAULT_LONGITUDE_LATITUDE_CONVERTOR_HPP_

#include "hs_cartographics/cartographics_conversion/convertor.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class HS_EXPORT DefaultLongitudeLatitudeConvertor
{
public:
  typedef double Scalar;
  typedef hs::cartographics::HS_CoordinateSystem<Scalar> CoordinateSystem;
  typedef hs::cartographics::conversion::Convertor<Scalar> Convertor;
  typedef Convertor::Coordinate Coordinate;
  typedef Convertor::CoordinateContainer CoordinateContainer;

private:
  typedef CoordinateSystem::Projection Projection;
  typedef CoordinateSystem::Datum Datum;
  typedef CoordinateSystem::General General;

public:
  int ConvertToCartisian(const CoordinateSystem& longitude_latitude_system,
                         const Coordinate& coordinate_longitude_latitude,
                         Coordinate& coordinate_cartisian) const;

  int GetDefaultCoordinateSystem(
    const CoordinateSystem& longitude_latitude_system,
    const CoordinateContainer& coordinates_longitude_latitude,
    CoordinateSystem& cartisian_system) const;
};

}
}
}

#endif