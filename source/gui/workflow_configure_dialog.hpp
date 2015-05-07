#ifndef _HS_3D_RECONSTRUCTOR_GUI_WORKFLOW_CONFIGURE_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_WORKFLOW_CONFIGURE_DIALOG_HPP_

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "gui/workflow_configure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class WorkflowConfigureDialog : public QDialog
{
  Q_OBJECT
public:
  WorkflowConfigureDialog(
    int start_configure_type,
    QWidget* parent = 0, Qt::WindowFlags f = 0);

public:
  WorkflowConfigureWidget::ConfigureType FirstConfigureType() const;
  WorkflowConfigureWidget::ConfigureType LastConfigureType() const;
  void FetchFeatureMatchConfig(
    workflow::FeatureMatchConfig& feature_match_config);
  void FetchPhotoOrientationConfig(
    workflow::PhotoOrientationConfig& photo_orientation_config);
  void FetchPointCloudConfig(
    workflow::PointCloudConfig& point_cloud_config);
  void FetchSurfaceModelConfig(
    workflow::MeshSurfaceConfig& surface_model_config);


private:
  WorkflowConfigureWidget* workflow_configure_widget_;
  QDialogButtonBox* dialog_button_box_ok_cancel_;
  QVBoxLayout* layout_;
};

}
}
}

#endif
