#ifndef _HS_3D_RECONSTRUCTOR_GUI_GCP_CONSTRAINED_OPTIMIZATION_CONFIG_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_GCP_CONSTRAINED_OPTIMIZATION_CONFIG_WIDGET_HPP_

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace hs
{
namespace recon
{
namespace gui
{

class GCPConstrainedOptimizationConfigWidget : public QWidget
{
  Q_OBJECT
public:
  GCPConstrainedOptimizationConfigWidget(QWidget* parent = nullptr,
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
  QVBoxLayout* layout_all_;

  QHBoxLayout* layout_gcp_planar_accuracy_;
  QLabel* label_gcp_planar_accuracy_;
  QLineEdit* line_edit_gcp_planar_accuracy_;

  QHBoxLayout* layout_gcp_height_accuracy_;
  QLabel* label_gcp_height_accuracy_;
  QLineEdit* line_edit_gcp_height_accuracy_;

  QHBoxLayout* layout_tiepoint_feature_accuracy_;
  QLabel* label_tiepoint_feature_accuracy_;
  QLineEdit* line_edit_tiepoint_feature_accuracy_;

  QHBoxLayout* layout_gcp_marker_accuracy_;
  QLabel* label_gcp_marker_accuracy_;
  QLineEdit* line_edit_gcp_marker_accuracy_;
};

}
}
}

#endif