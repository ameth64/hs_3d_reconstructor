#include "action_tester.hpp"

namespace test
{

ActionTester::ActionTester()
  : QObject(NULL), triggered_(false), hovered_(false)
{
}

void ActionTester::ResetTrigger()
{
  triggered_ = false;
}

void ActionTester::ResetHover()
{
  hovered_ = false;
}

bool ActionTester::IsTriggered()
{
  return triggered_;
}

bool ActionTester::IsHovered()
{
  return hovered_;
}

void ActionTester::OnActionTriggered()
{
  triggered_ = true;
}

void ActionTester::OnActionHovered()
{
  hovered_ = true;
}

}
