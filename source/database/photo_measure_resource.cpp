#include "database/photo_measure_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field PhotoMeasureResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"PHOTO_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"X", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE},
  {"Y", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE}
};

const char* PhotoMeasureResource::resource_name_ = "PHOTO_MEASURE";

DEFINE_RESOURCE_COMMON_MEMBERS(PhotoMeasureResource)

}
}
}
