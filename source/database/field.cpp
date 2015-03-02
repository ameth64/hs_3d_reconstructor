#include "database/field.hpp"

#include <limits>
#include <cmath>

#include <boost/format.hpp>

namespace hs
{
namespace recon
{
namespace db
{

Value::Value()
  : type_(FIELD_VALUE_UNKNOWN), storage_()
{
}

Value::Value(int int_value)
  : type_(FIELD_VALUE_INTEGER), storage_(int_value)
{
}

Value::Value(Float float_value)
  : type_(FIELD_VALUE_REAL), storage_(float_value)
{
}

Value::Value(const std::string& string_value)
  : type_(FIELD_VALUE_TEXT), storage_(string_value)
{
}

Value::Value(const Value& other)
  : type_(other.type_), storage_(other.storage_)
{
}

Value& Value::operator = (int int_value)
{
  type_ = FIELD_VALUE_INTEGER;
  storage_ = int_value;
  return *this;
}

Value& Value::operator = (Float float_value)
{
  type_ = FIELD_VALUE_REAL;
  storage_ = float_value;
  return *this;
}

Value& Value::operator = (const std::string& string_value)
{
  type_ = FIELD_VALUE_TEXT;
  storage_ = string_value;
  return *this;
}

Value& Value::operator = (const Value& other)
{
  type_ = other.type_;
  storage_ = other.storage_;
  return *this;
}

bool Value::operator == (const Value& other) const
{
  if (type() != other.type())
  {
    return false;
  }

  bool result = true;
  switch (type_)
  {
  case FIELD_VALUE_INTEGER:
    {
      result = ToInt() == other.ToInt();
      break;
    }
  case FIELD_VALUE_REAL:
    {
      Float real_1 = ToFloat();
      Float real_2 = other.ToFloat();
      result = real_1 == 0 ? std::abs(real_2) < Float(1e-4) :
               (std::abs(real_1 - real_2) / real_1) < Float(1e-4);
      break;
    }
  case FIELD_VALUE_TEXT:
    {
      result = ToString() == other.ToString();
      break;
    }
  case FIELD_VALUE_UNKNOWN:
    {
      result = false;
      break;
    }
  }
  return result;
}

bool Value::operator != (const Value& other) const
{
  return !((*this) == other);
}

FieldValueType Value::type() const
{
  return FieldValueType(type_);
}


int Value::ToInt() const
{
  if (type_ != FIELD_VALUE_INTEGER)
  {
    return std::numeric_limits<int>::max();
  }
  else
  {
    return boost::any_cast<int>(storage_);
  }
}

Float Value::ToFloat() const
{
  if (type_ != FIELD_VALUE_REAL)
  {
    return std::numeric_limits<Float>::quiet_NaN();
  }
  else
  {
    return boost::any_cast<Float>(storage_);
  }
}

std::string Value::ToString() const
{
  if (type_ != FIELD_VALUE_TEXT)
  {
    return std::string();
  }
  else
  {
    return boost::any_cast<std::string>(storage_);
  }
}

std::ostream& operator << (std::ostream& ostream, const Value& value)
{
  if (value.type() == FIELD_VALUE_TEXT)
  {
    ostream << value.ToString();
  }
  else if (value.type() == FIELD_VALUE_INTEGER)
  {
    ostream << value.ToInt();
  }
  else if (value.type() == FIELD_VALUE_REAL)
  {
    ostream << value.ToFloat();
  }
  return ostream;
}

std::istream& operator >> (std::istream& istream, Value& value)
{
  if (value.type() == FIELD_VALUE_TEXT)
  {
    std::string string_value;
    istream >> string_value;
    value = string_value;
  }
  else if (value.type() == FIELD_VALUE_INTEGER)
  {
    int int_value;
    istream >> int_value;
    value = int_value;
  }
  else if (value.type() == FIELD_VALUE_REAL)
  {
    Float float_value;
    istream >> float_value;
    value = float_value;
  }
  return istream;
}

std::string SqlTableField(const Field& field)
{
  std::string sql_text = field.name;
  switch(field.value_type)
  {
  case FIELD_VALUE_TEXT:
    {
      sql_text += " TEXT";
      break;
    }
  case FIELD_VALUE_INTEGER:
    {
      sql_text += " INTEGER";
      break;
    }
  case FIELD_VALUE_REAL:
    {
      sql_text += " REAL";
      break;
    }
  case FIELD_VALUE_UNKNOWN:
    {
      return std::string();
    }
  }
  switch(field.constaits_type)
  {
  case FIELD_CONSTRAINT_KEY:
    {
      sql_text += " PRIMARY KEY NOT NULL";
      break;
    }
  case FIELD_CONSTRAINT_UNIQUE:
    {
      sql_text += " UNIQUE NOT NULL";
      break;
    }
  case FIELD_CONSTRAINT_NONE:
    {
      break;
    }
  }
  return sql_text;
}

std::string SqlFieldValue(const Value& value)
{
  std::string sql_text;
  switch (value.type())
  {
  case FIELD_VALUE_TEXT:
    {
      sql_text = boost::str(boost::format("\'%1%\'") %
                            value.ToString());
      break;
    }
  case FIELD_VALUE_INTEGER:
    {
      sql_text = boost::str(boost::format("%1%") %
                            value.ToInt());
      break;
    }
  case FIELD_VALUE_REAL:
    {
      sql_text = boost::str(boost::format("%10.10f") %
                            value.ToFloat());
      break;
    }
  case FIELD_VALUE_UNKNOWN:
    {
      break;
    }
  }

  return sql_text;
}

}
}
}
