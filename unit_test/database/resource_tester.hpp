#ifndef _HS_3D_RECONSTRUCTOR_UNIT_TEST_RESOURCE_TESTER_HPP_
#define _HS_3D_RECONSTRUCTOR_UNIT_TEST_RESOURCE_TESTER_HPP_

#include <iostream>
#include <iomanip>
#include <random>

#include <boost/filesystem.hpp>

#include "database/database.hpp"
#include "database/field.hpp"
#include "database/condition.hpp"

namespace hs
{
namespace test
{

using namespace hs::recon::db;

template <typename Resource_>
class ResourceTester
{
public:
  typedef Resource_ Resource;
  typedef typename Resource::AddRequest AddRequest;
  typedef typename Resource::AddRequestContainer AddRequestContainer;
  typedef typename Resource::AddedRecord AddedRecord;
  typedef typename Resource::AddedRecordContainer AddedRecordContainer;
  typedef typename Resource::SelectMask SelectMask;
  typedef typename Resource::SelectedRecord SelectedRecord;
  typedef typename Resource::SelectedRecordContainer SelectedRecordContainer;
  typedef typename Resource::UpdateRequest UpdateRequest;
  typedef typename Resource::UpdateRequestContainer UpdateRequestContainer;
  typedef typename Resource::UpdatedRecord UpdatedRecord;
  typedef typename Resource::UpdatedRecordContainer UpdatedRecordContainer;
  typedef typename Resource::Record Record;
  typedef typename Resource::RecordContainer RecordContainer;

  enum TestResult
  {
    TEST_SUCCESS = 0,
    REGISTER_FAILED,
    ADD_FAILED,
    UPDATE_FAILED,
    DELETE_FAILED
  };

  static int Test(const std::string& test_db_directory,
                  const AddRequestContainer& add_requests,
                  const RecordContainer& true_added_records)
  {
    boost::filesystem::path test_db_path(test_db_directory);
    if (boost::filesystem::exists(test_db_path))
    {
      boost::filesystem::remove_all(test_db_path);
    }
    Database database;
    database.Create(test_db_directory);
    Resource* resource = Resource::RegisterToDatabase(database);
    Resource* resource_again = Resource::RegisterToDatabase(database);
    if (resource == nullptr) return REGISTER_FAILED;
    if (resource_again == nullptr) return REGISTER_FAILED;

    int result = TEST_SUCCESS;
    while (1)
    {
      AddedRecordContainer added_records;
      if (resource->Add(add_requests, added_records) != 0)
      {
        result = ADD_FAILED;
        break;
      }

      if (added_records.size() != true_added_records.size())
      {
        result = ADD_FAILED;
        break;
      }
      auto itr_added_record = added_records.begin();
      auto itr_added_record_end = added_records.end();
      auto itr_true_added_record = true_added_records.begin();
      bool success = true;
      for (; itr_added_record != itr_added_record_end;
           ++itr_added_record, ++itr_true_added_record)
      {
        if (!CompareFullRecord(itr_added_record->second,
                               itr_true_added_record->second))
        {
          success = false;
          break;
        }
      }
      if (!success)
      {
        result = ADD_FAILED;
        break;
      }
      RecordContainer all_records;
      if (resource->GetAll(all_records) != 0)
      {
        result = ADD_FAILED;
        break;
      }
      auto itr_all_record = all_records.begin();
      auto itr_all_record_end = all_records.end();
      itr_true_added_record = true_added_records.begin();
      for (; itr_all_record != itr_all_record_end;
           ++itr_all_record, ++itr_true_added_record)
      {
        if (!CompareFullRecord(itr_all_record->second,
                               itr_true_added_record->second))
        {
          success = false;
          break;
        }
      }
      if (!success)
      {
        result = ADD_FAILED;
        break;
      }

      //随机抽取两条记录，使用Update交换该两条记录中非唯一的数据
      size_t random_1, random_2;
      std::random_device rd;
      std::default_random_engine e1(rd());
      std::uniform_int_distribution<size_t>
        distribution(0, added_records.size() - 1);
      random_1 = distribution(rd);
      random_2 = distribution(rd);
      UpdateRequestContainer update_requests;
      RecordContainer expected_updated_records = added_records;
      auto itr_expected_updated_record = expected_updated_records.begin();
      auto itr_expected_updated_record_end = expected_updated_records.end();
      auto itr_random_record1 = expected_updated_records.end();
      auto itr_random_record2 = expected_updated_records.end();
      for (size_t i = 0;
           itr_expected_updated_record != itr_expected_updated_record_end;
           ++itr_expected_updated_record, i++)
      {
        if (random_1 == i)
        {
          itr_random_record1 = itr_expected_updated_record;
        }
        if (random_2 == i)
        {
          itr_random_record2 = itr_expected_updated_record;
        }
      }
      UpdateRequest update_request1;
      UpdateRequest update_request2;
      for (int i = 1; i < Resource::number_of_fields_; ++i)
      {
        if (Resource::fields_[i].constaits_type != FIELD_CONSTRAINT_UNIQUE)
        {
          update_request1[i] = itr_random_record2->second[i];
          update_request2[i] = itr_random_record1->second[i];
          std::swap(itr_random_record1->second[i],
                    itr_random_record2->second[i]);
        }
      }
      update_requests[itr_random_record1->first] = update_request1;
      update_requests[itr_random_record2->first] = update_request2;
      UpdatedRecordContainer updated_records;
      if (resource->Update(update_requests, updated_records) != 0)
      {
        result = UPDATE_FAILED;
        break;
      }
      if (updated_records != update_requests)
      {
        result = UPDATE_FAILED;
        break;
      }
      RecordContainer updated_all_records;
      if (resource->GetAll(updated_all_records) != 0)
      {
        result = UPDATE_FAILED;
        break;
      }
      if (updated_all_records != expected_updated_records)
      {
        result = UPDATE_FAILED;
        break;
      }

      //随机删除一条记录
      distribution.param(
        std::uniform_int_distribution<size_t>::param_type(
          0, updated_all_records.size() - 1));
      size_t random_delete = distribution(rd);
      RecordContainer expected_deleted_records = expected_updated_records;
      auto itr_expected_deleted_record = expected_deleted_records.begin();
      auto itr_expected_deleted_record_end = expected_deleted_records.end();
      for (int i = 0;
           itr_expected_deleted_record != itr_expected_deleted_record_end;
           ++itr_expected_deleted_record, ++i)
      {
        if (i == random_delete)
        {
          Record record = (*resource)[itr_expected_deleted_record->first];
          if (record != itr_expected_deleted_record->second)
          {
            success = false;
            break;
          }
          resource->Delete(
            EqualTo(Resource::fields_[0],
                    Value(int(itr_expected_deleted_record->first))));
          expected_deleted_records.erase(itr_expected_deleted_record);
          break;
        }
      }
      if (!success)
      {
        result = DELETE_FAILED;
        break;
      }
      RecordContainer deleted_all_records;
      if (resource->GetAll(deleted_all_records) != 0)
      {
        result = DELETE_FAILED;
        break;
      }
      if (deleted_all_records != expected_deleted_records)
      {
        result = DELETE_FAILED;
        break;
      }

      break;
    }

    delete resource;
    return result;
  }

private:
  static bool CompareFullRecord(const Record& record1, const Record& record2)
  {
    for (int i = 1; i < Resource::number_of_fields_; i++)
    {
      if (record1[i] != record2[i])
      {

        std::cout.setf(std::ios::fixed);
        std::cout<<std::setprecision(8);
        std::cout<<"value1:"<<record1[i]<<"\n";
        std::cout<<"value2:"<<record2[i]<<"\n";
        return false;
      }
    }
    return true;
  }

  static bool ComparePatialRecord(const SelectedRecord& record1,
                                  const SelectedRecord& record2)
  {
    if (record1.size() != record2.size()) return false;
    bool result = true;
    for (int i = 1; i < Resource::number_of_fields_; i++)
    {
      auto itr_value1 = record1.find(i);
      auto itr_value2 = record2.find(i);
      if ((itr_value1 == record1.end() && itr_value2 != record2.end()) ||
          (itr_value1 != record1.end() && itr_value2 == record2.end()))
      {
        return false;
      }

      if (itr_value1 == record1.end ||
          itr_value1->second != itr_value2->second)
      {
        return false;
      }
    }
    return true;
  }

};

}
}

#endif
