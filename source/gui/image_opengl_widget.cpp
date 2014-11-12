#include "gui/image_opengl_widget.hpp"

#include <cmath>

#include <QResizeEvent>
#include <QWheelEvent>
#include <QGuiApplication>

namespace hs
{
namespace recon
{

ImageOpenGLWidget::ImageOpenGLWidget(const QGLFormat& format, QWidget* parent,
                         Qt::WindowFlags f)
  : OpenGLWidget(format, parent, f)
  , image_renderer_(nullptr)
  , need_reload_image_(false)
  , need_clear_image_(false)
{
  QObject::connect(this, &OpenGLWidget::MouseDragging,
                   this, &ImageOpenGLWidget::OnMouseDragging);
}

ImageOpenGLWidget* ImageOpenGLWidget::Create(QWidget* parent, Qt::WindowFlags f)
{
  QGLFormat format;
#if defined __APPLE_CC__
  format.setVersion(3, 3);
  format.setProfile(QGLFormat::CoreProfile);
#endif
  return new ImageOpenGLWidget(format, parent, f);
}

ImageOpenGLWidget::~ImageOpenGLWidget()
{
  if (image_renderer_ != nullptr)
  {
    delete image_renderer_;
    image_renderer_ = nullptr;
  }
}

void ImageOpenGLWidget::DisplayImage(const ImageData& image_data)
{
  image_data_ = image_data;
  need_reload_image_ = true;
  CenterView(image_data);
}

void ImageOpenGLWidget::ClearImage()
{
  image_data_.Reset();
  need_clear_image_ = true;
}

void ImageOpenGLWidget::initializeGL()
{
  glClearColor(1.0, 0.0, 1.0, 0.0);
}

void ImageOpenGLWidget::resizeGL(int w, int h)
{
  viewing_transformer_.SetViewport(0, 0, w, h);
}

void ImageOpenGLWidget::paintGL()
{
  glViewport(viewing_transformer_.viewport_x(),
             viewing_transformer_.viewport_y(),
             viewing_transformer_.viewport_width(),
             viewing_transformer_.viewport_height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (need_clear_image_)
  {
    if (image_renderer_)
    {
      delete image_renderer_;
      image_renderer_ = nullptr;
    }
    need_clear_image_ = false;
  }

  if (need_reload_image_)
  {
    if (image_renderer_)
    {
      delete image_renderer_;
      image_renderer_ = nullptr;
    }
    image_renderer_ = new ImageRenderer(image_data_);
    need_reload_image_ = false;
  }

  if (image_renderer_)
  {
    image_renderer_->Render(viewing_transformer_);
  }
  glFlush();
}

void ImageOpenGLWidget::wheelEvent(QWheelEvent* event)
{
  if (image_renderer_ &&
      QGuiApplication::keyboardModifiers() == Qt::NoModifier)
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
    glDraw();
  }
}

void ImageOpenGLWidget::CenterView(const ImageData& image_data)
{
  viewing_transformer_.SetPerspective(false);
  viewing_transformer_.SetKeepRatio(true);
  Vector2 min = Vector2::Zero();
  Vector2 max;
  max << image_data.width(),
         image_data.height();
  viewing_transformer_.SetProjectionBoundingBox2D(min, max);
}

void ImageOpenGLWidget::OnMouseDragging(Qt::KeyboardModifiers state_key,
                                  Qt::MouseButton mouse_button, QPoint offset)
{
  if (image_renderer_ &&
      state_key == Qt::NoModifier &&
      mouse_button == Qt::LeftButton)
  {
    Vector2 translate;
    translate << Float(-offset.x()),
                 Float(-offset.y());
    viewing_transformer_.Translate2DByView(translate);
    glDraw();
  }
}

}
}
