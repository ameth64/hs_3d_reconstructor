#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_BLOCK_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_BLOCK_RESOURCE_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class HS_EXPORT BlockResource
{
public:
  enum FieldIdentifier
  {
    BLOCK_FIELD_ID,
    BLOCK_FIELD_NAME,
    BLOCK_FIELD_DESCRIPTION,
    NUMBER_OF_BLOCK_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_BLOCK_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(BlockResource)
};

}
}
}

#endif
