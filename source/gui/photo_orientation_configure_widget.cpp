#include "gui/photo_orientation_configure_widget.hpp"

#include "hs_cartographics/cartographics_qt/coordinate_system_config_dialog.hpp"

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
  layout_inbox_ = new QVBoxLayout;
  group_box_->setLayout(layout_inbox_);

  //push_button_config_coordinate_system_ =
  //  new QPushButton(tr("Configure Coordinate System"), this);
  //layout_inbox_->addWidget(push_button_config_coordinate_system_);

  //QObject::connect(
  //  push_button_config_coordinate_system_, &QPushButton::clicked,
  //  this, &PhotoOrientationConfigureWidget::
  //         OnButtonConfigCoordinateSystemClicked);
}

void PhotoOrientationConfigureWidget::FetchPhotoOrientationConfig(
  workflow::PhotoOrientationConfig& photo_orientation_config)
{
}

void PhotoOrientationConfigureWidget::OnButtonConfigCoordinateSystemClicked()
{
  hs::cartographics::qt::CoordinateSystemConfigDialog dialog;
  dialog.SetCoordinateSystem(coordinate_system_);
  if (dialog.exec())
  {
    coordinate_system_ = dialog.GetCoordinateSystem();
  }
}

}
}
}
