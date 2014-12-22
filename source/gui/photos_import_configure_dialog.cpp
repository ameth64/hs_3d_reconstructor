#include <QMessageBox>

#include "gui/photos_import_configure_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotosImportConfigureDialog::PhotosImportConfigureDialog(QWidget* parent,
                                                         Qt::WindowFlags f)
  : QDialog(parent, f)
  , push_button_previous_(new QPushButton(tr("Previous"), this))
  , push_button_next_(new QPushButton(tr("Next"), this))
  , layout_all_(new QVBoxLayout(this))
  , container_(new QWidget(this))
  , layout_container_(new QVBoxLayout(container_))
  , layout_buttons_(new QHBoxLayout)
  , file_select_widget_(new PhotoFilesSelectWidget(container_))
  , photogroup_widget_(new PhotogroupInfoPOSWrapWidget(container_))
  , configure_state_(STATE_SELECT_PHOTO_FILES)
{
  layout_all_->addWidget(container_);
  layout_all_->addLayout(layout_buttons_);

  layout_container_->addWidget(file_select_widget_);

  layout_buttons_->addStretch();
  layout_buttons_->addWidget(push_button_previous_);
  layout_buttons_->addWidget(push_button_next_);

  push_button_previous_->setEnabled(false);
  push_button_next_->setEnabled(false);
  photogroup_widget_->hide();

  QObject::connect(
    push_button_previous_, &QPushButton::clicked,
    this, &PhotosImportConfigureDialog::OnPushButtonPreviousClicked);
  QObject::connect(
    push_button_next_, &QPushButton::clicked,
    this, &PhotosImportConfigureDialog::OnPushButtonNextClicked);
  QObject::connect(
    file_select_widget_, &PhotoFilesSelectWidget::PhotoFileEntriesChanged,
    this, &PhotosImportConfigureDialog::OnPhotoFileEntriesChanged);
}

PhotogroupInfo PhotosImportConfigureDialog::GetPhotogroupInfo()
{
  return photogroup_widget_->GetPhotogroupInfo();
}

PhotosImportConfigureDialog::POSEntryContainer
PhotosImportConfigureDialog::GetPOSEntries()
{
  return photogroup_widget_->GetPOSEntries();
}

void PhotosImportConfigureDialog::OnPushButtonPreviousClicked()
{
  switch (configure_state_)
  {
  case STATE_SELECT_PHOTO_FILES:
    {
      break;
    }
  case STATE_CONFIG_INFO_POS:
    {
      layout_container_->removeWidget(photogroup_widget_);
      photogroup_widget_->hide();
      layout_container_->addWidget(file_select_widget_);
      file_select_widget_->show();
      push_button_next_->setText(tr("Next"));
      push_button_previous_->setEnabled(false);
      configure_state_ = STATE_SELECT_PHOTO_FILES;
      break;
    }
  }
}

void PhotosImportConfigureDialog::OnPushButtonNextClicked()
{
  switch (configure_state_)
  {
  case STATE_SELECT_PHOTO_FILES:
    {
      auto photo_file_entries = file_select_widget_->GetPhotoFileEnties();
      bool all_the_same = true;
      auto itr_entry = photo_file_entries.begin();
      auto itr_entry_end = photo_file_entries.end();
      ++itr_entry;
      for (; itr_entry != itr_entry_end; ++itr_entry)
      {
        if ((*(photo_file_entries.begin())) != (*itr_entry))
        {
          all_the_same = false;
          break;
        }
      }
      if (!all_the_same)
      {
        QMessageBox msg_box;
        msg_box.setText(
          tr("Selected photos has no unique internal parameters."));
        msg_box.exec();
      }
      itr_entry = photo_file_entries.begin();
      PhotogroupInfo info;
      info.width = itr_entry->width;
      info.height = itr_entry->height;
      info.focal_length = double(itr_entry->focal_length);
      info.pixel_size_x = double(itr_entry->pixel_x_size);
      info.pixel_size_y = double(itr_entry->pixel_y_size);
      info.principal_x = double(itr_entry->width) * 0.5;
      info.principal_y = double(itr_entry->height) * 0.5;
      photogroup_widget_->SetFromPhotogroupInfo(info);

      layout_container_->removeWidget(file_select_widget_);
      file_select_widget_->hide();
      layout_container_->addWidget(photogroup_widget_);

      QStringList photo_file_paths = file_select_widget_->GetPhotoFilePaths();
      photogroup_widget_->SetPhotoPaths(photo_file_paths);
      photogroup_widget_->show();

      push_button_next_->setText(tr("Done"));
      push_button_previous_->setEnabled(true);
      configure_state_ = STATE_CONFIG_INFO_POS;
      break;
    }
  case STATE_CONFIG_INFO_POS:
    {
      accept();
      break;
    }
  }
}

void PhotosImportConfigureDialog::OnPhotoFileEntriesChanged()
{
  if (file_select_widget_->GetPhotoFilePaths().empty())
  {
    push_button_next_->setEnabled(false);
  }
  else
  {
    push_button_next_->setEnabled(true);
  }
}

}
}
}
