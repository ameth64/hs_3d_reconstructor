#ifndef _HS_3D_RECONSTRUCTOR_TEXTURE_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_TEXTURE_CONFIGURE_WIDGET_HPP_

#include <QWidget>

namespace hs
{
namespace recon
{
namespace gui
{

class TextureConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  TextureConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
};

}
}
}

#endif
