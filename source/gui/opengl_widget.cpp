#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QMouseEvent>
#include <QGuiApplication>

#include "gui/opengl_widget.hpp"

namespace hs
{
namespace recon
{

QPoint OpenGLWidget::QtPosToGLPos(const QPoint& qt_pos)
{
  QPoint gl_pos = qt_pos;
  gl_pos.setY(height() - qt_pos.y() - 1);
  return gl_pos;
}

OpenGLWidget::OpenGLWidget(const QGLFormat& format, QWidget* parent,
                           Qt::WindowFlags f)
  : QGLWidget(format, parent, 0, f)
  , mouse_drag_timer_(new QTimer(this))
  , click_threshold_(50)
  , mouse_press_time_point_()
  , mouse_press_pos_()
  , mouse_press_modifiers_(Qt::NoModifier)
  , mouse_press_button_(Qt::NoButton)
  , last_dragging_pos_()
  , start_mouse_dragging_emited_(false)
{
  QObject::connect(mouse_drag_timer_, &QTimer::timeout,
                   this, &OpenGLWidget::OnTimeOut);
}

OpenGLWidget* OpenGLWidget::Create(QWidget* parent, Qt::WindowFlags f)
{
  QGLFormat format;
#if defined __APPLE_CC__
  format.setVersion(3, 3);
  format.setProfile(QGLFormat::CoreProfile);
#endif
  return new OpenGLWidget(format, parent, f);
}

OpenGLWidget::~OpenGLWidget()
{
}

void OpenGLWidget::initializeGL()
{
}

void OpenGLWidget::resizeGL(int w, int h)
{
}

void OpenGLWidget::paintGL()
{
}

void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  emit MouseDoubleClicked(event->modifiers(), event->button(),
                          QtPosToGLPos(event->pos()));
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
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

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
  mouse_drag_timer_->start(click_threshold_.count());
  mouse_press_time_point_ = Clock::now();
  mouse_press_pos_ = event->pos();
  mouse_press_modifiers_ = event->modifiers();
  mouse_press_button_ = event->button();
  start_mouse_dragging_emited_ = false;
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
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

void OpenGLWidget::OnTimeOut()
{
  emit StartMouseDragging(mouse_press_modifiers_, mouse_press_button_,
                          QtPosToGLPos(mouse_press_pos_));
  last_dragging_pos_ = mouse_press_pos_;
  start_mouse_dragging_emited_ = true;
  mouse_drag_timer_->stop();
}

}
}
