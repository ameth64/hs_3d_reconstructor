#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_FIELD_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_FIELD_HPP_

#include <iostream>
#include <string>

#include <boost/any.hpp>

#include "database/database.hpp"

namespace hs
{
namespace recon
{
namespace db
{

typedef Database::Identifier Identifier;
typedef Database::Float Float;

enum FieldValueType
{
  FIELD_VALUE_TEXT = 0,
  FIELD_VALUE_INTEGER,
  FIELD_VALUE_REAL,
  FIELD_VALUE_UNKNOWN
};

enum FieldConstraintType
{
  FIELD_CONSTRAINT_KEY,
  FIELD_CONSTRAINT_UNIQUE,
  FIELD_CONSTRAINT_NONE
};

struct Field
{
  const char* name;
  FieldValueType value_type;
  FieldConstraintType constaits_type;
};


class Value
{
public:
  Value();
  Value(int int_value);
  Value(Float float_value);
  Value(const std::string& string_value);
  Value(const Value& other);
  Value& operator = (int int_value);
  Value& operator = (Float float_value);
  Value& operator = (const std::string& string_value);
  Value& operator = (const Value& other);

  bool operator == (const Value& other) const;
  bool operator != (const Value& other) const;

  FieldValueType type() const;

  int ToInt() const;
  Float ToFloat() const;
  std::string ToString() const;
  friend std::ostream& operator << (std::ostream& ostream, const Value& value);
  friend std::istream& operator >> (std::istream& istream, Value& value);
private:
  int type_;
  boost::any storage_;
};
typedef boost::any FieldValue;

std::string SqlTableField(const Field& field);

std::string SqlFieldValue(const Value& value);

}
}
}

#endif
