#ifndef _HS_3D_RECONSTRUCTOR_SECNE_2D_RENDER_LAYER_HPP_
#define _HS_3D_RECONSTRUCTOR_SECNE_2D_RENDER_LAYER_HPP_

#include "hs_graphics/graphics_utility/viewing_transformer.hpp"

namespace hs
{
namespace recon
{

/**
 *  2维场景渲染图层抽象类。
 *  该类将2维场景图层渲染所需的公共接口抽象出来，使得GUI窗口类可以将所有的2维图形渲染问题视为具有同样接口的渲染图层。
 */
template <typename _Scalar>
class Scene2DRenderLayer
{
public:
  typedef _Scalar Scalar;
  typedef hs::graphics::ViewingTransformer<Scalar> ViewingTransformer;

  virtual void Render(const ViewingTransformer& viewing_transformer) = 0;
};

}
}

#endif
