#include "gui/surface_model_configure_widget.hpp"

#include <QIntValidator>
#include <QDoubleValidator>
#include <QFileDialog>

namespace hs
{
namespace recon
{
namespace gui
{

SurfaceModelConfigureWidget::SurfaceModelConfigureWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  main_layout_ = new QVBoxLayout;
  setLayout(main_layout_);

  group_box_ = new QGroupBox(tr("Surface Model Configure"), this);
  main_layout_->addWidget(group_box_);

  group_box_layout_ = new QVBoxLayout;
  group_box_->setLayout(group_box_layout_);
  layout_surface_model_quality_ = new QHBoxLayout;
  group_box_layout_->addLayout(layout_surface_model_quality_);

  label_surface_model_quality_ =
    new QLabel(tr("Surface Model Quality "), group_box_);
  layout_surface_model_quality_->addWidget(label_surface_model_quality_);

  combo_box_ = new QComboBox;
  combo_box_->setEditable(false);
  QStringList item_text;
  item_text << tr("Low")
            << tr("Medium")
            << tr("High");
  combo_box_->addItems(item_text);
  combo_box_->setCurrentIndex(1);
  layout_surface_model_quality_->addWidget(combo_box_);



}

void SurfaceModelConfigureWidget::FetchSurfaceModelConfig(
  workflow::MeshSurfaceConfig& mesh_surface_config)
{
  if (combo_box_->currentIndex() == 0)
  {
    mesh_surface_config.set_octree_depth(6);
    mesh_surface_config.set_min_depth(3);
    mesh_surface_config.set_point_weight(3.0);
    mesh_surface_config.set_cube_ratio(1.0);
    mesh_surface_config.set_solver_divide(6);
    mesh_surface_config.set_iso_divide(6);
    mesh_surface_config.set_samples_per_node(1.0);
    mesh_surface_config.set_min_iters(6);
    mesh_surface_config.set_solver_accuracy(0.001f);
    mesh_surface_config.set_confidence(0);
    mesh_surface_config.set_polygon_mesh(0);
  }
  else if (combo_box_->currentIndex() == 1)
  {
    mesh_surface_config.set_octree_depth(8);
    mesh_surface_config.set_min_depth(5);
    mesh_surface_config.set_point_weight(4.0f);
    mesh_surface_config.set_cube_ratio(1.1f);
    mesh_surface_config.set_solver_divide(8);
    mesh_surface_config.set_iso_divide(8);
    mesh_surface_config.set_samples_per_node(1.0f);
    mesh_surface_config.set_min_iters(8);
    mesh_surface_config.set_solver_accuracy(0.001f);
    mesh_surface_config.set_confidence(0);
    mesh_surface_config.set_polygon_mesh(0);
  }
  else
  {
    mesh_surface_config.set_octree_depth(10);
    mesh_surface_config.set_min_depth(7);
    mesh_surface_config.set_point_weight(5.5f);
    mesh_surface_config.set_cube_ratio(1.1f);
    mesh_surface_config.set_solver_divide(10);
    mesh_surface_config.set_iso_divide(10);
    mesh_surface_config.set_samples_per_node(1.0f);
    mesh_surface_config.set_min_iters(10);
    mesh_surface_config.set_solver_accuracy(0.001f);
    mesh_surface_config.set_confidence(0);
    mesh_surface_config.set_polygon_mesh(0);
  }

}

}
}
}
