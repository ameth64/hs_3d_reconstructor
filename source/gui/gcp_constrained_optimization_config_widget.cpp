#include <QDoubleValidator>

#include "gui/gcp_constrained_optimization_config_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

GCPConstrainedOptimizationConfigWidget::GCPConstrainedOptimizationConfigWidget(
  QWidget* parent, Qt::WindowFlags f,
  double gcp_planar_accuracy,
  double gcp_height_accuracy,
  double tiepoint_feature_accuracy,
  double gcp_marker_accuracy)
  : QWidget(parent, f)
{
  layout_all_ = new QVBoxLayout(this);
  setLayout(layout_all_);

  QDoubleValidator* validator = new QDoubleValidator(this);
  validator->setBottom(0.0);

  layout_gcp_planar_accuracy_ = new QHBoxLayout;
  label_gcp_planar_accuracy_ = new QLabel(tr("GCP Planar Accuracy:"));
  line_edit_gcp_planar_accuracy_ = new QLineEdit;
  line_edit_gcp_planar_accuracy_->setValidator(validator);
  line_edit_gcp_planar_accuracy_->setText(
    QString::number(gcp_planar_accuracy));
  layout_gcp_planar_accuracy_->addWidget(label_gcp_planar_accuracy_);
  layout_gcp_planar_accuracy_->addWidget(line_edit_gcp_planar_accuracy_);
  layout_all_->addLayout(layout_gcp_planar_accuracy_);

  layout_gcp_height_accuracy_ = new QHBoxLayout;
  label_gcp_height_accuracy_ = new QLabel(tr("GCP Height Accuracy:"));
  line_edit_gcp_height_accuracy_ = new QLineEdit;
  line_edit_gcp_height_accuracy_->setValidator(validator);
  line_edit_gcp_height_accuracy_->setText(
    QString::number(gcp_height_accuracy));
  layout_gcp_height_accuracy_->addWidget(label_gcp_height_accuracy_);
  layout_gcp_height_accuracy_->addWidget(line_edit_gcp_height_accuracy_);
  layout_all_->addLayout(layout_gcp_height_accuracy_);

  layout_tiepoint_feature_accuracy_ = new QHBoxLayout;
  label_tiepoint_feature_accuracy_ =
    new QLabel(tr("Tiepoint Feature Accuracy (Pixels):"));
  line_edit_tiepoint_feature_accuracy_ = new QLineEdit;
  line_edit_tiepoint_feature_accuracy_->setValidator(validator);
  line_edit_tiepoint_feature_accuracy_->setText(
    QString::number(tiepoint_feature_accuracy));
  layout_tiepoint_feature_accuracy_->addWidget(
    label_tiepoint_feature_accuracy_);
  layout_tiepoint_feature_accuracy_->addWidget(
    line_edit_tiepoint_feature_accuracy_);
  layout_all_->addLayout(layout_tiepoint_feature_accuracy_);

  layout_gcp_marker_accuracy_ = new QHBoxLayout;
  label_gcp_marker_accuracy_ = new QLabel(tr("GCP Marker Accuracy (Pixels):"));
  line_edit_gcp_marker_accuracy_ = new QLineEdit;
  line_edit_gcp_marker_accuracy_->setValidator(validator);
  line_edit_gcp_marker_accuracy_->setText(
    QString::number(gcp_marker_accuracy));
  layout_gcp_marker_accuracy_->addWidget(label_gcp_marker_accuracy_);
  layout_gcp_marker_accuracy_->addWidget(line_edit_gcp_marker_accuracy_);
  layout_all_->addLayout(layout_gcp_marker_accuracy_);

}

double GCPConstrainedOptimizationConfigWidget::GetGCPPlanarAccuracy() const
{
  return line_edit_gcp_planar_accuracy_->text().toDouble();
}

double GCPConstrainedOptimizationConfigWidget::GetGCPHeightAccuracy() const
{
  return line_edit_gcp_height_accuracy_->text().toDouble();
}

double GCPConstrainedOptimizationConfigWidget::GetTiepointFeatureAccuracy()const
{
  return line_edit_tiepoint_feature_accuracy_->text().toDouble();
}

double GCPConstrainedOptimizationConfigWidget::GetGCPMarkerAccuracy() const
{
  return line_edit_gcp_marker_accuracy_->text().toDouble();
}

}
}
}