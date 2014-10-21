#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/photogroup_resource.hpp"

namespace
{

using namespace hs::recon::db;
typedef Database::Identifier Identifier;
typedef Database::Float Float;

TEST(TestPhotogroupResource, SimpleTest)
{
  typedef hs::test::ResourceTester<PhotogroupResource> Tester;
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
    Value(std::string("test_photogroup1")),
    Value(std::string("test_photogroup1\nFor testing")),
    Value(int(4912)),
    Value(int(3264)),
    Value(Float(0.00489089)),
    Value(Float(0.00489089)),
    Value(Float(16.150)),
    Value(Float(2442.63)),
    Value(Float(1603.72)),
    Value(Float(0)),
    Value(Float(-0.0648004)),
    Value(Float(0.0816083)),
    Value(Float(0.0113371)),
    Value(Float(-0.00102127)),
    Value(Float(-6.92298e-5))
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(std::string("test_photogroup2")),
    Value(std::string("test_photogroup2\nFor testing")),
    Value(int(7360)),
    Value(int(4912)),
    Value(Float(0.00488967)),
    Value(Float(0.00488967)),
    Value(Float(36.223653294)),
    Value(Float(3701.75)),
    Value(Float(2510.2)),
    Value(Float(0)),
    Value(Float(0.0500093)),
    Value(Float(-0.220031)),
    Value(Float(-0.00340774)),
    Value(Float(9.82655e-05)),
    Value(Float(-1.32354e-05))
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(std::string("test_photogroup3")),
    Value(std::string("test_photogroup3\nFor testing")),
    Value(int(6000)),
    Value(int(4000)),
    Value(Float(0.004)),
    Value(Float(0.004)),
    Value(Float(30.6692)),
    Value(Float(3017.3)),
    Value(Float(2005.19)),
    Value(Float(0)),
    Value(Float(-0.136838)),
    Value(Float(0.243901)),
    Value(Float(0.212523)),
    Value(Float(-0.000543599)),
    Value(Float(0.000303527))
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(std::string("test_photogroup4")),
    Value(std::string("test_photogroup4\nFor testing")),
    Value(int(5472)),
    Value(int(3648)),
    Value(Float(0.00244361)),
    Value(Float(0.00244361)),
    Value(Float(10.5046639763)),
    Value(Float(2735.68)),
    Value(Float(1845.04)),
    Value(Float(0)),
    Value(Float(0.00457927)),
    Value(Float(-0.00475212)),
    Value(Float(-0.00512842)),
    Value(Float(0.00166101)),
    Value(Float(0.000614202))
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

  ASSERT_EQ(Tester::TEST_SUCCESS, Tester::Test("test_photogroup_resource.db",
                                               add_requests,
                                               true_added_records));
}

}
