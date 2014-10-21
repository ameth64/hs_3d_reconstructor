#include <iostream>

#include <gtest/gtest.h>

#include "database/condition.hpp"

namespace
{

TEST(TestCondition, SimpleTest)
{
  using namespace hs::recon::db;
  Field fields[3] =
  {
    {"ID", FIELD_VALUE_INTEGER, FIELD_CONSTRAINT_KEY},
    {"NAME", FIELD_VALUE_TEXT, FIELD_CONSTRAINT_UNIQUE},
    {"RATIO", FIELD_VALUE_REAL, FIELD_CONSTRAINT_NONE}
  };

  std::string expected_sql_text =
    "((ID < 9) AND NAME = 'TEST') OR RATIO > 0.5";
  ASSERT_EQ(expected_sql_text,
            Or(And(LessThan(fields[0], Value(int(9))),
                   EqualTo(fields[1], Value(std::string("TEST")))),
               GreaterThan(fields[2], Value(Float(0.5)))).sql_text());
}

}
