#ifndef _HS_3D_RECONSTRUCTOR_PHOTOGROUP_INFO_SETTING_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTOGROUP_INFO_SETTING_WIDGET_HPP_

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "gui/photogroup_info_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotogroupInfoSettingWidget : public QWidget
{
  Q_OBJECT
public:
  PhotogroupInfoSettingWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

  void SetFromPhotogroupInfo(uint id, const PhotogroupInfo& info);

private slots:
  void OnPushButtonRestClicked();
  void OnPushButtonUpdateClicked();

signals:
  void PhotogroupInfoUpdated(uint id, const PhotogroupInfo& photogroup_info);

private:
  PhotogroupInfoWidget* info_widget_;
  QHBoxLayout* layout_buttons_;
  QVBoxLayout* layout_all_;
  QPushButton* push_button_reset_;
  QPushButton* push_button_update_;

  PhotogroupInfo info_;
  uint id_;
};

}
}
}

#endif
