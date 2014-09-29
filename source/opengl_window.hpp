#ifndef _HS_3D_RECONSTRUCTOR_OPENGL_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_OPENGL_WINDOW_HPP_

#include <QWindow>

namespace hs
{
namespace recon
{

class OpenGLWindow : public QWindow
{
  Q_OBJECT
public:
  OpenGLWindow(QWindow* parent = 0);

public:
  void SetAnimating(bool animating);

protected:
  void RenderNow();
  void RenderLater();
  virtual void Render() = 0;

protected:
  bool event(QEvent* event);
  void exposeEvent(QExposeEvent* event);

signals:
  void StartMouseDragging(Qt::MouseButton mouse_button, QPoint pos);
  void MouseDragging(Qt::MouseButton mouse_button, QPoint offset);
  void EndMouseDragging(Qt::MouseButton mouse_button, QPoint pos);
  void MouseClicked(Qt::MouseButton mouse_button, QPoint pos);
  void MouseDoubleClicked(Qt::MouseButton mouse_button, QPoint pos);

private:
  bool animating_;
  QOpenGLContext* context_;
};

}
}

#endif
