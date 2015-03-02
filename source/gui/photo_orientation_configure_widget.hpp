#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "workflow/photo_orientation/incremental_photo_orientation.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotoOrientationConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  PhotoOrientationConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

  void FetchPhotoOrientationConfig(
    workflow::PhotoOrientationConfig& photo_orientation_config);

private:
  QVBoxLayout* layout_all_;
  QVBoxLayout* layout_inbox_;
  QGroupBox* group_box_;
};

}
}
}

#endif
