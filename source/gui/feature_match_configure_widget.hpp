#ifndef _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>

#include "workflow/feature_match/feature_match_config.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class FeatureMatchConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  FeatureMatchConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

  void FetchFeatureMatchConfig(
    workflow::FeatureMatchConfig& feature_match_config);


private:
  QVBoxLayout* layout_all_;
  QVBoxLayout* layout_inbox_;
  QGroupBox* group_box_;
  QLabel* label_feature_point_limits_;
  QLineEdit* line_edit_feature_point_limits_;
  QHBoxLayout* layout_feature_point_limits_;
};

}
}
}

#endif
