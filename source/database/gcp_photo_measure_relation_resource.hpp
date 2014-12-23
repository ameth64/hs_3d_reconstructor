#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_GCP_PHOTO_MEASURE_RELATION_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_GCP_PHOTO_MEASURE_RELATION_RESOURCE_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class HS_EXPORT GCPPhotoMeasureRelationResource
{
public:
  enum FieldIdentifier
  {
    GPMR_FIELD_ID = 0,
    GPMR_FIELD_GCP_ID,
    GPMR_FIELD_PHOTO_MEASURE_ID,
    NUMBER_OF_GPMR_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_GPMR_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(GCPPhotoMeasureRelationResource)
};

}
}
}

#endif
