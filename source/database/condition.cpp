#include "database/condition.hpp"

#include <iostream>

#include <boost/format.hpp>

namespace hs
{
namespace recon
{
namespace db
{

std::string Condition::sql_text() const
{
  return std::string();
}

EqualTo::EqualTo(const Field& field, const Value& value)
  : field_(field), value_(value)
{
}

std::string EqualTo::sql_text() const
{
  std::string value_text = SqlFieldValue(value_);
  std::string text =
    boost::str(
      boost::format("%1% = %2%") %
      field_.name %
      value_text);
  return text;
}

GreaterThan::GreaterThan(const Field& field, const Value& value)
  : field_(field), value_(value)
{
}

std::string GreaterThan::sql_text() const
{
  std::string value_text = SqlFieldValue(value_);
  std::string text =
    boost::str(
      boost::format("%1% > %2%") %
      field_.name %
      value_text);
  return text;
}

LessThan::LessThan(const Field& field, const Value& value)
  : field_(field), value_(value)
{
}

std::string LessThan::sql_text() const
{
  std::string value_text = SqlFieldValue(value_);
  std::string text =
    boost::str(
      boost::format("%1% < %2%") %
      field_.name %
      value_text);
  return text;
}

Or::Or(const Condition& condition_1, const Condition& condition_2)
  : condition_1_(condition_1), condition_2_(condition_2)
{
}

std::string Or::sql_text() const
{
  std::string text = condition_1_.sql_text();
  text.insert(0, "(");
  text += ") OR ";
  text += condition_2_.sql_text();
  return text;
}

And::And(const Condition& condition_1, const Condition& condition_2)
  : condition_1_(condition_1), condition_2_(condition_2)
{
}

std::string And::sql_text() const
{
  std::string text = condition_1_.sql_text();
  text.insert(0, "(");
  text += ") AND ";
  text += condition_2_.sql_text();
  return text;
}

}
}
}
