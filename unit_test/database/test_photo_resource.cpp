#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/photo_resource.hpp"

namespace
{

TEST(TestPhotoResource, SimpleTest)
{
  using namespace hs::recon::db;
  typedef Database::Identifier Identifier;
  typedef Database::Float Float;

  typedef hs::test::ResourceTester<PhotoResource> Tester;
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
    Value(std::string("DSC00432.JPG")),
    Value(Float(108.281637)),
    Value(Float(22.848552)),
    Value(Float(151.665707)),
    Value(Float(78.983)),
    Value(Float(-106.660)),
    Value(Float(307.951))
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(int(1)),
    Value(std::string("DSC00433.JPG")),
    Value(Float(108.247397)),
    Value(Float(22.886737)),
    Value(Float(151.880472)),
    Value(Float(81.832)),
    Value(Float(-76.571)),
    Value(Float(339.042))
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(int(1)),
    Value(std::string("DSC00434.JPG")),
    Value(Float(108.187657)),
    Value(Float(22.980626)),
    Value(Float(151.539856)),
    Value(Float(-55.464)),
    Value(Float(-96.998)),
    Value(Float(329.913))
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(int(1)),
    Value(std::string("DSC00435.JPG")),
    Value(Float(108.179570)),
    Value(Float(22.008955)),
    Value(Float(151.275192)),
    Value(Float(-61.312)),
    Value(Float(-95.387)),
    Value(Float(329.820))
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

  ASSERT_EQ(Tester::TEST_SUCCESS, Tester::Test("test_photo_resource",
                                               add_requests,
                                               true_added_records));
}

}
