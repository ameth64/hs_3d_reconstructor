#include "gui/gcp_constrained_optimization_config_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

GCPConstrainedOptimizationConfigDialog::GCPConstrainedOptimizationConfigDialog(
  QWidget* parent, Qt::WindowFlags f,
  double gcp_planar_accuracy,
  double gcp_height_accuracy,
  double tiepoint_feature_accuracy,
  double gcp_marker_accuracy)
  : QDialog(parent, f)
{
  layout_ = new QVBoxLayout(this);

  widget_ =
    new GCPConstrainedOptimizationConfigWidget(this, 0,
                                               gcp_planar_accuracy,
                                               gcp_height_accuracy,
                                               tiepoint_feature_accuracy,
                                               gcp_marker_accuracy);
  layout_->addWidget(widget_);

  button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel, this);
  layout_->addWidget(button_box_);

  QObject::connect(button_box_, &QDialogButtonBox::accepted,
                   this, &QDialog::accept);
  QObject::connect(button_box_, &QDialogButtonBox::rejected,
                   this, &QDialog::reject);
}

double GCPConstrainedOptimizationConfigDialog::GetGCPPlanarAccuracy() const
{
  return widget_->GetGCPPlanarAccuracy();
}

double GCPConstrainedOptimizationConfigDialog::GetGCPHeightAccuracy() const
{
  return widget_->GetGCPHeightAccuracy();
}

double GCPConstrainedOptimizationConfigDialog::GetTiepointFeatureAccuracy()const
{
  return widget_->GetTiepointFeatureAccuracy();
}

double GCPConstrainedOptimizationConfigDialog::GetGCPMarkerAccuracy() const
{
  return widget_->GetGCPMarkerAccuracy();
}

}
}
}