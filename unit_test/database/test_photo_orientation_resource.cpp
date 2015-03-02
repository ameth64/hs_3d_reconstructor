#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/photo_orientation_resource.hpp"

namespace
{

TEST(TestPhotoOrientationResource, SimpleTest)
{
  using namespace hs::recon::db;
  typedef Database::Identifier Identifier;
  typedef Database::Float Float;

  typedef hs::test::ResourceTester<PhotoOrientationResource> Tester;
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
    Value(std::string("test_photo_orientation1")),
    Value(std::string("photo_orientations/test_photo_orientation1")),
    Value(int(PhotoOrientationResource::FLAG_NOT_COMPLETED)),
    Value(Float(1.2)),
    Value(Float(0.8)),
    Value(Float(0.3)),
    Value(Float(0.4)),
    Value(Float(2445000)),
    Value(Float(500389)),
    Value(Float(503))
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(int(2)),
    Value(std::string("test_photo_orientation2")),
    Value(std::string("photo_orientations/test_photo_orientation2")),
    Value(int(PhotoOrientationResource::FLAG_NOT_COMPLETED)),
    Value(Float(1.24)),
    Value(Float(0.85)),
    Value(Float(0.23)),
    Value(Float(0.43)),
    Value(Float(2478201)),
    Value(Float(503379)),
    Value(Float(593))
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(int(3)),
    Value(std::string("test_photo_orientation3")),
    Value(std::string("photo_orientations/test_photo_orientation3")),
    Value(int(PhotoOrientationResource::FLAG_NOT_COMPLETED)),
    Value(Float(1.13)),
    Value(Float(0.89)),
    Value(Float(0.32)),
    Value(Float(0.44)),
    Value(Float(2445274)),
    Value(Float(502874)),
    Value(Float(529))
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(int(4)),
    Value(std::string("test_photo_orientation4")),
    Value(std::string("photo_orientations/test_photo_orientation4")),
    Value(int(PhotoOrientationResource::FLAG_COMPLETED)),
    Value(Float(1.2)),
    Value(Float(0.78)),
    Value(Float(0.20)),
    Value(Float(0.51)),
    Value(Float(2445871)),
    Value(Float(500290)),
    Value(Float(524))
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
      "test_photo_orientation_resource",
      add_requests,
      true_added_records));
}

}
