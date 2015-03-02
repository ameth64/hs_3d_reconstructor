#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTOGROUP_POS_CONFIGURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTOGROUP_POS_CONFIGURE_WIDGET_HPP_

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>

#include "hs_cartographics/cartographics_utility/coordinate_system.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotogroupPOSConfigureWidget : public QWidget
{
  Q_OBJECT
public:
  typedef std::map<QString, int> LabelRowMap;
  typedef hs::cartographics::HS_CoordinateSystem<double> CoordinateSystem;

  struct POSEntry
  {
    QString photo_path;
    double x;
    double y;
    double z;
    double pitch;
    double roll;
    double heading;
    QString coordinate_system;
  };
  typedef std::vector<POSEntry> POSEntryContainer;

public:
  PhotogroupPOSConfigureWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
  PhotogroupPOSConfigureWidget(const QStringList& photo_paths,
                               QWidget* parent = 0, Qt::WindowFlags f = 0);

  void SetPhotoPaths(const QStringList& photo_paths);

  POSEntryContainer GetPOSEntries();

private:
  void Initialize();
  void DoPosStatistic(double& x_mean, double& y_mean,
                      double& x_stddev, double& y_stddev);

private slots:
  void OnPushButtonConfigCoordinateSystemClicked();
  void OnPushButtonImportClicked();
  void OnPushButtonClearClicked();

private:
  QVBoxLayout* layout_all_;

  QHBoxLayout* layout_header_;
  QPushButton* push_button_import_;
  QPushButton* push_button_clear_;
  QPushButton* push_button_config_coordinate_system_;
  QTableWidget* table_pos_;

  QIcon photo_icon_;
  LabelRowMap label_row_map_;

  CoordinateSystem coordinate_system_;

private:
  static QStringList HeaderLabels();
};

}
}
}

#endif
