#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_TEXTURE_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_TEXTURE_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class TextureResource
{
public:
  enum FieldIdentifier
  {
    TEXTURE_FIELD_ID = 0,
    TEXTURE_FIELD_SURFACE_MODEL_ID,
    TEXTURE_FIELD_NAME,
    TEXTURE_FIELD_PATH,
    NUMBER_OF_TEXTURE_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_TEXTURE_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(TextureResource)
};

}
}
}

#endif
