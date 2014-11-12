#ifndef _HS_3D_RECONSTRUCTOR_OPENGL_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_OPENGL_WINDOW_HPP_

#include <chrono>

#include <QWindow>
#include <QTimer>

namespace hs
{
namespace recon
{

class OpenGLWindow : public QWindow
{
  Q_OBJECT
public:
  typedef std::chrono::steady_clock Clock;
  typedef Clock::time_point TimePoint;
  typedef std::chrono::milliseconds Milliseconds;

public:
  QPoint QtPosToGLPos(const QPoint& qt_pos);

public:
  OpenGLWindow(QWindow* parent = 0);

public:
  void SetAnimating(bool animating);

protected:
  void RenderNow();
  void RenderLater();
  virtual void Render() = 0;
  virtual void Initialize();

protected:
  bool event(QEvent* event);
  virtual void exposeEvent(QExposeEvent* event);
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

protected:
  QOpenGLContext* context_;
private:
  bool animating_;
  bool update_pending_;

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
