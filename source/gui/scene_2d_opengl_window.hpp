#ifndef _HS_3D_RECONSTRUCTOR_SCENE_2D_OPENGL_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_SCENE_2D_OPENGL_WINDOW_HPP_

#include "hs_graphics/graphics_utility/viewing_transformer.hpp"

#include "gui/opengl_window.hpp"

namespace hs
{
namespace recon
{

class Scene2DOpenGLWindow : public OpenGLWindow
{
  Q_OBJECT
public:
  typedef float Float;
  typedef hs::graphics::ViewingTransformer<Float> ViewingTransformer;
protected:
  virtual void Render() = 0;
  virtual void Initialize() = 0;
protected:
  void resizeEvent(QResizeEvent* event);
  void wheelEvent(QWheelEvent* event);

protected slots:
  void OnMouseDragging(Qt::KeyboardModifiers, state_key,
                       Qt::MouseButton mouse_button, QPoint offset);

protected:
  ViewingTransformer viewing_transformer_;
};

}
}

#endif
