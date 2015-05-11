#ifndef _HS_3D_RECONSTRUCTOR_SURFACE_MODEL_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_SURFACE_MODEL_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include"workflow/mesh_surface/poisson_surface_model.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class SurfaceModelConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  SurfaceModelConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
  void FetchSurfaceModelConfig(workflow::MeshSurfaceConfig& mesh_surface_config);

private:
  void OnButtonBrowseClicked();

private:
  QVBoxLayout* main_layout_;
  QVBoxLayout* group_box_layout_;

  QGroupBox* group_box_;
  QComboBox* combo_box_;

  QGroupBox* group_box_dem_;
  QVBoxLayout* layout_group_box_dem_;

  QHBoxLayout* layout_dem_path_;
  QLabel* label_dem_path_;
  QLineEdit* line_edit_dem_path_;
  QPushButton* button_browse_;

  QHBoxLayout* layout_dem_scale_;
  QLabel* label_dem_x_scale_;
  QLineEdit* line_edit_dem_x_scale_;
  QLabel* label_dem_y_scale_;
  QLineEdit* line_edit_dem_y_scale_;

  QHBoxLayout* layout_dem_tile_size_;
  QLabel* label_dem_tile_x_size_;
  QLineEdit* line_edit_dem_tile_x_size_;
  QLabel* label_dem_tile_y_size_;
  QLineEdit* line_edit_dem_tile_y_size_;
};

}
}
}

#endif
