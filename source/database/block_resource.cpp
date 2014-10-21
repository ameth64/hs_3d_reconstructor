#include "database/block_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Field BlockResource::fields_[number_of_fields_] =
{
  {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
  {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_UNIQUE},
  {"DESCRIPTION", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_NONE}
};

const char* BlockResource::resource_name_ = "BLOCK";

DEFINE_RESOURCE_COMMON_MEMBERS(BlockResource)

}
}
}
