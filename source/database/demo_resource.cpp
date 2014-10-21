#include "database/demo_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field DemoResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_UNIQUE},
  {"RATIO", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE}
};

const char* DemoResource::resource_name_ = "DEMO";

DEFINE_RESOURCE_COMMON_MEMBERS(DemoResource)

}
}
}
