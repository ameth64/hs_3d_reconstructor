#include <QtTest/QtTest>
#include <QApplication>

#include "opengl_window.hpp"

namespace test
{

class OpenGLWindowTester : public hs::recon::OpenGLWindow
{
  Q_OBJECT
public:
  OpenGLWindowTester(QWindow* parent = 0);

  uint rendered_count() const;
  Qt::MouseButton start_mouse_dragging_button() const;
  QPoint start_mouse_dragging_point() const;
  Qt::MouseButton mouse_dragging_button() const;
  QPoint mouse_dragging_offset() const;
  Qt::MouseButton end_mouse_dragging_button() const;
  QPoint end_mouse_dragging_point() const;
  Qt::MouseButton mouse_click_button() const;
  QPoint mouse_click_point() const;
  Qt::MouseButton mouse_double_click_button() const;
  QPoint mouse_double_click_point() const;

public slots:
  void OnStartMouseDragging(Qt::MouseButton mouse_button, QPoint pos);
  void OnMouseDragging(Qt::MouseButton mouse_button, QPoint offset);
  void OnEndMouseDragging(Qt::MouseButton mouse_button, QPoint pos);
  void OnMouseClicked(Qt::MouseButton mouse_button, QPoint pos);
  void OnMouseDoubleClicked(Qt::MouseButton mouse_button, QPoint pos);

protected:
  virtual void Render();

private:
  uint rendered_count_;
  Qt::MouseButton start_mouse_dragging_button_;
  QPoint start_mouse_dragging_point_;
  Qt::MouseButton mouse_dragging_button_;
  QPoint mouse_dragging_offset_;
  Qt::MouseButton end_mouse_dragging_button_;
  QPoint end_mouse_dragging_point_;
  Qt::MouseButton mouse_click_button_;
  QPoint mouse_click_point_;
  Qt::MouseButton mouse_double_click_button_;
  QPoint mouse_double_click_point_;
};

OpenGLWindowTester::OpenGLWindowTester(QWindow* parent)
  : hs::recon::OpenGLWindow(parent),
    rendered_count_(0),
    start_mouse_dragging_button_(Qt::NoButton),
    mouse_dragging_button_(Qt::NoButton),
    end_mouse_dragging_button_(Qt::NoButton),
    mouse_click_button_(Qt::NoButton),
    mouse_double_click_button_(Qt::NoButton)
{
  QObject::connect(this, &OpenGLWindow::StartMouseDragging,
                   this, &OpenGLWindowTester::OnStartMouseDragging);
  QObject::connect(this, &OpenGLWindow::MouseDragging,
                   this, &OpenGLWindowTester::OnMouseDragging);
  QObject::connect(this, &OpenGLWindow::EndMouseDragging,
                   this, &OpenGLWindowTester::OnEndMouseDragging);
  QObject::connect(this, &OpenGLWindow::MouseClicked,
                   this, &OpenGLWindowTester::OnMouseClicked);
  QObject::connect(this, &OpenGLWindow::MouseDoubleClicked,
                   this, &OpenGLWindowTester::OnMouseDoubleClicked);
}

uint OpenGLWindowTester::rendered_count() const
{
  return rendered_count_;
}

Qt::MouseButton OpenGLWindowTester::start_mouse_dragging_button() const
{
  return start_mouse_dragging_button_;
}
QPoint OpenGLWindowTester::start_mouse_dragging_point() const
{
  return start_mouse_dragging_point_;
}
Qt::MouseButton OpenGLWindowTester::mouse_dragging_button() const
{
  return mouse_dragging_button_;
}
QPoint OpenGLWindowTester::mouse_dragging_offset() const
{
  return mouse_dragging_offset_;
}
Qt::MouseButton OpenGLWindowTester::end_mouse_dragging_button() const
{
  return end_mouse_dragging_button_;
}
QPoint OpenGLWindowTester::end_mouse_dragging_point() const
{
  return end_mouse_dragging_point_;
}
Qt::MouseButton OpenGLWindowTester::mouse_click_button() const
{
  return mouse_click_button_;
}
QPoint OpenGLWindowTester::mouse_click_point() const
{
  return mouse_click_point_;
}
Qt::MouseButton OpenGLWindowTester::mouse_double_click_button() const
{
  return mouse_double_click_button_;
}
QPoint OpenGLWindowTester::mouse_double_click_point() const
{
  return mouse_double_click_point_;
}

void OpenGLWindowTester::OnStartMouseDragging(Qt::MouseButton mouse_button,
                                              QPoint pos)
{
  start_mouse_dragging_button_ = mouse_button;
  start_mouse_dragging_point_ = pos;
}

void OpenGLWindowTester::OnMouseDragging(Qt::MouseButton mouse_button,
                                         QPoint offset)
{
  mouse_dragging_button_ = mouse_button;
  mouse_dragging_offset_ = offset;
}

void OpenGLWindowTester::OnEndMouseDragging(Qt::MouseButton mouse_button,
                                            QPoint pos)
{
  end_mouse_dragging_button_ = mouse_button;
  end_mouse_dragging_point_ = pos;
}

void OpenGLWindowTester::OnMouseClicked(Qt::MouseButton mouse_button,
                                        QPoint pos)
{
  mouse_click_button_ = mouse_button;
  mouse_click_point_ = pos;
}

void OpenGLWindowTester::OnMouseDoubleClicked(Qt::MouseButton mouse_button,
                                              QPoint pos)
{
  mouse_double_click_button_ = mouse_button;
  mouse_double_click_point_ = pos;
}

void OpenGLWindowTester::Render()
{
  rendered_count_++;
}

class TestOpenGLWindow : public QObject
{
  Q_OBJECT
private slots:
  void TestRender();
  void TestAnimating();
  void TestMouse();
};

void TestOpenGLWindow::TestRender()
{
  OpenGLWindowTester tester;
  tester.SetAnimating(false);
  tester.show();

  QTest::qWait(1000);

  QCOMPARE(uint(1), tester.rendered_count());
}

void TestOpenGLWindow::TestAnimating()
{
  OpenGLWindowTester tester;
  tester.SetAnimating(true);
  tester.show();

  QTest::qWait(1000);

  QCOMPARE(true, tester.rendered_count() > 100);
}

void TestOpenGLWindow::TestMouse()
{
  OpenGLWindowTester tester;
  int width = 800;
  int height = 600;
  tester.resize(width, height);
  tester.show();

  QPoint start_mouse_dragging_point(400, 300);
  Qt::MouseButton mouse_dragging_button = Qt::LeftButton;
  QTest::mousePress(&tester, mouse_dragging_button, 0,
                    start_mouse_dragging_point);
  QPoint mouse_dragging_point(500, 400);
  QTest::mouseMove(&tester, mouse_dragging_point);
  QTest::mouseRelease(&tester, mouse_dragging_button, 0,
                      mouse_dragging_point);

  QPoint mouse_click_point(300, 200);
  Qt::MouseButton mouse_click_button = Qt::RightButton;
  QTest::mouseClick(&tester, mouse_click_button, 0,
                    mouse_click_point);

  QPoint mouse_double_click_point(200, 100);
  Qt::MouseButton mouse_double_click_button = Qt::MiddleButton;
  QTest::mouseDClick(&tester, mouse_double_click_button, 0,
                     mouse_double_click_point);

  start_mouse_dragging_point.setY(height - start_mouse_dragging_point.y() - 1);
  mouse_dragging_point.setY(height - mouse_dragging_point.y() - 1);
  QPoint mouse_dragging_offset(mouse_dragging_point.x() -
                               start_mouse_dragging_point.x(),
                               mouse_dragging_point.y() -
                               start_mouse_dragging_point.y());
  mouse_click_point.setY(height - mouse_click_point.y() - 1);
  mouse_double_click_point.setY(height - mouse_double_click_point.y() - 1);

  QCOMPARE(start_mouse_dragging_point, tester.start_mouse_dragging_point());
  QCOMPARE(mouse_dragging_button, tester.start_mouse_dragging_button());
  QCOMPARE(mouse_dragging_offset, tester.mouse_dragging_offset());
  QCOMPARE(mouse_dragging_button, tester.mouse_dragging_button());
  QCOMPARE(mouse_dragging_point, tester.end_mouse_dragging_point());
  QCOMPARE(mouse_dragging_button, tester.end_mouse_dragging_button());
  QCOMPARE(mouse_click_point, tester.mouse_click_point());
  QCOMPARE(mouse_click_button, tester.mouse_click_button());
  QCOMPARE(mouse_double_click_point, tester.mouse_double_click_point());
  QCOMPARE(mouse_double_click_button, tester.mouse_double_click_button());
}

}

QTEST_MAIN(test::TestOpenGLWindow)
#include "test_opengl_window.moc"
