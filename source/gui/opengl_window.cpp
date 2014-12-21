#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QMouseEvent>
#include <QGuiApplication>

#include "opengl_window.hpp"

namespace hs
{
namespace recon
{

QPoint OpenGLWindow::QtPosToGLPos(const QPoint& qt_pos)
{
  QPoint gl_pos = qt_pos;
  gl_pos.setY(height() - qt_pos.y() - 1);
  return gl_pos;
}

OpenGLWindow::OpenGLWindow(QWindow* parent)
  : QWindow(parent)
  , animating_(false)
  , update_pending_(false)
  , context_(nullptr)
  , mouse_drag_timer_(new QTimer(this))
  , click_threshold_(50)
  , mouse_press_time_point_()
  , mouse_press_pos_()
  , mouse_press_modifiers_(Qt::NoModifier)
  , mouse_press_button_(Qt::NoButton)
  , last_dragging_pos_()
  , start_mouse_dragging_emited_(false)

{
  setSurfaceType(QWindow::OpenGLSurface);
  QSurfaceFormat format;
  format.setMajorVersion(3);
  format.setMinorVersion(3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  setFormat(format);
  QObject::connect(mouse_drag_timer_, &QTimer::timeout,
                   this, &OpenGLWindow::OnTimeOut);
}

void OpenGLWindow::SetAnimating(bool animating)
{
  animating_ = animating;

  if (animating_)
  {
    RenderLater();
  }
}

void OpenGLWindow::RenderNow()
{
  if (!isExposed())
    return;

  bool needs_initialize = false;
  if (!context_)
  {
    context_ = new QOpenGLContext(this);
    context_->setFormat(requestedFormat());
    context_->create();

    needs_initialize = true;
  }

  context_->makeCurrent(this);
  if (needs_initialize)
  {
    this->Initialize();
  }

  this->Render();

  context_->swapBuffers(this);

  if (animating_)
  {
    RenderLater();
  }
}

void OpenGLWindow::RenderLater()
{
  if (!update_pending_)
  {
    update_pending_ = true;
    QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}

bool OpenGLWindow::event(QEvent* event)
{
  switch (event->type())
  {
  case QEvent::UpdateRequest:
    update_pending_ = false;
    RenderNow();
    return true;
  default:
    return QWindow::event(event);
  }
  return true;
}

void OpenGLWindow::exposeEvent(QExposeEvent* event)
{
  Q_UNUSED(event);
  if (isExposed())
    RenderNow();
}

void OpenGLWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
  emit MouseDoubleClicked(event->modifiers(), event->button(),
                          QtPosToGLPos(event->pos()));
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (mouse_press_modifiers_ == event->modifiers() &&
      mouse_press_pos_ != QPoint())
  {
    if (!start_mouse_dragging_emited_)
    {
      emit StartMouseDragging(mouse_press_modifiers_, mouse_press_button_,
                              QtPosToGLPos(mouse_press_pos_));
      last_dragging_pos_ = mouse_press_pos_;
      start_mouse_dragging_emited_ = true;
    }
    emit MouseDragging(mouse_press_modifiers_, mouse_press_button_,
                       QPoint(event->pos().x() - last_dragging_pos_.x(),
                              last_dragging_pos_.y() - event->pos().y()));
    last_dragging_pos_ = event->pos();
  }
}

void OpenGLWindow::mousePressEvent(QMouseEvent* event)
{
  mouse_drag_timer_->start(click_threshold_.count());
  mouse_press_time_point_ = Clock::now();
  mouse_press_pos_ = event->pos();
  mouse_press_modifiers_ = event->modifiers();
  mouse_press_button_ = event->button();
  start_mouse_dragging_emited_ = false;
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent* event)
{
  TimePoint mouse_release_time_point = Clock::now();
  if (mouse_press_modifiers_ == event->modifiers() &&
      mouse_press_button_ == event->button())
  {
    if ((mouse_release_time_point -
         mouse_press_time_point_) < click_threshold_ &&
        mouse_press_pos_ == event->pos())
    {
      emit MouseClicked(mouse_press_modifiers_, mouse_press_button_,
                        QtPosToGLPos(mouse_press_pos_));
    }
    else
    {
      emit EndMouseDragging(event->modifiers(), event->button(),
                            QtPosToGLPos(event->pos()));
    }
  }

  mouse_drag_timer_->stop();
  mouse_press_pos_ = QPoint();
  mouse_press_modifiers_ = Qt::NoModifier;
  mouse_press_button_ = Qt::NoButton;
  last_dragging_pos_ = QPoint();
}

void OpenGLWindow::OnTimeOut()
{
  emit StartMouseDragging(mouse_press_modifiers_, mouse_press_button_,
                          QtPosToGLPos(mouse_press_pos_));
  last_dragging_pos_ = mouse_press_pos_;
  start_mouse_dragging_emited_ = true;
  mouse_drag_timer_->stop();
}

}
}
