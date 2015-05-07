#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "gui/feature_match_configure_widget.hpp"
#include "gui/feature_match_export_widget.hpp"
#include "gui/photo_orientation_configure_widget.hpp"
#include "gui/photo_orientation_export_widget.hpp"
#include "gui/point_cloud_configure_widget.hpp"
#include "gui/point_cloud_export_widget.hpp"
#include "gui/surface_model_configure_widget.hpp"
#include "gui/surface_model_export_widget.hpp"
#include "gui/texture_configure_widget.hpp"
#include "gui/texture_export_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class WorkflowConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  enum ConfigureType
  {
    CONFIGURE_NONE = 0,
    CONFIGURE_FEATURE_MATCH,
    CONFIGURE_PHOTO_ORIENTATION,
    CONFIGURE_POINT_CLOUD,
    CONFIGURE_SURFACE_MODEL,
    CONFIGURE_TEXTURE
  };

  WorkflowConfigureWidget(int start_configure_type,
                          QWidget* parent = 0, Qt::WindowFlags f = 0);

public:
  ConfigureType FirstConfigureType() const;
  ConfigureType LastConfigureType() const;
  void FetchFeatureMatchConfig(
    workflow::FeatureMatchConfig& feature_match_config);
  void FetchPhotoOrientationConfig(
    workflow::PhotoOrientationConfig& photo_orientation_config);
  void FetchPointCloudConfig(
    workflow::PointCloudConfig& point_cloud_config);
  void FetchSurfaceModelConfig(
    workflow::MeshSurfaceConfig& surface_model_config);

public slots:
  void SyncTreeNodeCheckState(QTreeWidgetItem* tree_item, int column);
  void DisplayWidget(QTreeWidgetItem* tree_item, QTreeWidgetItem* previouse);
  void GotoNextTreeNode();
  void GotoPreviousTreeNode();

private:
  FeatureMatchConfigureWidget* feature_match_configure_widget_;
  FeatureMatchExportWidget* feature_match_export_widget_;
  PhotoOrientationConfigureWidget* photo_orientation_configure_widget_;
  PhotoOrientationExportWidget* photo_orientation_export_widget_;
  PointCloudConfigureWidget* point_cloud_configure_widget_;
  PointCloudExportWidget* point_cloud_export_widget_;
  SurfaceModelConfigureWidget* surface_model_configure_widget_;
  SurfaceModelExportWidget* surface_model_export_widget_;
  TextureConfigureWidget* texture_configure_widget_;
  TextureExportWidget* texture_export_widget_;

  int start_configure_type_;
  QHBoxLayout *h_layout_main_;
  QVBoxLayout *v_layout_widget_;
  QHBoxLayout *h_layout_config_;
  QHBoxLayout *h_layout_export_;
  QHBoxLayout *h_layout_button_;
  QTreeWidget *tree_widget_;
  QTreeWidgetItem *tree_item_feature_match_;
  QTreeWidgetItem *tree_item_photo_orientation_;
  QTreeWidgetItem *tree_item_point_cloud_;
  QTreeWidgetItem *tree_item_surface_model_;
  QTreeWidgetItem *tree_item_texture_;

  QPushButton *button_next_;
  QPushButton *button_previous_;
};

}
}
}

#endif
