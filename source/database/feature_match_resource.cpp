#include "database/feature_match_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field FeatureMatchResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"BLOCK_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE},
  {"PATH", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE}
};
const char* FeatureMatchResource::resource_name_ = "FEATURE_MATCH";
DEFINE_RESOURCE_COMMON_MEMBERS(FeatureMatchResource)

}
}
}
