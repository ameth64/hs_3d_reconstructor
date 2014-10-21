#include "database/photo_block_relation_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field PhotoBlockRelationResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"PHOTO_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"BLOCK_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE}
};

const char* PhotoBlockRelationResource::resource_name_ =
  "PHOTO_BLOCK_RELATION";

DEFINE_RESOURCE_COMMON_MEMBERS(PhotoBlockRelationResource)

}
}
}
