#include <QFileInfo>

#include "gui/blocks_pane.hpp"
#include "gui/block_photos_select_dialog.hpp"
#include "gui/main_window.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlocksPane::BlocksPane(QWidget* parent)
  : ManagerPane(tr("Blocks"), parent)
  , blocks_tree_widget_(new BlocksTreeWidget(this))
  , icon_add_block_(":/images/icon_block_add.png")
  , icon_remove_block_(":/images/icon_block_remove.png")
  , icon_remove_photos_(":/images/icon_photo_remove.png")
  , icon_add_workflow_(":/images/icon_workflow_add.png")
  , selected_block_id_(std::numeric_limits<uint>::max())
  , selected_feature_match_id_(std::numeric_limits<uint>::max())
  , selected_photo_orientation_id_(std::numeric_limits<uint>::max())
  , selected_point_cloud_id_(std::numeric_limits<uint>::max())
  , selected_surface_model_id_(std::numeric_limits<uint>::max())
  , selected_dem_id_(std::numeric_limits<uint>::max())
  , selected_texture_id_(std::numeric_limits<uint>::max())
  , selected_dom_id_(std::numeric_limits<uint>::max())
{
  AddWidget(blocks_tree_widget_);

  action_add_block_ = new QAction(icon_add_block_,
                                  tr("Add Block"), this);
  action_remove_block_ = new QAction(icon_remove_block_,
                                     tr("Remove Block"), this);
  action_remove_block_->setEnabled(false);
  action_remove_photos_ = new QAction(icon_remove_photos_,
                                      tr("Remove Photos"), this);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_ = new QAction(icon_add_workflow_,
                                     tr("Add Workflow"), this);
  action_add_workflow_->setEnabled(false);

  tool_bar_ = new QToolBar(this);
  tool_bar_->addAction(action_add_block_);
  tool_bar_->addAction(action_remove_block_);
  tool_bar_->addAction(action_remove_photos_);
  tool_bar_->addAction(action_add_workflow_);

  main_window_->addToolBar(tool_bar_);

  QObject::connect(action_add_block_, &QAction::triggered,
                   this, &BlocksPane::OnActionAddBlockTriggered);

}

void BlocksPane::Response(int request_flag, void* response)
{

}

void BlocksPane::OnActionAddBlockTriggered()
{
  typedef BlockPhotosSelectDialog::GroupEntryContainer GroupEntryContainer;

  db::RequestGetAllPhotogroups photogroups_request;
  db::ResponseGetAllPhotogroups photogroups_response;
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_ALL_PHOTOGROUPS,
    photogroups_request, photogroups_response, false);
  if (photogroups_response.error_code != db::DatabaseMediator::NO_ERROR)
    return;

  db::RequestGetAllPhotos photos_request;
  db::ResponseGetAllPhotos photos_response;
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_ALL_PHOTOS,
    photos_request, photos_response, false);
  if (photos_response.error_code != db::DatabaseMediator::NO_ERROR)
    return;

  GroupEntryContainer group_entries;
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

  BlockPhotosSelectDialog dialog(QString(), QString(), group_entries);
  if (dialog.exec())
  {

  }
}

void BlocksPane::OnBlockItemSelected(uint block_id)
{
  action_remove_block_->setEnabled(true);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_block_id_ = block_id;
}

void BlocksPane::OnPhotosInOneBlockSelected(uint block_id,
                                            const std::vector<uint>& photo_ids)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(true);
  action_add_workflow_->setEnabled(false);
  selected_block_id_ = block_id;
}

void BlocksPane::OnPhotosItemSelected(uint block_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(false);
  selected_block_id_ = block_id;
}

void BlocksPane::OnFeatureMatchItemSelected(uint feature_match_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_feature_match_id_ = feature_match_id;
}

void BlocksPane::OnPhotoOrientationItemSelected(uint photo_orientation_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_photo_orientation_id_ = photo_orientation_id;
}

void BlocksPane::OnPointCloudItemSelected(uint point_cloud_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_point_cloud_id_ = point_cloud_id;
}

void BlocksPane::OnSurfaceModelItemSelected(uint surface_model_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_surface_model_id_ = surface_model_id;
}

void BlocksPane::OnDEMItemSelected(uint dem_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_dem_id_ = dem_id;
}

void BlocksPane::OnTextureItemSelected(uint texture_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_texture_id_ = texture_id;
}

void BlocksPane::OnDOMItemSelected(uint dom_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_dom_id_ = dom_id;
}

}
}
}
