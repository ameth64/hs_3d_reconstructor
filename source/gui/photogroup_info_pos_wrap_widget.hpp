#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTOGROUP_INFO_POS_WRAP_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTOGROUP_INFO_POS_WRAP_WIDGET_HPP_

#include <QHBoxLayout>
#include <QWidget>

#include "gui/photogroup_info_widget.hpp"
#include "gui/photogroup_pos_configure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotogroupInfoPOSWrapWidget : public QWidget
{
  Q_OBJECT
  typedef PhotogroupPOSConfigureWidget::POSEntry POSEntry;
  typedef PhotogroupPOSConfigureWidget::POSEntryContainer POSEntryContainer;
public:
  PhotogroupInfoPOSWrapWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
  PhotogroupInfoPOSWrapWidget(const PhotogroupInfo& info,
                              const QStringList& photo_paths,
                              QWidget* parent = 0, Qt::WindowFlags f = 0);

  void SetFromPhotogroupInfo(const PhotogroupInfo& info);

  void SetPhotoPaths(const QStringList& photo_paths);

  PhotogroupInfo GetPhotogroupInfo();

  POSEntryContainer GetPOSEntries();

private:
  void Initialize();

private:
  PhotogroupInfoWidget* info_widget_;
  PhotogroupPOSConfigureWidget* pos_widget_;
  QHBoxLayout* layout_all_;
};

}
}
}

#endif
