#include "database/photo_orientation_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field PhotoOrientationResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"FEATURE_MATCH_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"FLAG", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"COORDINATE_SYSTEM", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE}
};
const char* PhotoOrientationResource::resource_name_ = "PHOTO_ORIENTATION";
DEFINE_RESOURCE_COMMON_MEMBERS(PhotoOrientationResource)

}
}
}
