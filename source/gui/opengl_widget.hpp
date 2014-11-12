#ifndef _HS_3D_RECONSTRUCTOR_OPENGL_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_OPENGL_WIDGET_HPP_

#include <chrono>

#include <QGLWidget>
#include <QTimer>

namespace hs
{
namespace recon
{

class OpenGLWidget : public QGLWidget
{
  Q_OBJECT
public:
  typedef std::chrono::steady_clock Clock;
  typedef Clock::time_point TimePoint;
  typedef std::chrono::milliseconds Milliseconds;

protected:
  OpenGLWidget(const QGLFormat& format, QWidget* parent, Qt::WindowFlags f);
public:
  static OpenGLWidget* Create(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~OpenGLWidget();

public:
QPoint QtPosToGLPos(const QPoint& qt_pos);

protected:
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();

protected:
  void mouseDoubleClickEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);

signals:
  void StartMouseDragging(Qt::KeyboardModifiers state_key,
                          Qt::MouseButton mouse_button, QPoint pos);
  void MouseDragging(Qt::KeyboardModifiers state_key,
                     Qt::MouseButton mouse_button, QPoint offset);
  void EndMouseDragging(Qt::KeyboardModifiers state_key,
                        Qt::MouseButton mouse_button, QPoint pos);
  void MouseClicked(Qt::KeyboardModifiers state_key,
                    Qt::MouseButton mouse_button, QPoint pos);
  void MouseDoubleClicked(Qt::KeyboardModifiers state_key,
                          Qt::MouseButton mouse_button, QPoint pos);

private slots:
  void OnTimeOut();

private:
  QTimer* mouse_drag_timer_;
  Milliseconds click_threshold_;
  TimePoint mouse_press_time_point_;
  QPoint mouse_press_pos_;
  Qt::KeyboardModifiers mouse_press_modifiers_;
  Qt::MouseButton mouse_press_button_;
  QPoint last_dragging_pos_;
  bool start_mouse_dragging_emited_;

};

}
}

#endif
