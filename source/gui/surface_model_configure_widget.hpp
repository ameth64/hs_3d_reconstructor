#ifndef _HS_3D_RECONSTRUCTOR_SURFACE_MODEL_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_SURFACE_MODEL_CONFIGURE_WIDGET_HPP_

#include <QWidget>

namespace hs
{
namespace recon
{
namespace gui
{

class SurfaceModelConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  SurfaceModelConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
};

}
}
}

#endif
