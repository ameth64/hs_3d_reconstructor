#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_POINT_CLOUD_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_POINT_CLOUD_RESOURCE_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class HS_EXPORT PointCloudResource
{
public:
  enum FieldIdentifier
  {
    POINT_CLOUD_FIELD_ID = 0,
    POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID,
    POINT_CLOUD_FIELD_NAME,
    POINT_CLOUD_FIELD_FLAG,
    NUMBER_OF_POINT_CLOUD_FIELDS
  };

  enum Flag
  {
    FLAG_NOT_COMPLETED = 0,
    FLAG_COMPLETED
  };

  static const int number_of_fields_ = NUMBER_OF_POINT_CLOUD_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(PointCloudResource)
};

}
}
}

#endif
