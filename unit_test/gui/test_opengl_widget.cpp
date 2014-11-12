#include <iostream>

#include <QtTest/QtTest>
#include <QApplication>

#include "gui/opengl_widget.hpp"

namespace test
{

class OpenGLWidgetTester : public hs::recon::OpenGLWidget
{
  Q_OBJECT
private:
  OpenGLWidgetTester(const QGLFormat& format, QWidget* parent,
                     Qt::WindowFlags f);
public:
  static OpenGLWidgetTester* Create(QWidget* parent = 0,
                                    Qt::WindowFlags f = 0);

  uint rendered_count() const;
  Qt::KeyboardModifiers start_mouse_dragging_state_key() const;
  Qt::MouseButton start_mouse_dragging_button() const;
  QPoint start_mouse_dragging_point() const;
  Qt::KeyboardModifiers mouse_dragging_state_key() const;
  Qt::MouseButton mouse_dragging_button() const;
  QPoint mouse_dragging_offset() const;
  Qt::KeyboardModifiers end_mouse_dragging_state_key() const;
  Qt::MouseButton end_mouse_dragging_button() const;
  QPoint end_mouse_dragging_point() const;
  Qt::KeyboardModifiers mouse_click_state_key() const;
  Qt::MouseButton mouse_click_button() const;
  QPoint mouse_click_point() const;
  Qt::KeyboardModifiers mouse_double_click_state_key() const;
  Qt::MouseButton mouse_double_click_button() const;
  QPoint mouse_double_click_point() const;

public slots:
  void OnStartMouseDragging(Qt::KeyboardModifiers state_key,
                            Qt::MouseButton mouse_button, QPoint pos);
  void OnMouseDragging(Qt::KeyboardModifiers state_key,
                       Qt::MouseButton mouse_button, QPoint offset);
  void OnEndMouseDragging(Qt::KeyboardModifiers state_key,
                          Qt::MouseButton mouse_button, QPoint pos);
  void OnMouseClicked(Qt::KeyboardModifiers state_key,
                      Qt::MouseButton mouse_button, QPoint pos);
  void OnMouseDoubleClicked(Qt::KeyboardModifiers state_key,
                            Qt::MouseButton mouse_button, QPoint pos);

protected:
  virtual void paintGL();

private:
  uint rendered_count_;
  Qt::KeyboardModifiers start_mouse_dragging_state_key_;
  Qt::MouseButton start_mouse_dragging_button_;
  QPoint start_mouse_dragging_point_;
  Qt::KeyboardModifiers mouse_dragging_state_key_;
  Qt::MouseButton mouse_dragging_button_;
  QPoint mouse_dragging_offset_;
  Qt::KeyboardModifiers end_mouse_dragging_state_key_;
  Qt::MouseButton end_mouse_dragging_button_;
  QPoint end_mouse_dragging_point_;
  Qt::KeyboardModifiers mouse_click_state_key_;
  Qt::MouseButton mouse_click_button_;
  QPoint mouse_click_point_;
  Qt::KeyboardModifiers mouse_double_click_state_key_;
  Qt::MouseButton mouse_double_click_button_;
  QPoint mouse_double_click_point_;
};

OpenGLWidgetTester::OpenGLWidgetTester(const QGLFormat& format, QWidget* parent,
                                       Qt::WindowFlags f)
  : hs::recon::OpenGLWidget(format, parent, f),
    rendered_count_(0),
    start_mouse_dragging_button_(Qt::NoButton),
    mouse_dragging_button_(Qt::NoButton),
    end_mouse_dragging_button_(Qt::NoButton),
    mouse_click_button_(Qt::NoButton),
    mouse_double_click_button_(Qt::NoButton)
{
  QObject::connect(this, &OpenGLWidget::StartMouseDragging,
                   this, &OpenGLWidgetTester::OnStartMouseDragging);
  QObject::connect(this, &OpenGLWidget::MouseDragging,
                   this, &OpenGLWidgetTester::OnMouseDragging);
  QObject::connect(this, &OpenGLWidget::EndMouseDragging,
                   this, &OpenGLWidgetTester::OnEndMouseDragging);
  QObject::connect(this, &OpenGLWidget::MouseClicked,
                   this, &OpenGLWidgetTester::OnMouseClicked);
  QObject::connect(this, &OpenGLWidget::MouseDoubleClicked,
                   this, &OpenGLWidgetTester::OnMouseDoubleClicked);
}

OpenGLWidgetTester* OpenGLWidgetTester::Create(QWidget* parent,
                                              Qt::WindowFlags f)
{
  QGLFormat format;
#if defined __APPLE_CC__
  format.setVersion(3, 3);
  format.setProfile(QGLFormat::CoreProfile);
#endif
  return new OpenGLWidgetTester(format, parent, f);
}

uint OpenGLWidgetTester::rendered_count() const
{
  return rendered_count_;
}

Qt::KeyboardModifiers OpenGLWidgetTester::start_mouse_dragging_state_key() const
{
  return start_mouse_dragging_state_key_;
}
Qt::MouseButton OpenGLWidgetTester::start_mouse_dragging_button() const
{
  return start_mouse_dragging_button_;
}
QPoint OpenGLWidgetTester::start_mouse_dragging_point() const
{
  return start_mouse_dragging_point_;
}
Qt::KeyboardModifiers OpenGLWidgetTester::mouse_dragging_state_key() const
{
  return mouse_dragging_state_key_;
}
Qt::MouseButton OpenGLWidgetTester::mouse_dragging_button() const
{
  return mouse_dragging_button_;
}
QPoint OpenGLWidgetTester::mouse_dragging_offset() const
{
  return mouse_dragging_offset_;
}
Qt::KeyboardModifiers OpenGLWidgetTester::end_mouse_dragging_state_key() const
{
  return end_mouse_dragging_state_key_;
}
Qt::MouseButton OpenGLWidgetTester::end_mouse_dragging_button() const
{
  return end_mouse_dragging_button_;
}
QPoint OpenGLWidgetTester::end_mouse_dragging_point() const
{
  return end_mouse_dragging_point_;
}
Qt::KeyboardModifiers OpenGLWidgetTester::mouse_click_state_key() const
{
  return mouse_click_state_key_;
}
Qt::MouseButton OpenGLWidgetTester::mouse_click_button() const
{
  return mouse_click_button_;
}
QPoint OpenGLWidgetTester::mouse_click_point() const
{
  return mouse_click_point_;
}
Qt::KeyboardModifiers OpenGLWidgetTester::mouse_double_click_state_key() const
{
  return mouse_double_click_state_key_;
}
Qt::MouseButton OpenGLWidgetTester::mouse_double_click_button() const
{
  return mouse_double_click_button_;
}
QPoint OpenGLWidgetTester::mouse_double_click_point() const
{
  return mouse_double_click_point_;
}

void OpenGLWidgetTester::OnStartMouseDragging(Qt::KeyboardModifiers state_key,
                                              Qt::MouseButton mouse_button,
                                              QPoint pos)
{
  start_mouse_dragging_state_key_ = state_key;
  start_mouse_dragging_button_ = mouse_button;
  start_mouse_dragging_point_ = pos;
}

void OpenGLWidgetTester::OnMouseDragging(Qt::KeyboardModifiers state_key,
                                         Qt::MouseButton mouse_button,
                                         QPoint offset)
{
  mouse_dragging_state_key_ = state_key;
  mouse_dragging_button_ = mouse_button;
  mouse_dragging_offset_ = offset;
}

void OpenGLWidgetTester::OnEndMouseDragging(Qt::KeyboardModifiers state_key,
                                            Qt::MouseButton mouse_button,
                                            QPoint pos)
{
  end_mouse_dragging_state_key_ = state_key;
  end_mouse_dragging_button_ = mouse_button;
  end_mouse_dragging_point_ = pos;
}

void OpenGLWidgetTester::OnMouseClicked(Qt::KeyboardModifiers state_key,
                                        Qt::MouseButton mouse_button,
                                        QPoint pos)
{
  mouse_click_state_key_ = state_key;
  mouse_click_button_ = mouse_button;
  mouse_click_point_ = pos;
}

void OpenGLWidgetTester::OnMouseDoubleClicked(Qt::KeyboardModifiers state_key,
                                              Qt::MouseButton mouse_button,
                                              QPoint pos)
{
  mouse_double_click_state_key_ = state_key;
  mouse_double_click_button_ = mouse_button;
  mouse_double_click_point_ = pos;
}

void OpenGLWidgetTester::paintGL()
{
  rendered_count_++;
}

class TestOpenGLWidget : public QObject
{
  Q_OBJECT
private slots:
  void TestMouse();
};

void TestOpenGLWidget::TestMouse()
{
  OpenGLWidgetTester* tester = OpenGLWidgetTester::Create();
  int width = 800;
  int height = 600;
  tester->resize(width, height);
  tester->show();

  QPoint start_mouse_dragging_point(600, 400);
  Qt::KeyboardModifiers mouse_dragging_state_key = Qt::ShiftModifier;
  Qt::MouseButton mouse_dragging_button = Qt::LeftButton;
  QTest::mousePress(tester, mouse_dragging_button, mouse_dragging_state_key,
                    start_mouse_dragging_point);
  QPoint mouse_dragging_point(500, 400);
  QTest::mouseEvent(QTest::MouseMove, tester,
                    mouse_dragging_button, mouse_dragging_state_key,
                    mouse_dragging_point);
  QTest::mouseRelease(tester, mouse_dragging_button, mouse_dragging_state_key,
                      mouse_dragging_point);
  start_mouse_dragging_point.setY(height - start_mouse_dragging_point.y() - 1);
  mouse_dragging_point.setY(height - mouse_dragging_point.y() - 1);
  QPoint mouse_dragging_offset(mouse_dragging_point.x() -
                               start_mouse_dragging_point.x(),
                               mouse_dragging_point.y() -
                               start_mouse_dragging_point.y());
  QCOMPARE(start_mouse_dragging_point, tester->start_mouse_dragging_point());
  QCOMPARE(mouse_dragging_state_key, tester->start_mouse_dragging_state_key());
  QCOMPARE(mouse_dragging_button, tester->start_mouse_dragging_button());
  //TODO:No mouse move event send!
  //QCOMPARE(mouse_dragging_state_key, tester->mouse_dragging_state_key());
  //QCOMPARE(mouse_dragging_offset, tester->mouse_dragging_offset());
  //QCOMPARE(mouse_dragging_button, tester->mouse_dragging_button());
  QCOMPARE(mouse_dragging_state_key, tester->end_mouse_dragging_state_key());
  QCOMPARE(mouse_dragging_point, tester->end_mouse_dragging_point());
  QCOMPARE(mouse_dragging_button, tester->end_mouse_dragging_button());

  QPoint mouse_click_point(300, 200);
  Qt::KeyboardModifiers mouse_click_state_key = Qt::ControlModifier;
  Qt::MouseButton mouse_click_button = Qt::RightButton;
  QTest::mouseClick(tester, mouse_click_button, mouse_click_state_key,
                    mouse_click_point);
  mouse_click_point.setY(height - mouse_click_point.y() - 1);
  QCOMPARE(mouse_click_state_key, tester->mouse_click_state_key());
  QCOMPARE(mouse_click_point, tester->mouse_click_point());
  QCOMPARE(mouse_click_button, tester->mouse_click_button());

  QPoint mouse_double_click_point(200, 100);
  Qt::KeyboardModifiers mouse_double_click_state_key = Qt::AltModifier;
  Qt::MouseButton mouse_double_click_button = Qt::MiddleButton;
  QTest::mouseDClick(tester, mouse_double_click_button,
                     mouse_double_click_state_key,
                     mouse_double_click_point);
  mouse_double_click_point.setY(height - mouse_double_click_point.y() - 1);
  QCOMPARE(mouse_double_click_state_key,
           tester->mouse_double_click_state_key());
  QCOMPARE(mouse_double_click_point, tester->mouse_double_click_point());
  QCOMPARE(mouse_double_click_button, tester->mouse_double_click_button());

}

}

QTEST_MAIN(test::TestOpenGLWidget)
#include "test_opengl_widget.moc"
