#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_CONFIGURE_WIDGET_HPP_

#include <QWidget>

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
    CONFIGURE_FEATURE_MATCH = 0,
    CONFIGURE_PHOTO_ORIENTATION,
    CONFIGURE_POINT_CLOUD,
    CONFIGURE_SURFACE_MODEL,
    CONFIGURE_TEXTURE
  };

  WorkflowConfigureWidget(ConfigureType start_configure_type,
                          QWidget* parent = 0, Qt::WindowFlags f = 0);

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
};

}
}
}

#endif
