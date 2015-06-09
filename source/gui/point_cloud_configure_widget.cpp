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

  group_box_ = new QGroupBox(tr("Point Cloud Configure"), this);
  layout_all_->addWidget(group_box_);

  layout_inbox_ = new QVBoxLayout;
  group_box_->setLayout(layout_inbox_);
  layout_point_cloud_quality_ = new QHBoxLayout;
  layout_inbox_->addLayout(layout_point_cloud_quality_);

  label_point_cloud_quality_ =
    new QLabel(tr("Point Cloud Quality "), group_box_);
  layout_point_cloud_quality_->addWidget(label_point_cloud_quality_);

  combo_box_ = new QComboBox;
  combo_box_->setEditable(false);
  QStringList item_text;
  item_text << tr("Low")
            << tr("Medium")
            << tr("High");
  combo_box_->addItems(item_text);
  combo_box_->setCurrentIndex(1);
  layout_inbox_->addWidget(combo_box_);

}

void PointCloudConfigureWidget::FetchPointCloudConfig(
  workflow::PointCloudConfig& point_cloud_config)
{
  if (combo_box_->currentIndex() == 0) //Low
  {
    point_cloud_config.set_s_pyramid_level(4);
    point_cloud_config.set_s_patch_density(2);
    point_cloud_config.set_s_patch_range(2);
    point_cloud_config.set_c_accuracy_threshold(0.7f);
    point_cloud_config.set_c_coverage_threshold(0.7f);
    point_cloud_config.set_c_cluster_size(50);
    point_cloud_config.set_p_visibility_threshold(2);
    point_cloud_config.set_p_consistency_threshold(0.7f);
    point_cloud_config.set_p_group_threshold(0.001f);
    point_cloud_config.set_m_quality_threshold(1);
    point_cloud_config.set_m_visibility_threshold(2);
  }
  else if (combo_box_->currentIndex() == 1) //Medium
  {
    point_cloud_config.set_s_pyramid_level(3);
    point_cloud_config.set_s_patch_density(2);
    point_cloud_config.set_s_patch_range(2);
    point_cloud_config.set_c_accuracy_threshold(0.7f);
    point_cloud_config.set_c_coverage_threshold(0.7f);
    point_cloud_config.set_c_cluster_size(50);
    point_cloud_config.set_p_visibility_threshold(2);
    point_cloud_config.set_p_consistency_threshold(0.7f);
    point_cloud_config.set_p_group_threshold(0.001f);
    point_cloud_config.set_m_quality_threshold(1);
    point_cloud_config.set_m_visibility_threshold(2);
  }
  else //Hight
  {
    point_cloud_config.set_s_pyramid_level(0);
    point_cloud_config.set_s_patch_density(2);
    point_cloud_config.set_s_patch_range(2);
    point_cloud_config.set_c_accuracy_threshold(0.7f);
    point_cloud_config.set_c_coverage_threshold(0.7f);
    point_cloud_config.set_c_cluster_size(50);
    point_cloud_config.set_p_visibility_threshold(2);
    point_cloud_config.set_p_consistency_threshold(0.7f);
    point_cloud_config.set_p_group_threshold(0.001f);
    point_cloud_config.set_m_quality_threshold(1);
    point_cloud_config.set_m_visibility_threshold(2);
  }
}

}
}
}
