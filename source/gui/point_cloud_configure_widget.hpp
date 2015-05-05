#ifndef _HS_3D_RECONSTRUCTOR_POINT_CLOUD_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_POINT_CLOUD_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "workflow/point_cloud/pmvs_point_cloud.hpp"

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

  void FetchPointCloudConfig(
    workflow::PointCloudConfig& point_cloud_config);

private:
  QVBoxLayout* layout_all_;
  QVBoxLayout* layout_inbox_;
  QGroupBox* group_box_;

};

}
}
}

#endif
