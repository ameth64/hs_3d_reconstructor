#ifndef _HS_3D_RECONSTRUCTOR_POINT_CLOUD_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_POINT_CLOUD_CONFIGURE_WIDGET_HPP_

#include <QWidget>

namespace hs
{
namespace recon
{
namespace gui
{

class PointCloudConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  PointCloudConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
};

}
}
}

#endif
