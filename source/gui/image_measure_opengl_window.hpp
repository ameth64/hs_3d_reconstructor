#ifndef _HS_3D_RECONSTRUCTOR_IMAGE_MEASURE_OPENGL_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_IMAGE_MEASURE_OPENGL_WINDOW_HPP_

#include "hs_graphics/graphics_render/position_icon_2d_render_layer.hpp"

#include "gui/image_opengl_window.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class ImageMeasureOpenGLWindow : public ImageOpenGLWindow
{
  Q_OBJECT
public:
  typedef ImageOpenGLWindow Base;
  typedef Base::ViewingTransformer ViewingTransformer;
  typedef Base::Float Float;
  typedef Base::RenderLayer RenderLayer;
  typedef hs::graphics::render::ThumbnailImageRenderLayer<Float>
          ThumbnailImageRenderLayer;
  typedef std::shared_ptr<ThumbnailImageRenderLayer>
          PThumbnailImageRenderLayer;
  typedef hs::graphics::render::PositionIcon2DRenderLayer<Float>
          PositionIcon2DRenderLayer;
  typedef std::shared_ptr<PositionIcon2DRenderLayer>
          PPositionIcon2DRenderLayer;

  typedef PositionIcon2DRenderLayer::Position Position;
  typedef PositionIcon2DRenderLayer::PositionContainer PositionContainer;

  typedef hs::imgio::whole::ImageData ImageData;

public:
  ImageMeasureOpenGLWindow(QWindow* parent = 0);

  Position pos_predicated() const;

  Position pos_measure() const;

  void set_pos_predicated(const Position& pos_predicated);

  void set_pos_measure(const Position& pos_measure);

  uint photo_id() const;

  void set_photo_id(uint photo_id);

  void ClearPredicated();

  void ClearMeasure();

  void ClearPosition();

protected slots:
  void OnMouseClicked(Qt::KeyboardModifiers state_key,
                      Qt::MouseButton mouse_button, QPoint pos);

signals:
  void Measured(uint photo_id, const Position& position);

private:
  ImageData LoadIcon(const QString& icon_path);

  Position WindowPosToPhotoPos(const Position& window_pos) const;

  Position PhotoPosToWindowPos(const Position& photo_pos) const;

private:
  Position pos_predicated_;
  Position pos_measure_;

  uint photo_id_;

  PPositionIcon2DRenderLayer position_icon_2d_render_layer_predicated_;
  PPositionIcon2DRenderLayer position_icon_2d_render_layer_measure_;
};

}
}
}

#endif
