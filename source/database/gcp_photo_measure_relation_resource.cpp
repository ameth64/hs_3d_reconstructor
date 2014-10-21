#include "database/gcp_photo_measure_relation_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field GCPPhotoMeasureRelationResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"GCP_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE},
  {"PHOTO_MEASURE_ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_NONE}
};
const char* GCPPhotoMeasureRelationResource::resource_name_ =
  "GCP_PHOTO_MEASURE_RELATION";

DEFINE_RESOURCE_COMMON_MEMBERS(GCPPhotoMeasureRelationResource)

}
}
}
