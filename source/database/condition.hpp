#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_CONDITION_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_CONDITION_HPP_

#include <string>

#include "database/field.hpp"

namespace hs
{
namespace recon
{
namespace db
{

class Condition
{
public:
  virtual std::string sql_text() const;
};

class EqualTo : public Condition
{
public:
  EqualTo(const Field& field, const Value& value);

  virtual std::string sql_text() const;

private:
  Field field_;
  Value value_;
};

class GreaterThan : public Condition
{
public:
  GreaterThan(const Field& field, const Value& value);

  virtual std::string sql_text() const;

private:
  Field field_;
  Value value_;
};

class LessThan : public Condition
{
public:
  LessThan(const Field& field, const Value& value);

  virtual std::string sql_text() const;

private:
  Field field_;
  Value value_;
};

class Or : public Condition
{
public:
  Or(const Condition& condition_1, const Condition& condition_2);
  virtual std::string sql_text() const;

private:
  const Condition& condition_1_;
  const Condition& condition_2_;
};

class And : public Condition
{
public:
  And(const Condition& condition_1, const Condition& condition_2);
  virtual std::string sql_text() const;

private:
  const Condition& condition_1_;
  const Condition& condition_2_;
};

}
}
}

#endif
