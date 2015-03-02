#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_EXPORT_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_EXPORT_WIDGET_HPP_

#include <QWidget>

namespace hs
{
namespace recon
{
namespace gui
{

class PhotoOrientationExportWidget : public QWidget
{
  Q_OBJECT
public:
  PhotoOrientationExportWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
};

}
}
}

#endif
