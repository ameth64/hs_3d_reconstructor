#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_POINT_CLOUD_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_POINT_CLOUD_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class PointCloudResource
{
public:
  enum FieldIdentifier
  {
    POINT_CLOUD_FIELD_ID = 0,
    POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID,
    POINT_CLOUD_FIELD_NAME,
    POINT_CLOUD_FIELD_PATH,
    NUMBER_OF_POINT_CLOUD_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_POINT_CLOUD_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(PointCloudResource)
};

}
}
}

#endif
