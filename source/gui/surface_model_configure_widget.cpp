#include "gui/surface_model_configure_widget.hpp"

#include <QIntValidator>
#include <QDoubleValidator>

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
  group_box_layout_ = new QVBoxLayout;

  layout_group_box_dem_ = new QVBoxLayout;

  combo_box_ = new QComboBox;
  combo_box_->setEditable(false);
  QStringList item_text;
  item_text << tr("Low")
    << tr("Medium")
    << tr("High");
  combo_box_->addItems(item_text);
  combo_box_->setCurrentIndex(1);
  group_box_layout_->addWidget(combo_box_);

  layout_dem_path_ = new QHBoxLayout;
  label_dem_path_ = new QLabel(tr("DEM Path:"));
  line_edit_dem_path_ = new QLineEdit;
  line_edit_dem_path_->setEnabled(false);
  button_browse_ = new QPushButton(tr("Browse"));
  layout_dem_path_->addWidget(label_dem_path_);
  layout_dem_path_->addWidget(line_edit_dem_path_);
  layout_dem_path_->addWidget(button_browse_);
  layout_group_box_dem_->addLayout(layout_dem_path_);

  QIntValidator* int_validator = new QIntValidator(this);
  int_validator->setBottom(0);
  QDoubleValidator* double_validator = new QDoubleValidator(this);
  double_validator->setBottom(0.0);
  layout_dem_scale_ = new QHBoxLayout;
  label_dem_x_scale_ = new QLabel(tr("DEM X Scale:"));
  line_edit_dem_x_scale_ = new QLineEdit;
  line_edit_dem_x_scale_->setValidator(int_validator);
  line_edit_dem_x_scale_->setText(QString::number(0.1));
  label_dem_y_scale_ = new QLabel(tr("DEM Y Scale:"));
  line_edit_dem_y_scale_ = new QLineEdit;
  line_edit_dem_y_scale_->setValidator(int_validator);
  line_edit_dem_y_scale_->setText(QString::number(0.1));
  layout_dem_scale_->addWidget(label_dem_x_scale_);
  layout_dem_scale_->addWidget(line_edit_dem_x_scale_);
  layout_dem_scale_->addWidget(label_dem_y_scale_);
  layout_dem_scale_->addWidget(line_edit_dem_y_scale_);
  layout_group_box_dem_->addLayout(layout_dem_scale_);

  layout_dem_tile_size_ = new QHBoxLayout;
  label_dem_tile_x_size_ = new QLabel(tr("DEM Tile X Size:"));
  line_edit_dem_tile_x_size_ = new QLineEdit;
  line_edit_dem_tile_x_size_->setValidator(double_validator);
  line_edit_dem_tile_x_size_->setText(QString::number(2048));
  label_dem_tile_y_size_ = new QLabel(tr("DEM Tile Y Size:"));
  line_edit_dem_tile_y_size_ = new QLineEdit;
  line_edit_dem_tile_y_size_->setValidator(double_validator);
  line_edit_dem_tile_y_size_->setText(QString::number(2048));
  layout_dem_tile_size_->addWidget(label_dem_tile_x_size_);
  layout_dem_tile_size_->addWidget(line_edit_dem_tile_x_size_);
  layout_dem_tile_size_->addWidget(label_dem_tile_y_size_);
  layout_dem_tile_size_->addWidget(line_edit_dem_tile_y_size_);
  layout_group_box_dem_->addLayout(layout_dem_tile_size_);

  group_box_ = new QGroupBox(tr("Surface Model Configure"), this);
  group_box_->setLayout(group_box_layout_);
  main_layout_->addWidget(group_box_);

  group_box_dem_ = new QGroupBox(tr("DEM Configure"), this);
  group_box_dem_->setLayout(layout_group_box_dem_);
  group_box_dem_->setCheckable(true);
  group_box_dem_->setChecked(false);
  main_layout_->addWidget(group_box_dem_);

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
    mesh_surface_config.set_solver_accuracy(0.001);
    mesh_surface_config.set_confidence(0);
    mesh_surface_config.set_polygon_mesh(0);
  }
  else if (combo_box_->currentIndex() == 1)
  {
    mesh_surface_config.set_octree_depth(8);
    mesh_surface_config.set_min_depth(5);
    mesh_surface_config.set_point_weight(4.0);
    mesh_surface_config.set_cube_ratio(1.1);
    mesh_surface_config.set_solver_divide(8);
    mesh_surface_config.set_iso_divide(8);
    mesh_surface_config.set_samples_per_node(1.0);
    mesh_surface_config.set_min_iters(8);
    mesh_surface_config.set_solver_accuracy(0.001);
    mesh_surface_config.set_confidence(0);
    mesh_surface_config.set_polygon_mesh(0);
  }
  else
  {
    mesh_surface_config.set_octree_depth(10);
    mesh_surface_config.set_min_depth(7);
    mesh_surface_config.set_point_weight(5.5);
    mesh_surface_config.set_cube_ratio(1.1);
    mesh_surface_config.set_solver_divide(10);
    mesh_surface_config.set_iso_divide(10);
    mesh_surface_config.set_samples_per_node(1.0);
    mesh_surface_config.set_min_iters(10);
    mesh_surface_config.set_solver_accuracy(0.001);
    mesh_surface_config.set_confidence(0);
    mesh_surface_config.set_polygon_mesh(0);
  }
}

}
}
}
