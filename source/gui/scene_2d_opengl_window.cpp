#include "gui/scene_2d_opengl_window.hpp"

#include <cmath>

#include <QGuiApplication>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMoveEvent>

namespace hs
{
namespace recon
{

Scene2DOpenGLWindow::Scene2DOpenGLWindow(QWindow* parent)
  : OpenGLWindow(parent)
{
  viewing_transformer_.SetPerspective(false);
  viewing_transformer_.SetKeepRatio(true);
  viewing_transformer_.SetProjectionBoundingBox2D(min, max);

  QObject::connect(this, &OpenGLWindow::MouseDragging,
                   this, &Scene2DOpenGLWindow::OnMouseDragging);
}

void Scene2DOpenGLWindow::resizeEvent(QResizeEvent* event)
{
  const qreal retina_scale = devicePixelRatio();
  viewing_transformer_.SetViewport(0, 0,
                                   event->size().width() * retina_scale,
                                   event->size().height() * retina_scale);
}

void Scene2DOpenGLWindow::wheelEvent(QWheelEvent* event)
{
  if (QGuiApplication::keyboardModifiers() ==
      Qt::NoModifier)
  {
    Vector2 center;
    center << Float((width() - 1) / 2) + Float(0.5),
              Float((height() - 1) / 2) + Float(0.5);
    center[1] = (Float(height()) - center[1]) - Float(1);
    const Float base = Float(1.001);
    Float scale = std::pow(base, Float(-event->angleDelta().y()));
    Vector2 cursor;
    cursor << event->pos().x(),
              height() - event->pos().y() - 1;
    Vector2 translate = (Float(1) - Float(1) / scale) * (center - cursor);
    viewing_transformer_.Scale2D(scale);
    viewing_transformer_.Translate2DByView(translate);
    RenderNow();
  }

}

void Scene2DOpenGLWindow::OnMouseDragging(
  Qt::KeyboardModifiers state_key, Qt::MouseButton mouse_button, QPoint offset)
{
  if (state_key == Qt::NoModifier &&
      mouse_button == Qt::LeftButton)
  {
    Vector2 translate;
    translate << Float(-offset.x()),
                 Float(-offset.y());
    viewing_transformer_.Translate2DByView(translate);
    RenderNow();
  }
}

}
}
