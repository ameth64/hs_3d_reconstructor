#include "gui/photogroup_info_pos_wrap_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotogroupInfoPOSWrapWidget::PhotogroupInfoPOSWrapWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  Initialize();
}

PhotogroupInfoPOSWrapWidget::PhotogroupInfoPOSWrapWidget(
  const PhotogroupInfo& info, const QStringList& photo_paths,
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  Initialize();
  SetFromPhotogroupInfo(info);
  SetPhotoPaths(photo_paths);
}

void PhotogroupInfoPOSWrapWidget::SetFromPhotogroupInfo(
  const PhotogroupInfo& info)
{
  info_widget_->SetFromPhotogroupInfo(info);
}

void PhotogroupInfoPOSWrapWidget::SetPhotoPaths(const QStringList& photo_paths)
{
  pos_widget_->SetPhotoPaths(photo_paths);
}

PhotogroupInfo PhotogroupInfoPOSWrapWidget::GetPhotogroupInfo()
{
  return info_widget_->GetPhotogroupInfo();
}

PhotogroupInfoPOSWrapWidget::POSEntryContainer
PhotogroupInfoPOSWrapWidget::GetPOSEntries()
{
  return pos_widget_->GetPOSEntries();
}

void PhotogroupInfoPOSWrapWidget::Initialize()
{
  info_widget_ = new PhotogroupInfoWidget(this);
  pos_widget_ = new PhotogroupPOSConfigureWidget(this);
  layout_all_ = new QHBoxLayout(this);
  layout_all_->addWidget(info_widget_, 1);
  layout_all_->addWidget(pos_widget_, 2);
}

}
}
}
