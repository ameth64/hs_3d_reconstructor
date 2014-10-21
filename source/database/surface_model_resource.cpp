#include "database/surface_model_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field SurfaceModelResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"POINT_CLOUD_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"PATH", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE}
};
const char* SurfaceModelResource::resource_name_ = "SURFACE_MODEL";
DEFINE_RESOURCE_COMMON_MEMBERS(SurfaceModelResource)

}
}
}
