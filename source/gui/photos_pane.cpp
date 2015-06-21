#include <QToolBar>
#include <QFileInfo>
#include <thread>

#include "gui/photos_pane.hpp"
#include "gui/main_window.hpp"

#include "gui/progress_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotosPane::PhotosPane(QWidget* parent)
  : ManagerPane(tr("Photos"), parent)
  , photos_tree_widget_(new PhotosTreeWidget(this))
  , photogroup_info_widget_(new PhotogroupInfoSettingWidget(this))
  , photo_display_widget_(new PhotoDisplayWidget(new ImageOpenGLWindow, this))
  , icon_add_photogroup_(":/images/icon_group_add.png")
  //, icon_add_photos_(":/images/icon_photo_add.png")
  , icon_remove_photogroup_(":/images/icon_group_remove.png")
  , icon_remove_photos_(":/images/icon_photo_remove.png")
  , is_import_complete_(false)
{
  timer_import_photos_ = new QTimer;

  AddWidget(photos_tree_widget_);
  photogroup_info_widget_->hide();
  photo_display_widget_->hide();

  action_add_photogroup_ = new QAction(icon_add_photogroup_,
                                       tr("Add Photogroup"), this);
  //action_add_photos_ = new QAction(icon_add_photos_,
  //                                tr("Add Photos"), this);
  //action_add_photos_->setEnabled(false);
  action_remove_photogroup_ = new QAction(icon_remove_photogroup_,
                                          tr("Remove Photogroup"), this);
  action_remove_photogroup_->setEnabled(false);
  action_remove_photos_ = new QAction(icon_remove_photos_,
                                      tr("Remove Photos"), this);
  action_remove_photos_->setEnabled(false);

  tool_bar_ = new QToolBar(this);
  tool_bar_->addAction(action_add_photogroup_);
  //tool_bar_->addAction(action_add_photos_);
  //tool_bar_->addAction(action_remove_photogroup_);
  //tool_bar_->addAction(action_remove_photos_);

  main_window_->addToolBar(tool_bar_);

  QObject::connect(action_add_photogroup_, &QAction::triggered,
                   this, &PhotosPane::OnActionAddPhotogroupTriggered);
  //QObject::connect(action_add_photos_, &QAction::triggered,
  //                 this, &PhotosPane::OnActionAddPhotosTriggered);
  QObject::connect(action_remove_photogroup_, &QAction::triggered,
                   this, &PhotosPane::OnActionRemovePhotogroupTriggered);
  QObject::connect(action_remove_photos_, &QAction::triggered,
                   this, &PhotosPane::OnActionRemovePhotosTriggered);

  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::PhotoSelected,
    this, &PhotosPane::OnPhotoSelected);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::GroupsOnlySelected,
    this, &PhotosPane::OnGroupsOnlySelected);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::SingleGroupSelected,
    this, &PhotosPane::OnSingleGroupSelected);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::PhotosOnlySelected,
    this, &PhotosPane::OnPhotosOnlySelected);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::NothingSelected,
    this, &PhotosPane::OnNothingSelected);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::PhotosAndGroupsSelected,
    this, &PhotosPane::OnPhotosAndGroupsSelected);

  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::SelectedGroupsDeleted,
    this, &PhotosPane::OnSelectedGroupsDeleted);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::SelectedPhotosRemoved,
    this, &PhotosPane::OnSelectedPhotosRemoved);

  QObject::connect(
    photogroup_info_widget_,
    &PhotogroupInfoSettingWidget::PhotogroupInfoUpdated,
    this,
    &PhotosPane::OnPhotogroupInfoUpdated);

  QObject::connect(
    timer_import_photos_, &QTimer::timeout,
    this, &PhotosPane::OnImportTimeout);

  //progress_import_photos_ = new ProgressDialog;
  //progress_import_photos_->SetRange(0, 100);
  //timer_ = new QTimer(this);
  //QObject::connect(timer_, &QTimer::timeout,
  //  this, &PhotosPane::SetProgress);

}

void PhotosPane::Response(int request_flag, void* response)
{
  switch (request_flag)
  {
  case db::DatabaseMediator::REQUEST_OPEN_DATABASE:
    {
      while (1)
      {
        db::ResponseOpenDatabase* response_open =
          static_cast<db::ResponseOpenDatabase*>(response);
        if (response_open->error_code == db::DatabaseMediator::NO_ERROR)
        {
          db::RequestGetAllPhotogroups photogroups_request;
          db::ResponseGetAllPhotogroups photogroups_response;
          ((MainWindow*)parent())->database_mediator().Request(
            this, db::DatabaseMediator::REQUEST_GET_ALL_PHOTOGROUPS,
            photogroups_request, photogroups_response, false);
          if (photogroups_response.error_code != db::DatabaseMediator::NO_ERROR)
            break;

          db::RequestGetAllPhotos photos_request;
          db::ResponseGetAllPhotos photos_response;
          ((MainWindow*)parent())->database_mediator().Request(
            this, db::DatabaseMediator::REQUEST_GET_ALL_PHOTOS,
            photos_request, photos_response, false);
          if (photos_response.error_code != db::DatabaseMediator::NO_ERROR)
            break;

          std::map<uint, PhotosTreeWidget::GroupEntry> group_entries;
          auto itr_group_record = photogroups_response.records.begin();
          auto itr_group_record_end = photogroups_response.records.end();
          for (; itr_group_record != itr_group_record_end; ++itr_group_record)
          {
            PhotosTreeWidget::GroupEntry group_entry;
            group_entry.id = uint(itr_group_record->first);
            std::string name =
              itr_group_record->second[
                db::PhotogroupResource::PHOTOGROUP_FIELD_NAME].ToString();
            group_entry.name = QString::fromLocal8Bit(name.c_str());
            group_entries[group_entry.id] = group_entry;
          }

          auto itr_photo_record = photos_response.records.begin();
          auto itr_photo_record_end = photos_response.records.end();
          for (; itr_photo_record != itr_photo_record_end; ++itr_photo_record)
          {
            uint group_id =
              uint(itr_photo_record->second[
                db::PhotoResource::PHOTO_FIELD_PHOTOGROUP_ID].ToInt());
            auto itr_group_entry = group_entries.find(group_id);
            if (itr_group_entry == group_entries.end()) continue;

            PhotosTreeWidget::PhotoEntry photo_entry;
            photo_entry.id = uint(itr_photo_record->first);
            std::string photo_path =
              itr_photo_record->second[
                db::PhotoResource::PHOTO_FIELD_PATH].ToString();
            QFileInfo file_info(QString::fromLocal8Bit(photo_path.c_str()));
            photo_entry.file_name = file_info.fileName();
            photo_entry.x =
              PhotosTreeWidget::Float(
                itr_photo_record->second[
                  db::PhotoResource::PHOTO_FIELD_POS_X].ToFloat());
            photo_entry.y =
              PhotosTreeWidget::Float(
                itr_photo_record->second[
                  db::PhotoResource::PHOTO_FIELD_POS_Y].ToFloat());
            photo_entry.z =
              PhotosTreeWidget::Float(
                itr_photo_record->second[
                  db::PhotoResource::PHOTO_FIELD_POS_Z].ToFloat());
            photo_entry.pitch =
              PhotosTreeWidget::Float(
                itr_photo_record->second[
                  db::PhotoResource::PHOTO_FIELD_PITCH].ToFloat());
            photo_entry.roll =
              PhotosTreeWidget::Float(
                itr_photo_record->second[
                  db::PhotoResource::PHOTO_FIELD_ROLL].ToFloat());
            photo_entry.heading =
              PhotosTreeWidget::Float(
                itr_photo_record->second[
                  db::PhotoResource::PHOTO_FIELD_HEADING].ToFloat());
            itr_group_entry->second.photos[photo_entry.id] = photo_entry;
          }

          auto itr_group_entry = group_entries.begin();
          auto itr_group_entry_end = group_entries.end();
          for (; itr_group_entry != itr_group_entry_end; ++itr_group_entry)
          {
            photos_tree_widget_->AddGroup(itr_group_entry->first,
                                          itr_group_entry->second);
          }
          for (int column = 0; column < photos_tree_widget_->columnCount();
               column++)
          {
            photos_tree_widget_->resizeColumnToContents(column);
          }
        }

        break;
      }
      break;
    }
  case db::DatabaseMediator::REQUEST_CLOSE_DATABASE:
    {
      db::ResponseCloseDatabase* response_close =
        static_cast<db::ResponseCloseDatabase*>(response);
      if (response_close->error_code == db::DatabaseMediator::NO_ERROR)
      {
        photos_tree_widget_->ClearGroups();
      }
      break;
    }
  }
}

void PhotosPane::OnActionAddPhotogroupTriggered()
{
  PhotosImportConfigureDialog dialog;
  dialog.resize(1300, 800);
  dialog.setWindowTitle(tr("Import Photos"));
  if (dialog.exec())
  {
    PhotogroupInfo photogroup_info = dialog.GetPhotogroupInfo();
    PhotogroupPOSConfigureWidget::POSEntryContainer pos_entries =
      dialog.GetPOSEntries();

    is_import_complete_ = false;
    ProgressDialog progress_dialog;
    progress_dialog.Start(&PhotosPane::ImportPhotos,
                          this, photogroup_info, pos_entries,
                          progress_dialog.GetProgressManagerPtr());
    timer_import_photos_->start(100);
  }
}

void PhotosPane::OnActionAddPhotosTriggered()
{

}

void PhotosPane::OnActionRemovePhotogroupTriggered()
{
  photos_tree_widget_->DeleteGroupsBySelectedItems();
}

void PhotosPane::OnActionRemovePhotosTriggered()
{
  photos_tree_widget_->RemovePhotosBySelectedItems();
}

void PhotosPane::OnPhotoSelected(uint photo_id)
{
  //action_add_photos_->setEnabled(false);
  action_remove_photogroup_->setEnabled(false);
  action_remove_photos_->setEnabled(true);

  RemoveWidget(photogroup_info_widget_);

  db::RequestGetPhoto request;
  db::ResponseGetPhoto response;
  request.id = db::Database::Identifier(photo_id);
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_PHOTO,
    request, response);
  if (response.error_code == db::DatabaseMediator::NO_ERROR)
  {
    std::string photo_path =
      response.record[db::PhotoResource::PHOTO_FIELD_PATH].ToString();
    QString origin_image_path = QString::fromLocal8Bit(photo_path.c_str());

    std::string thumbnail_path =
      ((MainWindow*)parent())->database_mediator().GetThumbnailPath(
        request.id);
    hs::imgio::whole::ImageData thumbnail_image_data;
    hs::imgio::whole::ImageIO image_io;
    if (image_io.LoadImage(thumbnail_path, thumbnail_image_data) != 0)
    {
      thumbnail_image_data.CreateImage(10, 10, 3);
      for (int row = 0; row < 10; row++)
      {
        for (int col = 0; col < 10; col++)
        {
          for (int channel = 0; channel < 3; channel++)
          {
            thumbnail_image_data.GetByte(row, col, channel) =
              hs::imgio::whole::ImageData::Byte(255);
          }
        }
      }
    }

    QFileInfo file_info(origin_image_path);
    photo_display_widget_->SetComment(file_info.fileName());
    photo_display_widget_->ResetImage(thumbnail_image_data, origin_image_path);
    photo_display_widget_->show();
    AddWidget(photo_display_widget_);
  }
}

void PhotosPane::OnGroupsOnlySelected(const std::vector<uint>& group_ids)
{
  //action_add_photos_->setEnabled(false);
  action_remove_photogroup_->setEnabled(true);
  action_remove_photos_->setEnabled(false);

  RemoveWidget(photogroup_info_widget_);
  RemoveWidget(photo_display_widget_);
}
void PhotosPane::OnSingleGroupSelected(uint group_id)
{
  //action_add_photos_->setEnabled(true);
  action_remove_photogroup_->setEnabled(true);
  action_remove_photos_->setEnabled(false);

  RemoveWidget(photo_display_widget_);

  db::RequestGetPhotogroup request;
  db::ResponseGetPhotogroup response;
  request.id = db::Database::Identifier(group_id);
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_PHOTOGROUP,
    request, response);
  if (response.error_code == db::DatabaseMediator::NO_ERROR)
  {
    PhotogroupInfo info;
    std::string name =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_NAME].ToString();
    info.name = QString::fromLocal8Bit(name.c_str());
    std::string description =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_DESCRIPTION].ToString();
    info.description = QString::fromLocal8Bit(description.c_str());
    info.width =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_WIDTH].ToInt();
    info.height =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_HEIGHT].ToInt();
    info.focal_length =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_FOCAL_LENGTH].ToFloat();
    info.pixel_size_x =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_X_SIZE].ToFloat();
    info.pixel_size_y =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_Y_SIZE].ToFloat();
    info.principal_x =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_X].ToFloat();
    info.principal_y =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_Y].ToFloat();
    info.skew =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_SKEW].ToFloat();
    info.radial1 =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL1].ToFloat();
    info.radial2 =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL2].ToFloat();
    info.radial3 =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL3].ToFloat();
    info.decentering1 =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING1].ToFloat();
    info.decentering2 =
      response.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING2].ToFloat();

    photogroup_info_widget_->SetFromPhotogroupInfo(group_id, info);
    photogroup_info_widget_->show();
    AddWidget(photogroup_info_widget_);
  }
}
void PhotosPane::OnPhotosOnlySelected(const std::vector<uint>& photo_ids)
{
  //action_add_photos_->setEnabled(false);
  action_remove_photogroup_->setEnabled(false);
  action_remove_photos_->setEnabled(true);

  RemoveWidget(photogroup_info_widget_);
  RemoveWidget(photo_display_widget_);
}
void PhotosPane::OnNothingSelected()
{
  //action_add_photos_->setEnabled(false);
  action_remove_photogroup_->setEnabled(false);
  action_remove_photos_->setEnabled(false);

  RemoveWidget(photogroup_info_widget_);
  RemoveWidget(photo_display_widget_);
}
void PhotosPane::OnPhotosAndGroupsSelected()
{
  //action_add_photos_->setEnabled(false);
  action_remove_photogroup_->setEnabled(false);
  action_remove_photos_->setEnabled(false);

  RemoveWidget(photogroup_info_widget_);
  RemoveWidget(photo_display_widget_);
}

void PhotosPane::OnSelectedGroupsDeleted(const std::vector<uint>& group_ids)
{
  db::RequestDeletePhotogroups request;
  db::ResponseDeletePhotogroups response;
  auto itr_group_id = group_ids.begin();
  auto itr_group_id_end = group_ids.end();
  for (; itr_group_id != itr_group_id_end; ++itr_group_id)
  {
    request.ids.push_back(db::Database::Identifier(*itr_group_id));
  }
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_DELETE_PHOTOGROUPS,
    request, response, true);
}
void PhotosPane::OnSelectedPhotosRemoved(const std::vector<uint>& photo_ids)
{
  db::RequestRemovePhotos request;
  db::ResponseRemovePhotos response;
  auto itr_photo_id = photo_ids.begin();
  auto itr_photo_id_end = photo_ids.end();
  for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
  {
    request.ids.push_back(db::Database::Identifier(*itr_photo_id));
  }
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_REMOVE_PHOTOS,
    request, response, true);
}

void PhotosPane::OnPhotogroupInfoUpdated(uint id, const PhotogroupInfo& info)
{
  db::RequestUpdatePhotogroups request;
  db::ResponseUpdatePhotogroups response;

  db::PhotogroupResource::UpdateRequest update_request;
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_NAME] =
    db::Value(std::string(info.name.toLocal8Bit().data()));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_DESCRIPTION] =
    db::Value(std::string(info.description.toLocal8Bit().data()));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_WIDTH] =
    db::Value(info.width);
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_HEIGHT] =
    db::Value(info.height);
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_FOCAL_LENGTH] =
    db::Value(db::Float(info.focal_length));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_X_SIZE] =
    db::Value(db::Float(info.pixel_size_x));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_Y_SIZE] =
    db::Value(db::Float(info.pixel_size_y));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_X] =
    db::Value(db::Float(info.principal_x));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_Y] =
    db::Value(db::Float(info.principal_y));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_SKEW] =
    db::Value(db::Float(info.skew));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL1] =
    db::Value(db::Float(info.radial1));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL2] =
    db::Value(db::Float(info.radial2));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL3] =
    db::Value(db::Float(info.radial3));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING1] =
    db::Value(db::Float(info.decentering1));
  update_request[db::PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING2] =
    db::Value(db::Float(info.decentering2));
  request.update_requests[db::Database::Identifier(id)] = update_request;

  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_UPDATE_PHOTOGROUPS,
    request, response, true);
}

void PhotosPane::OnImportTimeout()
{
  if (is_import_complete_)
  {
    if (imported_group_entry_.id != std::numeric_limits<uint>::max())
    {
      photos_tree_widget_->AddGroup(imported_group_entry_.id,
                                    imported_group_entry_);
      for (int column = 0; column < photos_tree_widget_->columnCount();
        column++)
      {
        photos_tree_widget_->resizeColumnToContents(column);
      }
    }

    if (timer_import_photos_->isActive())
    {
      timer_import_photos_->stop();
    }
  }
}

//void PhotosPane::SetProgress()
//{
//    if (progress_manager_->CheckKeepWorking())
//    {
//      int progress = int(progress_manager_->GetCompleteRatio() * 100);
//      progress_import_photos_->SetStatus(progress);
//    }
//}

void PhotosPane::ImportPhotos(const PhotogroupInfo &photogroup_info
  , const PhotogroupPOSConfigureWidget::POSEntryContainer &pos_entries,
  hs::progress::ProgressManager* progress_manager)
{
  imported_group_entry_.id = std::numeric_limits<uint>::max();

  hs::recon::db::RequestAddPhotogroup request;
  hs::recon::db::ResponseAddPhotogroup response;
  request.name = photogroup_info.name.toLocal8Bit().data();
  request.description = photogroup_info.description.toLocal8Bit().data();
  request.width = photogroup_info.width;
  request.height = photogroup_info.height;
  request.focal_length = photogroup_info.focal_length;
  request.pixel_x_size = photogroup_info.pixel_size_x;
  request.pixel_y_size = photogroup_info.pixel_size_y;
  request.principal_x = photogroup_info.principal_x;
  request.principal_y = photogroup_info.principal_y;
  request.skew = photogroup_info.skew;
  request.radial1 = photogroup_info.radial1;
  request.radial2 = photogroup_info.radial2;
  request.radial3 = photogroup_info.radial2;
  request.decentering1 = photogroup_info.decentering1;
  request.decentering2 = photogroup_info.decentering2;

  ((MainWindow*)parent())->database_mediator().Request(
    this, hs::recon::db::DatabaseMediator::REQUEST_ADD_PHOTOGROUP,
    request, response, true);
  if (response.error_code == db::DatabaseMediator::NO_ERROR)
  {
    hs::recon::db::RequestAddPhotos request_add_photos;
    hs::recon::db::ResponseAddPhotos response_add_photos;
    request_add_photos.progress_manager = progress_manager;
    //progress_manager_ = &response_add_photos.progress_manager;
    //PhotogroupPOSConfigureWidget::POSEntryContainer pos_entries =
    //  dialog.GetPOSEntries();
    auto itr_entry = pos_entries.begin();
    auto itr_entry_end = pos_entries.end();
    for (; itr_entry != itr_entry_end; ++itr_entry)
    {
      db::RequestAddPhotos::PhotoEntry photo_entry;
      photo_entry.photogroup_id = int(response.added_id);
      photo_entry.path = itr_entry->photo_path.toLocal8Bit().data();
      photo_entry.pos_x = itr_entry->x;
      photo_entry.pos_y = itr_entry->y;
      photo_entry.pos_z = itr_entry->z;
      photo_entry.pitch = itr_entry->pitch;
      photo_entry.roll = itr_entry->roll;
      photo_entry.heading = itr_entry->heading;
      photo_entry.coordinate_system =
        itr_entry->coordinate_system.toLocal8Bit().data();
      request_add_photos.photos.push_back(photo_entry);
    }
//    progress_import_photos_->show();
    ((MainWindow*)parent())->database_mediator().Request(
      this, hs::recon::db::DatabaseMediator::REQUEST_ADD_PHOTOS,
      request_add_photos, response_add_photos, true);

    //progress_manager_ = new hs::progress::ProgressManager;

    if (response_add_photos.error_code == db::DatabaseMediator::NO_ERROR)
    {
      imported_group_entry_.id = uint(response.added_id);
      imported_group_entry_.name = photogroup_info.name;
      imported_group_entry_.photos.clear();
      auto itr_added_photo = response_add_photos.added_photos.begin();
      auto itr_added_photo_end = response_add_photos.added_photos.end();
      for (; itr_added_photo != itr_added_photo_end; ++itr_added_photo)
      {
        PhotosTreeWidget::PhotoEntry photo_entry;
        photo_entry.id = uint(itr_added_photo->first);
        QFileInfo file_info(
          QString::fromLocal8Bit(itr_added_photo->second.path.c_str()));
        photo_entry.file_name = file_info.fileName();
        photo_entry.x =
          PhotosTreeWidget::Float(itr_added_photo->second.pos_x);
        photo_entry.y =
          PhotosTreeWidget::Float(itr_added_photo->second.pos_y);
        photo_entry.z =
          PhotosTreeWidget::Float(itr_added_photo->second.pos_z);
        photo_entry.pitch =
          PhotosTreeWidget::Float(itr_added_photo->second.pitch);
        photo_entry.roll =
          PhotosTreeWidget::Float(itr_added_photo->second.roll);
        photo_entry.heading =
          PhotosTreeWidget::Float(itr_added_photo->second.heading);
        imported_group_entry_.photos[photo_entry.id] = photo_entry;
      }
    }
  }

  is_import_complete_ = true;
}

}
}
}
