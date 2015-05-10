#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

#include "hs_cartographics/cartographics_utility/coordinate_system.hpp"

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
  typedef hs::cartographics::HS_CoordinateSystem<double> CoordinateSystem;

  PhotoOrientationConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

  void FetchPhotoOrientationConfig(
    workflow::PhotoOrientationConfig& photo_orientation_config);

private slots:
  void OnButtonConfigCoordinateSystemClicked();

private:
  QVBoxLayout* layout_all_;
  QVBoxLayout* layout_inbox_;
  QGroupBox* group_box_;
  QPushButton* push_button_config_coordinate_system_;

  CoordinateSystem coordinate_system_;
};

}
}
}

#endif
