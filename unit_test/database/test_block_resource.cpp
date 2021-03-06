﻿#include <gtest/gtest.h>

#include "resource_tester.hpp"
#include "database/block_resource.hpp"

namespace
{

TEST(TestBlockResource, SimpleTest)
{
  using namespace hs::recon::db;
  typedef Database::Identifier Identifier;
  typedef Database::Float Float;

  typedef hs::test::ResourceTester<BlockResource> Tester;
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
    Value(std::string("test_block_1")),
    Value(std::string("test_block_1\nFor testing, yeah!"))
  };
  AddRequest add_request2 =
  {
    Value(int(0)),
    Value(std::string("test_block_2")),
    Value(std::string("test_block_2\nFor testing, yeah!"))
  };
  AddRequest add_request3 =
  {
    Value(int(0)),
    Value(std::string("test_block_3")),
    Value(std::string("test_block_3\nFor testing, yeah!"))
  };
  AddRequest add_request4 =
  {
    Value(int(0)),
    Value(std::string("test_block_4")),
    Value(std::string("test_block_4\nFor testing, yeah!"))
  };
  AddRequest add_request_duplicate =
  {
    Value(int(0)),
    Value(std::string("test_block_4")),
    Value(std::string("test_block_4\nFor testing, yeah!"))
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

  ASSERT_EQ(Tester::TEST_SUCCESS, Tester::Test("test_block_resource",
                                               add_requests,
                                               true_added_records));
}

}
