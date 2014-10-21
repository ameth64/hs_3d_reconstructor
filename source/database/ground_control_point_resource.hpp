#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_GROUND_CONTROL_POINT_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_GROUND_CONTROL_POINT_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class GroundControlPointResource
{
public:
  enum FieldIdentifier
  {
    GCP_FIELD_ID = 0,
    GCP_FIELD_NAME,
    GCP_FIELD_DESCRIPTION,
    GCP_FIELD_X,
    GCP_FIELD_Y,
    GCP_FIELD_Z,
    NUMBER_OF_GCP_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_GCP_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(GroundControlPointResource)
};

}
}
}

#endif
