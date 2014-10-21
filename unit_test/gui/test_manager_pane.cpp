#include <vector>

#include <QtTest/QtTest>
#include <QMainWindow>
#include <QListWidget>
#include <QAction>
#include <QMouseEvent>

#include "manager_pane.hpp"
#include "action_tester.hpp"

namespace test
{

class TestManagerPane : public QObject
{
  Q_OBJECT
private slots:
  void TestTitle();
  void TestToolBar();
  void TestAddWidget();
  void TestOrder();
  void TestInsertWidget();
  void TestPaneResize();
  void TestSplitterResize();
  void TestRemoveWidget();
  void TestReplaceWidget();
};

void TestManagerPane::TestTitle()
{
  QString title = tr("Test Title");
  hs::recon::ManagerPane test_pane(title, NULL);
  QCOMPARE(test_pane.windowTitle(), title);
}

void TestManagerPane::TestToolBar()
{
  QString title = tr("Test Tool Bar");

  hs::recon::ManagerPane test_pane(title, NULL);

  QToolBar* tool_bar = new QToolBar(0);
  QAction* action = new QAction(tr("Test Manager Pane Tool Bar"), tool_bar);
  tool_bar->addAction(action);
  test_pane.SetToolBar(tool_bar);

  QCOMPARE(tool_bar->parent(), test_pane.widget());
}

void TestManagerPane::TestAddWidget()
{
  QString title = tr("Test Add Widget");
  hs::recon::ManagerPane test_pane(title, NULL);

  int widgets_count = 5;
  std::vector<QWidget> widgets(widgets_count);
  for (int i = 0; i < widgets_count; i++)
  {
    test_pane.AddWidget(&(widgets[i]));
    QCOMPARE(test_pane.GetWidget(i), &(widgets[i]));
  }
  QCOMPARE(test_pane.WidgetsCount(), widgets_count);

  test_pane.AddWidget(&(widgets[0]));
  QCOMPARE(test_pane.GetWidget(0), &(widgets[0]));
  QCOMPARE(test_pane.WidgetsCount(), widgets_count);
}

void TestManagerPane::TestOrder()
{
  QMainWindow main_window;
  main_window.resize(800, 600);

  QString title = tr("Test Order");
  hs::recon::ManagerPane test_pane(title, &main_window);

  main_window.show();

  main_window.addDockWidget(Qt::RightDockWidgetArea, &test_pane);
  QCOMPARE(test_pane.WidgetsOrientation(), Qt::Vertical);

  main_window.addDockWidget(Qt::LeftDockWidgetArea, &test_pane);
  QCOMPARE(test_pane.WidgetsOrientation(), Qt::Vertical);

  main_window.addDockWidget(Qt::TopDockWidgetArea, &test_pane);
  QCOMPARE(test_pane.WidgetsOrientation(), Qt::Horizontal);

  main_window.addDockWidget(Qt::BottomDockWidgetArea, &test_pane);
  QCOMPARE(test_pane.WidgetsOrientation(), Qt::Horizontal);

}

void TestManagerPane::TestInsertWidget()
{
  QString title = tr("Test Insert Widget");
  hs::recon::ManagerPane test_pane(title, NULL);

  int widgets_count = 4;
  std::vector<QWidget> widgets(widgets_count);
  test_pane.AddWidget(&(widgets[0]));
  test_pane.AddWidget(&(widgets[1]));
  test_pane.AddWidget(&(widgets[3]));
  test_pane.InsertWidget(2, &(widgets[2]));
  QCOMPARE(test_pane.GetWidget(2), &(widgets[2]));
}

void TestManagerPane::TestPaneResize()
{
  QMainWindow main_window;

  QString title = tr("Test Pane Resize");
  hs::recon::ManagerPane test_pane(title, &main_window);

  main_window.addDockWidget(Qt::RightDockWidgetArea, &test_pane);

  QWidget widget_0;
  QWidget widget_1;
  test_pane.AddWidget(&widget_0);
  test_pane.AddWidget(&widget_1);

  main_window.show();

  QSize pane_size = test_pane.size();
  test_pane.resize(pane_size.width() / 2, pane_size.height());
  pane_size = test_pane.size();

  QCOMPARE(widget_0.size().width(), pane_size.width());
  QCOMPARE(widget_1.size().width(), pane_size.width());

}

void TestManagerPane::TestSplitterResize()
{
  QMainWindow main_window;
  main_window.resize(800, 600);

  QWidget* central_widget = new QWidget(&main_window);
  main_window.setCentralWidget(central_widget);

  QString title = tr("Test Splitter Resize");
  hs::recon::ManagerPane test_pane(title, &main_window);

  main_window.addDockWidget(Qt::RightDockWidgetArea, &test_pane);

  QListWidget widget_0;
  QListWidget widget_1;
  test_pane.AddWidget(&widget_0);
  test_pane.AddWidget(&widget_1);

  main_window.show();

  QWidget* pane_widget = test_pane.widget();
  QSize pane_widget_size = pane_widget->size();
  qApp->processEvents();
  QTest::mouseMove(pane_widget, QPoint(pane_widget_size.width() / 2,
                                       pane_widget_size.height() / 2), 500);
  qApp->processEvents();
  QTest::qWait(500);
  QTest::mousePress(pane_widget, Qt::LeftButton, 0,
                    QPoint(pane_widget_size.width() / 2,
                           pane_widget_size.height() / 2), 500);
  qApp->processEvents();
  QTest::qWait(500);
  QTest::mouseMove(pane_widget, QPoint(pane_widget_size.width() / 2,
                                       pane_widget_size.height() / 4), 500);
  qApp->processEvents();
  QTest::qWait(500);
  QTest::mouseRelease(pane_widget, Qt::LeftButton, 0,
                      QPoint(pane_widget_size.width() / 2,
                             pane_widget_size.height() / 4), 500);
  qApp->processEvents();
  QTest::qWait(500);

  //QTest::qWait(1000);
  //QPoint half_point(pane_widget_size.width() / 2,
  //                  pane_widget_size.height() / 2);
  //QPoint quarder_point(pane_widget_size.width() / 2,
  //                     pane_widget_size.height() / 4);
  //QMouseEvent* press_event =
  //  new QMouseEvent(QEvent::MouseButtonPress,
  //                  half_point, pane_widget->mapToGlobal(half_point),
  //                  Qt::LeftButton, 0, 0);
  //QApplication::postEvent(pane_widget, press_event);
  //qApp->processEvents();
  //QTest::qWait(250);

  //QMouseEvent* move_event =
  //  new QMouseEvent(QEvent::MouseMove,
  //                  quarder_point,
  //                  pane_widget->mapToGlobal(quarder_point),
  //                  Qt::NoButton, 0, 0);
  //QApplication::postEvent(pane_widget, move_event);
  //qApp->processEvents();
  //QTest::qWait(250);

  //qApp->exec();

  QSize widget_size_0 = widget_0.size();
  QSize widget_size_1 = widget_1.size();
  QCOMPARE(widget_size_0.width(), pane_widget_size.width());
  QCOMPARE(widget_size_0.height(), pane_widget_size.height() / 4);
  QCOMPARE(widget_size_1.width(), pane_widget_size.width());
  QCOMPARE(widget_size_1.height(), pane_widget_size.height() / 2);
}

void TestManagerPane::TestRemoveWidget()
{
  QString title = tr("Test Remove Widget");
  hs::recon::ManagerPane test_pane(title, NULL);

  int widgets_count = 4;
  std::vector<QWidget> widgets(widgets_count);
  for (int i = 0; i < widgets_count; i++)
  {
    test_pane.AddWidget(&(widgets[i]));
  }
  test_pane.RemoveWidget(&(widgets[2]));
  QCOMPARE(test_pane.GetWidget(0), &(widgets[0]));
  QCOMPARE(test_pane.GetWidget(1), &(widgets[1]));
  QCOMPARE(test_pane.GetWidget(2), &(widgets[3]));
}

void TestManagerPane::TestReplaceWidget()
{
  QString title = tr("Test Replace Widget");
  hs::recon::ManagerPane test_pane(title, NULL);

  int widgets_count = 4;
  std::vector<QWidget> widgets(widgets_count);
  for (int i = 0; i < widgets_count - 1; i++)
  {
    test_pane.AddWidget(&(widgets[i]));
  }

  test_pane.ReplaceWidget(2, &(widgets[1]));
  QCOMPARE(test_pane.GetWidget(0), &(widgets[0]));
  QCOMPARE(test_pane.GetWidget(1), &(widgets[1]));
  QCOMPARE(test_pane.GetWidget(2), &(widgets[2]));

  test_pane.ReplaceWidget(2, &(widgets[3]));
  QCOMPARE(test_pane.GetWidget(0), &(widgets[0]));
  QCOMPARE(test_pane.GetWidget(1), &(widgets[1]));
  QCOMPARE(test_pane.GetWidget(2), &(widgets[3]));
}

}

QTEST_MAIN(test::TestManagerPane)
#include "test_manager_pane.moc"


