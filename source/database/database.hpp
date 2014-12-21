#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_HPP_

#include <bitset>

#include <sqlite3.h>
#include <boost/filesystem.hpp>

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

public:
  Database();
  ~Database();

  enum ErrorCode
  {
    NO_ERROR = 0,
    ERROR_FAIL_TO_OPEN_SQLITE_DATABASE,
    ERROR_DATABASE_FILE_NOT_EXIST,
    ERROR_DATABASE_DIRECTORY_EXIST,
    ERROR_CANT_CREATE_DATABASE_DIRECTORY,
    ERROR_FAIL_TO_CLOSE_SQLITE_DATABASE,
    NUMBER_OF_ERROR_CODE
  };

  int Open(const std::string& database_file);

  int Create(const std::string& database_directory);

  int Close();

  sqlite3* sqlite_db();

  std::string Name() const;
  std::string DatabaseFilePath() const;

private:
  sqlite3* sqlite_db_;
  boost::filesystem::path database_file_path_;
};

}
}
}

#endif
