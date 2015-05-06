#include "database/point_cloud_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field PointCloudResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"PHOTO_ORIENTATION_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"PATH", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"FLAG", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE}
};
const char* PointCloudResource::resource_name_ = "POINT_CLOUD";
DEFINE_RESOURCE_COMMON_MEMBERS(PointCloudResource)

}
}
}
