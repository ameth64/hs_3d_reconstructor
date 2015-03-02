#include "gui/workflow_configure_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

WorkflowConfigureDialog::WorkflowConfigureDialog(
  int start_configure_type,
  QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  layout_ = new QVBoxLayout(this);

  workflow_configure_widget_ =
    new WorkflowConfigureWidget(start_configure_type, this);
  layout_->addWidget(workflow_configure_widget_);

  dialog_button_box_ok_cancel_ =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                         this);
  layout_->addWidget(dialog_button_box_ok_cancel_);

  QObject::connect(dialog_button_box_ok_cancel_, &QDialogButtonBox::accepted,
                   this, &QDialog::accept);
  QObject::connect(dialog_button_box_ok_cancel_, &QDialogButtonBox::rejected,
                   this, &QDialog::reject);
}

WorkflowConfigureWidget::ConfigureType
WorkflowConfigureDialog::FirstConfigureType() const
{
  return workflow_configure_widget_->FirstConfigureType();
}

WorkflowConfigureWidget::ConfigureType
WorkflowConfigureDialog::LastConfigureType() const
{
  return workflow_configure_widget_->LastConfigureType();
}

void WorkflowConfigureDialog::FetchFeatureMatchConfig(
  workflow::FeatureMatchConfig& feature_match_config)
{
  workflow_configure_widget_->FetchFeatureMatchConfig(feature_match_config);
}

void WorkflowConfigureDialog::FetchPhotoOrientationConfig(
  workflow::PhotoOrientationConfig& photo_orientation_config)
{
  workflow_configure_widget_->FetchPhotoOrientationConfig(
    photo_orientation_config);
}

}
}
}
