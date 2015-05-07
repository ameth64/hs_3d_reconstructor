#ifndef _HS_3D_RECONSTRUCTOR_SURFACE_MODEL_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_SURFACE_MODEL_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>

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
  QVBoxLayout* main_layout_;
  QVBoxLayout* group_box_layout_;

  QGroupBox* group_box_;
  QComboBox* combo_box_;


};

}
}
}

#endif
