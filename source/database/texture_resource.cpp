#include "database/texture_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field TextureResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"SURFACE_MODEL_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"FLAG", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE}
};
const char* TextureResource::resource_name_ = "TEXTURE";
DEFINE_RESOURCE_COMMON_MEMBERS(TextureResource)

}
}
}
