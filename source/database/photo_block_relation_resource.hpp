#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_PHOTO_BLOCK_RELATION_RESOURCE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_PHOTO_BLOCK_RELATION_RESOURCE_HPP_

#include "database/resource_utility.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class PhotoBlockRelationResource
{
public:
  enum FieldIdentifier
  {
    PBR_FIELD_ID = 0,
    PBR_FIELD_PHOTO_ID,
    PBR_FIELD_BLOCK_ID,
    NUMBER_OF_PBR_FIELDS
  };
  static const int number_of_fields_ = NUMBER_OF_PBR_FIELDS;
  DECLARE_RESOURCE_COMMON_MEMBERS(PhotoBlockRelationResource)

};

}
}
}

#endif
