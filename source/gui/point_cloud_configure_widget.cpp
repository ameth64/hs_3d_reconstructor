#include "gui/point_cloud_configure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PointCloudConfigureWidget::PointCloudConfigureWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  layout_all_ = new QVBoxLayout;
  setLayout(layout_all_);

  group_box_ = new QGroupBox(tr("PMVS Point Cloud Configure"), this);
  layout_all_->addWidget(group_box_);

}

void PointCloudConfigureWidget::FetchPointCloudConfig(
  workflow::PointCloudConfig& point_cloud_config)
{

}

}
}
}
