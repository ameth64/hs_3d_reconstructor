#include "gui/image_opengl_window.hpp"

#include <cmath>

#include <QResizeEvent>
#include <QWheelEvent>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QExposeEvent>
#include <QMoveEvent>

namespace hs
{
namespace recon
{

ImageOpenGLWindow::ImageOpenGLWindow(QWindow* parent)
  : OpenGLWindow(parent)
  , image_renderer_(nullptr)
  , need_reload_image_(false)
  , need_clear_image_(false)
  , need_set_origin_image_(false)
  , image_width_(0)
  , image_height_(0)
{
  //SetAnimating(true);
  QObject::connect(this, &OpenGLWindow::MouseDragging,
                   this, &ImageOpenGLWindow::OnMouseDragging);
}

ImageOpenGLWindow::~ImageOpenGLWindow()
{
  if (image_renderer_ != nullptr)
  {
    delete image_renderer_;
    image_renderer_ = nullptr;
  }
}

void ImageOpenGLWindow::DisplayThumbnailImage(
  int width, int height, const ImageData& thumbnail_image_data)
{
  image_width_ = width;
  image_height_ = height;
  thumbnail_image_data_ = thumbnail_image_data;
  need_reload_image_ = true;
  CenterView();
  RenderNow();
}

void ImageOpenGLWindow::SetOriginImage(const ImageData& origin_image_data)
{
  origin_image_data_ = origin_image_data;
  need_set_origin_image_ = true;
  RenderNow();
}

void ImageOpenGLWindow::ClearImage()
{
  thumbnail_image_data_.Reset();
  origin_image_data_.Reset();
  need_clear_image_ = true;
  RenderNow();
}

void ImageOpenGLWindow::Render()
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
    image_renderer_ = new ImageRenderer(image_width_, image_height_,
                                        thumbnail_image_data_);
    need_reload_image_ = false;
  }

  if (need_set_origin_image_ && image_renderer_)
  {
    image_renderer_->LoadOriginImage(origin_image_data_);
    need_set_origin_image_ = false;
  }

  if (image_renderer_)
  {
    image_renderer_->Render(viewing_transformer_);
  }
  glFlush();
}

void ImageOpenGLWindow::Initialize()
{
  glClearColor(1.0, 0.0, 1.0, 0.0);
}

void ImageOpenGLWindow::resizeEvent(QResizeEvent* event)
{
  const qreal retina_scale = devicePixelRatio();
  viewing_transformer_.SetViewport(0, 0,
                                   event->size().width() * retina_scale,
                                   event->size().height() * retina_scale);
}

void ImageOpenGLWindow::wheelEvent(QWheelEvent* event)
{
  if (image_renderer_ && QGuiApplication::keyboardModifiers() == Qt::NoModifier)
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

void ImageOpenGLWindow::CenterView()
{
  viewing_transformer_.SetPerspective(false);
  viewing_transformer_.SetKeepRatio(true);
  Vector2 min = Vector2::Zero();
  Vector2 max;
  max << Float(image_width_),
         Float(image_height_);
  viewing_transformer_.SetProjectionBoundingBox2D(min, max);
}

void ImageOpenGLWindow::OnMouseDragging(
  Qt::KeyboardModifiers state_key, Qt::MouseButton mouse_button, QPoint offset)
{
  if (image_renderer_ &&
      state_key == Qt::NoModifier &&
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
