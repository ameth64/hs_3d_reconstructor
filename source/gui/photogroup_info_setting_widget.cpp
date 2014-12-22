#include "gui/photogroup_info_setting_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotogroupInfoSettingWidget::PhotogroupInfoSettingWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f), id_(0)
{
  info_widget_ = new PhotogroupInfoWidget(this);
  layout_buttons_ = new QHBoxLayout;
  layout_all_ = new QVBoxLayout(this);
  push_button_reset_ = new QPushButton(tr("Reset"), this);
  push_button_update_ = new QPushButton(tr("Update"), this);

  layout_buttons_->addStretch();
  layout_buttons_->addWidget(push_button_reset_);
  layout_buttons_->addWidget(push_button_update_);

  layout_all_->addWidget(info_widget_);
  layout_all_->addLayout(layout_buttons_);

  QObject::connect(
    push_button_reset_, &QPushButton::clicked,
    this, &PhotogroupInfoSettingWidget::OnPushButtonRestClicked);
  QObject::connect(
    push_button_update_, &QPushButton::clicked,
    this, &PhotogroupInfoSettingWidget::OnPushButtonUpdateClicked);
}

void PhotogroupInfoSettingWidget::SetFromPhotogroupInfo(
  uint id, const PhotogroupInfo& info)
{
  info_ = info;
  id_ = id;
  info_widget_->SetFromPhotogroupInfo(info);
}

void PhotogroupInfoSettingWidget::OnPushButtonRestClicked()
{
  info_widget_->SetFromPhotogroupInfo(info_);
}

void PhotogroupInfoSettingWidget::OnPushButtonUpdateClicked()
{
  info_ = info_widget_->GetPhotogroupInfo();
  emit PhotogroupInfoUpdated(id_, info_);
}

}
}
}
