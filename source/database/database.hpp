#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_HPP_

#include <bitset>

#include <sqlite3.h>

namespace hs
{
namespace recon
{
namespace db
{

class ResourceBase;

class Database
{
public:
  typedef unsigned int Identifier;
  typedef double Float;

  //enum ResourceType
  //{
  //  UNKNOWN_RESOURCE = 0,
  //  BLOCK_RESOURCE,
  //  FEATURE_MATCH_RESOURCE,
  //  PHOTO_ORIENTATION_RESOURCE,
  //  POINT_CLOUD_RESOURCE,
  //  SURFACE_MODEL_RESOURCE,
  //  TEXTURE_RESOURCE,
  //  PHOTO_RESOURCE,
  //  PHOTOGROUP_RESOURCE,
  //  GROUND_CONTROL_POINT_RESOURCE,
  //  PHOTO_MEASURE_RESOURCE,
  //  NUMBER_OF_RESOURCE_TYPES
  //};

public:
  Database();

  int Open(const std::string& db_path);

  int Create(const std::string& db_path);

  sqlite3* sqlite_db();

private:
  sqlite3* sqlite_db_;
};

}
}
}

#endif
