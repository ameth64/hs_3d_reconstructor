#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/ground_control_point_resource.hpp"

namespace
{

TEST(TestGroundControlPointResource, SimpleTest)
{
  using namespace hs::recon::db;
  typedef Database::Identifier Identifier;
  typedef Database::Float Float;

  typedef hs::test::ResourceTester<GroundControlPointResource> Tester;
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
    Value(std::string("test_gcp_1")),
    Value(std::string("test_gcp_1\nFor testing!")),
    Value(Float(526788.229)),
    Value(Float(2531671.652)),
    Value(Float(61.0161))
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(std::string("test_gcp_2")),
    Value(std::string("test_gcp_2\nFor testing!")),
    Value(Float(526912.5394)),
    Value(Float(2531728.3632)),
    Value(Float(61.7141))
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(std::string("test_gcp_3")),
    Value(std::string("test_gcp_3\nFor testing!")),
    Value(Float(526910.4446)),
    Value(Float(2531780.808)),
    Value(Float(61.1791))
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(std::string("test_gcp_4")),
    Value(std::string("test_gcp_4\nFor testing!")),
    Value(Float(526877.8012)),
    Value(Float(2531819.2687)),
    Value(Float(61.5451))
  };
  AddRequest add_request_duplicate =
  {
    Value(int(0)),
    Value(std::string("test_gcp_4")),
    Value(std::string("test_gcp_4\nFor testing!")),
    Value(Float(526877.8012)),
    Value(Float(2531819.2687)),
    Value(Float(61.5451))
  };
  AddRequestContainer add_requests;
  add_requests.push_back(add_request1);
  add_requests.push_back(add_request2);
  add_requests.push_back(add_request3);
  add_requests.push_back(add_request4);
  add_requests.push_back(add_request_duplicate);
  AddedRecordContainer true_added_records;
  true_added_records[1] = add_request1;
  true_added_records[2] = add_request2;
  true_added_records[3] = add_request3;
  true_added_records[4] = add_request4;

  ASSERT_EQ(Tester::TEST_SUCCESS, Tester::Test("test_gcp_resource.db",
                                               add_requests,
                                               true_added_records));
}

}
