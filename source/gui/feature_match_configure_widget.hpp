#ifndef _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_CONFIGURE_WIDGET_HPP_

#include <QWidget>

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
};

}
}
}

#endif
