#ifndef _HS_3D_RECONSTRUCTOR_UNIT_TEST_ACTION_TESTER_HPP_
#define _HS_3D_RECONSTRUCTOR_UNIT_TEST_ACTION_TESTER_HPP_

#include <QObject>

namespace test
{

class ActionTester : public QObject
{
  Q_OBJECT
public:
  ActionTester();
  void ResetTrigger();
  void ResetHover();
  bool IsTriggered();
  bool IsHovered();
public slots:
  void OnActionTriggered();
  void OnActionHovered();

private:
  bool triggered_;
  bool hovered_;
};

}

#endif
