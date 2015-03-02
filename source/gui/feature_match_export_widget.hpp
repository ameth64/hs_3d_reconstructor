#ifndef _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_EXPORT_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_EXPORT_WIDGET_HPP_

#include <QWidget>

namespace hs
{
namespace recon
{
namespace gui
{

class FeatureMatchExportWidget : public QWidget
{
  Q_OBJECT
public:
  FeatureMatchExportWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
};

}
}
}

#endif
