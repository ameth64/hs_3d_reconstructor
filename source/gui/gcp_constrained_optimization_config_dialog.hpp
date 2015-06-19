#ifndef _HS_3D_RECONSTRUCTOR_GUI_GCP_CONSTRAINED_OPTIMIZATION_CONFIG_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_GCP_CONSTRAINED_OPTIMIZATION_CONFIG_DIALOG_HPP_

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "gui/gcp_constrained_optimization_config_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class GCPConstrainedOptimizationConfigDialog : public QDialog
{
  Q_OBJECT

public:
  GCPConstrainedOptimizationConfigDialog(QWidget* parent = nullptr,
                                         Qt::WindowFlags f = 0,
                                         double gcp_planar_accuracy = 0.005,
                                         double gcp_height_accuracy = 0.01,
                                         double tiepoint_feature_accuracy = 0.1,
                                         double gcp_marker_accuracy = 4.0);

  double GetGCPPlanarAccuracy() const;
  double GetGCPHeightAccuracy() const;
  double GetTiepointFeatureAccuracy() const;
  double GetGCPMarkerAccuracy() const;

private:
  GCPConstrainedOptimizationConfigWidget* widget_;
  QDialogButtonBox* button_box_;
  QVBoxLayout* layout_;
};

}
}
}

#endif