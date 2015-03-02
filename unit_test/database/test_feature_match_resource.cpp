#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/feature_match_resource.hpp"

namespace
{

TEST(TestFeatureMatchResource, SimpleTest)
{
  using namespace hs::recon::db;
  typedef Database::Identifier Identifier;
  typedef Database::Float Float;

  typedef hs::test::ResourceTester<FeatureMatchResource> Tester;
  typedef Tester::AddRequest AddRequest;
  typedef Tester::AddRequestContainer AddRequestContainer;
  typedef Tester::AddedRecord AddedRecord;
  typedef Tester::AddedRecordContainer AddedRecordContainer;
  typedef Tester::SelectMask SelectMask;
  typedef Tester::SelectedRecord SelectedRecord;
  typedef Tester::SelectedRecordContainer SelectedRecordContainer;
  typedef Tester::UpdateRequest UpdateRequest;
  typedef Tester::UpdateRequestContainer UpdateRequestContainer;
  typedef Tester::UpdatedRecord UpdatedRecord;
  typedef Tester::UpdatedRecordContainer UpdatedRecordContainer;
  typedef Tester::Record Record;
  typedef Tester::RecordContainer RecordContainer;

  AddRequest add_request1 =
  {
    Value(int(0)),
    Value(int(1)),
    Value(std::string("test_feature_match1")),
    Value(std::string("feature_matches/test_feature_match1")),
    Value(int(0))
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(int(2)),
    Value(std::string("test_feature_match2")),
    Value(std::string("feature_matches/test_feature_match2")),
    Value(int(1))
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(int(3)),
    Value(std::string("test_feature_match3")),
    Value(std::string("feature_matches/test_feature_match3")),
    Value(int(0))
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(int(4)),
    Value(std::string("test_feature_match4")),
    Value(std::string("feature_matches/test_feature_match4")),
    Value(int(1))
  };
  AddRequestContainer add_requests;
  add_requests.push_back(add_request1);
  add_requests.push_back(add_request2);
  add_requests.push_back(add_request3);
  add_requests.push_back(add_request4);
  AddedRecordContainer true_added_records;
  true_added_records[1] = add_request1;
  true_added_records[2] = add_request2;
  true_added_records[3] = add_request3;
  true_added_records[4] = add_request4;

  ASSERT_EQ(Tester::TEST_SUCCESS, Tester::Test(
      "test_feature_match_resource",
      add_requests,
      true_added_records));
}

}
