#include "gui/image_opengl_window.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

ImageOpenGLWindow::ImageOpenGLWindow(QWindow* parent)
  : Base(parent)
  , thumbnail_image_render_layer_(new ThumbnailImageRenderLayer())
{
  AddRenderLayer(
    std::static_pointer_cast<RenderLayer>(thumbnail_image_render_layer_));
  SetBackgroundColor(Float(1), Float(1), Float(1), Float(1));
}

void ImageOpenGLWindow::SetThumbnailImage(
  int width, int height, const ImageData& thumbnail_image_data)
{
  thumbnail_image_render_layer_->SetThumbnailImage(width, height,
                                                   thumbnail_image_data);
  ViewAll();
  RenderNow();
}

void ImageOpenGLWindow::SetOriginImage(const ImageData& origin_image_data)
{
  thumbnail_image_render_layer_->SetOriginImage(origin_image_data);
  RenderNow();
}

void ImageOpenGLWindow::ClearImage()
{
  thumbnail_image_render_layer_->ClearImage();
  RenderNow();
}

}
}
}
