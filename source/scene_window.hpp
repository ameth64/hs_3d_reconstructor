#ifndef _HS_3D_RECONSTRUCTOR_SCENE_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_SCENE_WINDOW_HPP_

#include "opengl_window.hpp"

namespace hs
{
namespace recon
{

class SceneWindow : public OpenGLWindow
{
  Q_OBJECT
public:
  SceneWindow(QWindow* parent);
};

}
}

#endif
