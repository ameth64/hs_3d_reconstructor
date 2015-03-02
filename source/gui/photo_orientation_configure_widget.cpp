#include "gui/photo_orientation_configure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotoOrientationConfigureWidget::PhotoOrientationConfigureWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  layout_all_ = new QVBoxLayout;
  setLayout(layout_all_);

  group_box_ = new QGroupBox(tr("Feature Match Configure"), this);
  layout_all_->addWidget(group_box_);
}

void PhotoOrientationConfigureWidget::FetchPhotoOrientationConfig(
  workflow::PhotoOrientationConfig& photo_orientation_config)
{

}

}
}
}
