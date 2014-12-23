#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_PHOTO_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_PHOTO_RESOURCE_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class HS_EXPORT PhotoResource
{
public:
  enum FieldIdentifier
  {
    PHOTO_FIELD_ID = 0,
    PHOTO_FIELD_PHOTOGROUP_ID,
    PHOTO_FIELD_PATH,
    PHOTO_FIELD_POS_X,
    PHOTO_FIELD_POS_Y,
    PHOTO_FIELD_POS_Z,
    PHOTO_FIELD_PITCH,
    PHOTO_FIELD_ROLL,
    PHOTO_FIELD_HEADING,
    NUMBER_OF_PHOTO_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_PHOTO_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(PhotoResource)
};

}
}
}

#endif
