#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_RESOURCE_UTILITY_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_RESOURCE_UTILITY_HPP_

#include <vector>
#include <map>
#include <array>
#include <iostream>

#include <boost/format.hpp>
#include <sqlite3.h>

#include "database/database.hpp"
#include "database/field.hpp"
#include "database/condition.hpp"

namespace hs
{
namespace recon
{
namespace db
{

template <typename Resource_>
class ResourceUtility
{
public:
  typedef Resource_ Resource;

  typedef std::array<Value, Resource::number_of_fields_> Record;
  typedef std::map<Identifier, Record> RecordContainer;

  typedef std::array<Value, Resource::number_of_fields_> AddRequest;
  typedef std::vector<AddRequest> AddRequestContainer;
  typedef std::array<Value, Resource::number_of_fields_> AddedRecord;
  typedef std::map<Identifier, AddedRecord> AddedRecordContainer;

  typedef std::bitset<Resource::number_of_fields_> SelectMask;
  typedef std::map<int, Value> SelectedRecord;
  typedef std::map<Identifier, SelectedRecord> SelectedRecordContainer;

  typedef std::map<int, Value> UpdateRequest;
  typedef std::map<Identifier, UpdateRequest> UpdateRequestContainer;
  typedef std::map<int, Value> UpdatedRecord;
  typedef std::map<Identifier, UpdatedRecord> UpdatedRecordContainer;

  static Resource* Register(Database& database)
  {
    if (Resource::IsResourceExist(database))
    {
      Resource* resource = new Resource(database);
      return resource;
    }
    else
    {
      sqlite3_stmt* statements = NULL;
      std::string sql_text =
        boost::str(
          boost::format("CREATE TABLE %1%(") %
          Resource::resource_name_);
      std::string prefix;
      for (int i = 0; i < Resource::number_of_fields_; i++)
      {
        sql_text += prefix + SqlTableField(Resource::fields_[i]);
        prefix = ",";
      }
      sql_text += ");";
      int result = sqlite3_prepare_v2(database.sqlite_db(), sql_text.c_str(),
                                      -1, &statements, NULL);
      result = sqlite3_step(statements);
      sqlite3_finalize(statements);
      if (result == SQLITE_DONE)
      {
        Resource* resource = new Resource(database);
        return resource;
      }
      else
      {
        return nullptr;
      }
    }
  }

  static bool IsExist(Database& database)
  {
    sqlite3_stmt* statements = NULL;
    std::string sql_text =
      boost::str(
        boost::format("SELECT name FROM sqlite_master WHERE type=\'table\' "
                      "AND name=\'%1%\'") % Resource::resource_name_);
    int result = sqlite3_prepare_v2(
      database.sqlite_db(), sql_text.c_str(), -1, &statements, NULL);
    result = sqlite3_step(statements);
    sqlite3_finalize(statements);
    return result == SQLITE_ROW;
  }

  static int AddResource(Database& database,
                         const AddRequestContainer& add_requests,
                         AddedRecordContainer& added_records)
  {
    added_records.clear();
    auto itr_request = add_requests.begin();
    auto itr_request_end = add_requests.end();
    for (; itr_request != itr_request_end; ++itr_request)
    {
      std::string sql_text =
        boost::str(boost::format("INSERT INTO %1% VALUES(NULL") %
                   Resource::resource_name_);
      std::string prefix = ",";
      for (int i = 1; i < Resource::number_of_fields_; i++)
      {
        sql_text += prefix +
                    SqlFieldValue((*itr_request)[i]);
      }
      sql_text += ");";
      sqlite3_stmt* statements = NULL;
      int prepare_result = sqlite3_prepare_v2(database.sqlite_db(),
                                              sql_text.c_str(), -1,
                                              &statements, NULL);
      if (prepare_result != SQLITE_OK) continue;
      int step_result = sqlite3_step(statements);
      if (step_result == SQLITE_DONE)
      {
        AddedRecord added_record;
        Identifier id =
          Identifier(sqlite3_last_insert_rowid(database.sqlite_db()));
        added_record[0] = int(id);
        for (int i = 1; i < Resource::number_of_fields_; i++)
        {
          added_record[i] = (*itr_request)[i];
        }
        added_records[id] = added_record;
      }
    }
    return 0;
  }

  static int SelectResource(Database& database,
                            const SelectMask& selected_mask,
                            const Condition& condition,
                            SelectedRecordContainer& selected_records)
  {
    selected_records.clear();
    std::string sql_text;
    std::string prefix = "SELECT ";
    prefix += Resource::fields_[0].name;
    prefix += ", ";
    for (int i = 1; i < Resource::number_of_fields_; i++)
    {
      if (selected_mask[i])
      {
        sql_text += prefix + Resource::fields_[i].name;
        prefix = ", ";
      }
    }
    sql_text +=
      boost::str(boost::format(" FROM %1% ") % Resource::resource_name_);
    std::string condition_text = condition.sql_text();
    if (!condition_text.empty())
    {
      sql_text += "WHERE ";
      sql_text += condition_text + ";";
    }

    sqlite3_stmt* statements = NULL;
    int prepare_result = sqlite3_prepare_v2(database.sqlite_db(),
                                            sql_text.c_str(),
                                            -1, &statements, NULL);
    if (prepare_result != SQLITE_OK) return -1;
    while (1)
    {
      int step_result = sqlite3_step(statements);
      if (step_result != SQLITE_ROW) break;
      SelectedRecord selected_record;
      Identifier id = Identifier(sqlite3_column_int(statements, 0));
      selected_record[0] = Value(int(id));
      int col = 1;
      for (int i = 1; i < Resource::number_of_fields_; i++)
      {
        if (selected_mask[i])
        {
          Value value;
          FieldValueType type = Resource::fields_[i].value_type;
          if (type == FIELD_VALUE_INTEGER)
          {
            value = int(sqlite3_column_int(statements, col));
          }
          else if (type == FIELD_VALUE_TEXT)
          {

            value = std::string(
                      (const char*)(sqlite3_column_text(statements, col)));
          }
          else if (type == FIELD_VALUE_REAL)
          {
            value = Float(sqlite3_column_double(statements, col));
          }
          selected_record[i] = value;
          col++;
        }
      }
      selected_records[id] = selected_record;
    }

    return 0;
  }

  static int UpdateResource(Database& database,
                            const UpdateRequestContainer& update_requests,
                            UpdatedRecordContainer& updated_records)
  {
    updated_records.clear();
    auto itr_request = update_requests.begin();
    auto itr_request_end = update_requests.end();
    for (; itr_request != itr_request_end; ++itr_request)
    {
      std::string sql_text = boost::str(boost::format("UPDATE %1% SET ") %
                                        Resource::resource_name_);
      std::string prefix;
      auto itr_value = itr_request->second.begin();
      auto itr_value_end = itr_request->second.end();
      for (; itr_value != itr_value_end; ++itr_value)
      {
        if (itr_value->first > 0 &&
            itr_value->first < Resource::number_of_fields_)
        {
          const Field& field = Resource::fields_[itr_value->first];
          sql_text += prefix +
                      boost::str(
                        boost::format("%1% = %2%") %
                        field.name %
                        SqlFieldValue(itr_value->second));
          prefix = ", ";
        }
      }
      sql_text += boost::str(
                    boost::format(" WHERE %1% = %2%;") %
                    Resource::fields_[0].name %
                    itr_request->first);
      sqlite3_stmt* statements = NULL;
      int prepare_result = sqlite3_prepare_v2(database.sqlite_db(),
                                              sql_text.c_str(),
                                              -1, &statements, NULL);
      if (prepare_result != SQLITE_OK) continue;
      int step_result = sqlite3_step(statements);
      if (step_result == SQLITE_DONE)
      {
        updated_records[itr_request->first] = itr_request->second;
      }
    }
    return 0;
  }

  static int DeleteResource(Database& database, const Condition& condition)
  {
    std::string sql_text = "DELETE FROM ";
    sql_text += Resource::resource_name_;
    sql_text += " ";
    std::string condition_text = condition.sql_text();
    if (!condition_text.empty())
    {
      sql_text += "WHERE ";
      sql_text += condition_text + ";";
    }
    sqlite3_stmt* statements = NULL;
    int prepare_result = sqlite3_prepare_v2(database.sqlite_db(),
                                            sql_text.c_str(),
                                            -1, &statements, NULL);
    if (prepare_result != SQLITE_OK) return -1;
    int step_result = sqlite3_step(statements);
    if (step_result != SQLITE_DONE)
      return -1;
    else
      return 0;
  }

  static int GetAllResource(Database& database, RecordContainer& records)
  {
    records.clear();
    SelectMask select_mask;
    select_mask.set();
    Condition condition;
    SelectedRecordContainer selected_records;
    int result = SelectResource(database, select_mask, condition,
                                selected_records);
    if (result != 0) return result;
    auto itr_selected_record = selected_records.begin();
    auto itr_selected_record_end = selected_records.end();
    for (; itr_selected_record != itr_selected_record_end;
         ++itr_selected_record)
    {
      if (itr_selected_record->second.size() !=
          size_t(Resource::number_of_fields_))
      {
        return -1;
      }
      auto itr_value = itr_selected_record->second.begin();
      auto itr_value_end = itr_selected_record->second.end();
      Record record;
      for (int i = 0; itr_value != itr_value_end; ++itr_value, ++i)
      {
        record[i] = itr_value->second;
      }
      records[itr_selected_record->first] = record;
    }

    return 0;
  }

  static int GetResourceById(Database& database, Identifier id, Record& record)
  {
    SelectMask select_mask;
    select_mask.set();
    SelectedRecordContainer selected_records;
    SelectResource(database, select_mask,
                   EqualTo(Resource::fields_[0], Value(int(id))),
                   selected_records);
    if (selected_records.size() != 1)
    {
      return -1;
    }
    else
    {
      auto itr_value = selected_records.begin()->second.begin();
      auto itr_value_end = selected_records.begin()->second.end();
      for (int i = 0; itr_value != itr_value_end; ++itr_value, ++i)
      {
        record[i] = itr_value->second;
      }
      return 0;
    }
  }

};

#define DECLARE_RESOURCE_COMMON_MEMBERS(ResourceName) \
public:\
  static Field fields_[number_of_fields_];\
  static const char* resource_name_;\
\
  static ResourceName* RegisterToDatabase(Database& database);\
  static bool IsResourceExist(Database& database);\
\
  typedef ResourceUtility<ResourceName> Utility;\
  friend Utility;\
\
  typedef Utility::Record Record;\
  typedef Utility::RecordContainer RecordContainer;\
\
  typedef Utility::AddRequest AddRequest;\
  typedef Utility::AddRequestContainer AddRequestContainer;\
  typedef Utility::AddedRecord AddedRecord;\
  typedef Utility::AddedRecordContainer AddedRecordContainer;\
\
  typedef Utility::SelectMask SelectMask;\
  typedef Utility::SelectedRecord SelectedRecord;\
  typedef Utility::SelectedRecordContainer SelectedRecordContainer;\
\
  typedef Utility::UpdateRequest UpdateRequest;\
  typedef Utility::UpdateRequestContainer UpdateRequestContainer;\
  typedef Utility::UpdatedRecord UpdatedRecord;\
  typedef Utility::UpdatedRecordContainer UpdatedRecordContainer;\
\
private:\
  ResourceName(Database& database);\
\
public:\
  int Add(const AddRequestContainer& add_requests,\
          AddedRecordContainer& added_records);\
  int Select(const SelectMask& select_mask,\
             const Condition& condition,\
             SelectedRecordContainer& selected_records);\
  int Update(const UpdateRequestContainer& update_requests,\
             UpdatedRecordContainer& updated_records);\
  int Delete(const Condition& condition);\
\
  int GetAll(RecordContainer& records);\
  int GetById(Identifier id, Record& record);\
  Record operator[] (Identifier id);\
\
private:\
  Database& database_;\
\

#define DEFINE_RESOURCE_COMMON_MEMBERS(ResourceName) \
ResourceName* ResourceName::RegisterToDatabase(Database& database)\
{\
  return Utility::Register(database);\
}\
\
bool ResourceName::IsResourceExist(Database& database)\
{\
  return Utility::IsExist(database);\
}\
\
ResourceName::ResourceName(Database& database)\
  : database_(database)\
{\
}\
\
int ResourceName::Add(const AddRequestContainer& add_requests,\
                      AddedRecordContainer& added_records)\
{\
  return Utility::AddResource(database_, add_requests, added_records);\
}\
\
int ResourceName::Select(const SelectMask& select_mask,\
                         const Condition& condition,\
                         SelectedRecordContainer& selected_records)\
{\
  return Utility::SelectResource(database_, select_mask, condition,\
                                 selected_records);\
}\
\
int ResourceName::Update(const UpdateRequestContainer& update_requests,\
                         UpdatedRecordContainer& updated_records)\
{\
  return Utility::UpdateResource(database_,update_requests, updated_records);\
}\
\
int ResourceName::Delete(const Condition& condition)\
{\
  return Utility::DeleteResource(database_, condition);\
}\
\
int ResourceName::GetAll(RecordContainer& records)\
{\
  return Utility::GetAllResource(database_, records);\
}\
\
int ResourceName::GetById(Identifier id, Record& record)\
{\
  return Utility::GetResourceById(database_, id, record);\
}\
\
ResourceName::Record ResourceName::operator[] (Identifier id)\
{\
  Record record;\
  GetById(id, record);\
  return record;\
}\

}
}
}

#endif
