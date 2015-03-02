#include <QIntValidator>

#include "gui/feature_match_configure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

FeatureMatchConfigureWidget::FeatureMatchConfigureWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  layout_all_ = new QVBoxLayout;
  setLayout(layout_all_);

  group_box_ = new QGroupBox(tr("Feature Match Configure"), this);
  layout_all_->addWidget(group_box_);

  layout_inbox_ = new QVBoxLayout;
  group_box_->setLayout(layout_inbox_);
  layout_feature_point_limits_ = new QHBoxLayout;
  layout_inbox_->addLayout(layout_feature_point_limits_);

  label_feature_point_limits_ =
    new QLabel(tr("Feature Point Limits"), group_box_);
  layout_feature_point_limits_->addWidget(label_feature_point_limits_);
  line_edit_feature_point_limits_ = new QLineEdit(group_box_);
  line_edit_feature_point_limits_->setText(QString::number(20000));
  QValidator* validator = new QIntValidator(5000, 40000, group_box_);
  line_edit_feature_point_limits_->setValidator(validator);
  layout_feature_point_limits_->addWidget(line_edit_feature_point_limits_);

}

void FeatureMatchConfigureWidget::FetchFeatureMatchConfig(
  workflow::FeatureMatchConfig& feature_match_config)
{
  feature_match_config.set_keys_limits(
    line_edit_feature_point_limits_->text().toInt());
}

}
}
}
