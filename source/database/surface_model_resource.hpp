#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_SURFACE_MODEL_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_SURFACE_MODEL_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class SurfaceModelResource
{
public:
  enum FieldIdentifier
  {
    SURFACE_MODEL_FIELD_ID = 0,
    SURFACE_MODEL_FIELD_POINT_CLOUD_ID,
    SURFACE_MODEL_FIELD_NAME,
    SURFACE_MODEL_FIELD_PATH,
    NUMBER_OF_SURFACE_MODEL_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_SURFACE_MODEL_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(SurfaceModelResource)
};

}
}
}

#endif
