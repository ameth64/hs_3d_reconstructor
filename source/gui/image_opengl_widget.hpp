#ifndef _HS_3D_RECONSTRUCTOR_IMAGE_OPENGL_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_IMAGE_OPENGL_WIDGET_HPP_

#include "hs_graphics/graphics_render/image_renderer.hpp"

#include "gui/opengl_widget.hpp"

namespace hs
{
namespace recon
{

class ImageOpenGLWidget : public OpenGLWidget
{
  Q_OBJECT
public:
  typedef float Float;
  typedef hs::graphics::render::ImageRenderer<Float> ImageRenderer;
  typedef ImageRenderer::ImageData ImageData;
  typedef ImageRenderer::ViewingTransformer ViewingTransformer;
private:
  typedef EIGEN_VECTOR(Float, 2) Vector2;
protected:
  ImageOpenGLWidget(const QGLFormat& format, QWidget* parent, Qt::WindowFlags f);
public:
  static ImageOpenGLWidget* Create(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~ImageOpenGLWidget();

  void DisplayImage(const ImageData& image_data);
  void ClearImage();

protected:
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();

protected:
  void wheelEvent(QWheelEvent* event);
private:
  void CenterView(const ImageData& image_data);

public:
  void OnMouseDragging(Qt::KeyboardModifiers state_key,
                       Qt::MouseButton mouse_button, QPoint offset);
private:
  ImageRenderer* image_renderer_;
  ViewingTransformer viewing_transformer_;
  ImageData image_data_;
  bool need_reload_image_;
  bool need_clear_image_;
};

}
}

#endif
