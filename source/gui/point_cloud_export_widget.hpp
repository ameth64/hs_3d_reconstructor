#ifndef _HS_3D_RECONSTRUCTOR_POINT_CLOUD_EXPORT_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_POINT_CLOUD_EXPORT_WIDGET_HPP_

#include <QWidget>

namespace hs
{
namespace recon
{
namespace gui
{

class PointCloudExportWidget : public QWidget
{
  Q_OBJECT
public:
  PointCloudExportWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
};

}
}
}

#endif
