#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_COMMON_DEFAULT_LONGITUDE_LATITUDE_CONVERTOR_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_COMMON_DEFAULT_LONGITUDE_LATITUDE_CONVERTOR_HPP_

#include "hs_cartographics/cartographics_conversion/convertor.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT DefaultLongitudeLatitudeConvertor
{
public:
  typedef hs::cartographics::HS_CoordinateSystem<double> CoordinateSystem;
  typedef hs::cartographics::conversion::Convertor<double> DefaultConvertor;
  typedef DefaultConvertor::Coordinate Coordinate;

private:
  typedef CoordinateSystem::Projection Projection;
  typedef CoordinateSystem::Datum Datum;
  typedef CoordinateSystem::General General;

public:
  int ConvertToCartisian(const CoordinateSystem& longitude_latitude_system,
                         const Coordinate& coordinate_longitude_latitude,
                         Coordinate& coordinate_cartisian) const;
};

}
}
}

#endif