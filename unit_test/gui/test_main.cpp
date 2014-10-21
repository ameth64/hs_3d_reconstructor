#include <QtTest/QtTest>

#include "test_manager_pane.hpp"

int main(int argc, char** argv)
{
  test::TestManagerPane test_manager_pane;
  QTest::qExec(&test_manager_pane);

  return 0;
}
