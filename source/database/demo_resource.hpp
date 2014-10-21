#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_DEMO_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_DEMO_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class DemoResource
{
public:
  enum FieldIdentifier
  {
    DEMO_FIELD_ID,
    DEMO_FIELD_NAME,
    DEMO_FIELD_RATIO,
    NUMBER_OF_DEMO_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_DEMO_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(DemoResource)
};

}
}
}

#endif
