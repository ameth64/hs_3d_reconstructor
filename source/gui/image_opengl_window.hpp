#ifndef _HS_3D_RECONSTRUCTOR_IMAGE_OPENGL_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_IMAGE_OPENGL_WINDOW_HPP_

#include "hs_graphics/graphics_qt/scene_2d_opengl_window.hpp"
#include "hs_graphics/graphics_render/thumbnail_image_render_layer.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class ImageOpenGLWindow : public hs::graphics::qt::Scene2DOpenGLWindow
{
  Q_OBJECT
public:
  typedef hs::graphics::qt::Scene2DOpenGLWindow Base;
  typedef Base::ViewingTransformer ViewingTransformer;
  typedef Base::Float Float;
  typedef Base::RenderLayer RenderLayer;
  typedef hs::graphics::render::ThumbnailImageRenderLayer<Float>
          ThumbnailImageRenderLayer;
  typedef std::shared_ptr<ThumbnailImageRenderLayer>
          PThumbnailImageRenderLayer;

  typedef hs::imgio::whole::ImageData ImageData;

public:
  ImageOpenGLWindow(QWindow* parent = 0);

  void SetThumbnailImage(int width, int height,
                        const ImageData& thumbnail_image_data);
  void SetOriginImage(const ImageData& origin_image_data);
  void ClearImage();

  int image_width() const;
  int image_height() const;

private:
  PThumbnailImageRenderLayer thumbnail_image_render_layer_;
};

}
}
}

#endif
