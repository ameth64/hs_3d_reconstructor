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

int Database::Open(const std::string& db_path)
{
  int result = sqlite3_open(db_path.c_str(), &sqlite_db_);
  if (result == SQLITE_OK)
  {
    return 0;
  }
  else
  {
    sqlite3_close(sqlite_db_);
    sqlite_db_ = nullptr;
    return -1;
  }
}

int Database::Create(const std::string& db_path)
{
  //Clear file content.
  std::ofstream clear_stream;
  clear_stream.open(db_path.c_str(),
                    std::ofstream::out | std::ofstream::trunc);
  clear_stream.close();

  int result = sqlite3_open(db_path.c_str(), &sqlite_db_);
  if (result == SQLITE_OK)
  {
    return 0;
  }
  else
  {
    sqlite3_close(sqlite_db_);
    sqlite_db_ = nullptr;
    return -1;
  }
}

sqlite3* Database::sqlite_db()
{
  return sqlite_db_;
}

}
}
}
