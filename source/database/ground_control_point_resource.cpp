#include "database/ground_control_point_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field GroundControlPointResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_UNIQUE},
  {"DESCRIPTION", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"X", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE},
  {"Y", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE},
  {"Z", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE}
};

const char* GroundControlPointResource::resource_name_ =
  "GROUND_CONTROL_POINT";

DEFINE_RESOURCE_COMMON_MEMBERS(GroundControlPointResource)

}
}
}
