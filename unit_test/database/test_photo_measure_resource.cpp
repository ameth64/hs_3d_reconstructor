#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/photo_measure_resource.hpp"

namespace
{

TEST(TestPOGCPPhotoRelationResource, SimpleTest)
{
  using namespace hs::recon::db;
  typedef Database::Identifier Identifier;
  typedef Database::Float Float;

  typedef hs::test::ResourceTester<PhotoMeasureResource> Tester;
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
    Value(int(1)),
    Value(int(2)),
    Value(int(PhotoMeasureResource::TYPE_CONTROL_POINT)),
    Value(3000.34),
    Value(2000.14)
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(int(4)),
    Value(int(7)),
    Value(int(33)),
    Value(int(POGCPPhotoRelationResource::TYPE_CONTROL_POINT)),
    Value(298.35),
    Value(1222.25)
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(int(2)),
    Value(int(3)),
    Value(int(1)),
    Value(int(POGCPPhotoRelationResource::TYPE_CONTROL_POINT)),
    Value(388.64),
    Value(2980.17)
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(int(2)),
    Value(int(8)),
    Value(int(102)),
    Value(int(POGCPPhotoRelationResource::TYPE_CHECK_POINT)),
    Value(3063.35),
    Value(1117.16)
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
      "test_po_gcp_photo_relation_resource",
      add_requests,
      true_added_records));
}

}
