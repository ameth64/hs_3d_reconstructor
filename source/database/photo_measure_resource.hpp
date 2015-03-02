#ifndef _HS_3D_RECONSTRUCTIO_DATABASE_PHOTO_MEASURE_RELATION_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTIO_DATABASE_PHOTO_MEASURE_RELATION_RESOURCE_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

/**
 *  This resource descript photo orientation,gcp and photo.
 */
class HS_EXPORT PhotoMeasureResource
{
public:
  enum FieldIdentifier
  {
    PHOTO_MEASURE_FIELD_ID = 0,
    PHOTO_MEASURE_FIELD_PHOTO_ORIENTATION_ID,
    PHOTO_MEASURE_FIELD_GCP_ID,
    PHOTO_MEASURE_FIELD_PHOTO_ID,
    PHOTO_MEASURE_FIELD_GCP_TYPE,
    PHOTO_MEASURE_FIELD_POS_X,
    PHOTO_MEASURE_FIELD_POS_Y,
    NUMBER_OF_PHOTO_MEASURE_FIELDS
  };

  enum GCPType
  {
    TYPE_CONTROL_POINT,
    TYPE_CHECK_POINT
  };

  static const int number_of_fields_ = NUMBER_OF_PHOTO_MEASURE_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(PhotoMeasureResource)
};

}
}
}

#endif
