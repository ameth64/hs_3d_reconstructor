#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_PHOTO_ORIENTATION_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_PHOTO_ORIENTATION_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class PhotoOrientationResource
{
public:
  enum FieldIdentifier
  {
    PHOTO_ORIENTATION_FIELD_ID = 0,
    PHOTO_ORIENTATION_FIELD_FEATURE_MATCH_ID,
    PHOTO_ORIENTATION_FIELD_NAME,
    PHOTO_ORIENTATION_FIELD_PATH,
    NUMBER_OF_PHOTO_ORIENTATION_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_PHOTO_ORIENTATION_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(PhotoOrientationResource)

};

}
}
}

#endif
