#include <fstream>

#include "database/database.hpp"

namespace hs
{
namespace recon
{
namespace db
{

Database::Database()
  : sqlite_db_(nullptr)
{
}

Database::~Database()
{
  Close();
}

int Database::Open(const std::string& database_file)
{
  boost::filesystem::path database_file_path = database_file;
  if (boost::filesystem::exists(database_file_path))
  {
    Close();
    database_file_path_ = database_file;
    int result = sqlite3_open(database_file_path_.string().c_str(), &sqlite_db_);
    if (result == SQLITE_OK)
    {
      return NO_ERROR;
    }
    else
    {
      sqlite3_close(sqlite_db_);
      sqlite_db_ = nullptr;
      return ERROR_FAIL_TO_OPEN_SQLITE_DATABASE;
    }
  }
  else
  {
    return ERROR_DATABASE_FILE_NOT_EXIST;
  }
}

int Database::Create(const std::string& database_directory)
{
  boost::filesystem::path database_directory_path = database_directory;
  if (boost::filesystem::exists(database_directory_path))
  {
    return ERROR_DATABASE_DIRECTORY_EXIST;
  }
  Close();
  if (!boost::filesystem::create_directory(database_directory_path))
  {
    return ERROR_CANT_CREATE_DATABASE_DIRECTORY;
  }
  database_file_path_ = database_directory_path;
  database_file_path_ /= database_directory_path.stem();
  database_file_path_ += ".3db";
  int result = sqlite3_open(database_file_path_.string().c_str(), &sqlite_db_);
  if (result == SQLITE_OK)
  {
    return NO_ERROR;
  }
  else
  {
    sqlite3_close(sqlite_db_);
    sqlite_db_ = nullptr;
    return ERROR_FAIL_TO_OPEN_SQLITE_DATABASE;
  }
}

int Database::Close()
{
  int result = sqlite3_close(sqlite_db_);
  sqlite_db_ = nullptr;
  if (result != 0)
  {
    return ERROR_FAIL_TO_CLOSE_SQLITE_DATABASE;
  }
  else
  {
    database_file_path_ = boost::filesystem::path();
    return NO_ERROR;
  }
}

sqlite3* Database::sqlite_db()
{
  return sqlite_db_;
}

std::string Database::Name() const
{
  return database_file_path_.stem().string();
}

std::string Database::DatabaseFilePath() const
{
  return database_file_path_.string();
}

}
}
}
