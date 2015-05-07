#include <iostream>
#include <set>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <QFileInfo>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>

#include "gui/blocks_pane.hpp"
#include "gui/block_photos_select_dialog.hpp"
#include "gui/main_window.hpp"
#include "gui/workflow_configure_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlocksPane::BlocksPane(QWidget* parent)
  : ManagerPane(tr("Blocks"), parent)
  , icon_add_block_(":/images/icon_block_add.png")
  , icon_remove_block_(":/images/icon_block_remove.png")
  , icon_remove_photos_(":/images/icon_photo_remove.png")
  , icon_add_workflow_(":/images/icon_workflow_add.png")
  , selected_block_id_(std::numeric_limits<uint>::max())
  , selected_feature_match_id_(std::numeric_limits<uint>::max())
  , selected_photo_orientation_id_(std::numeric_limits<uint>::max())
  , selected_point_cloud_id_(std::numeric_limits<uint>::max())
  , selected_surface_model_id_(std::numeric_limits<uint>::max())
  , selected_texture_id_(std::numeric_limits<uint>::max())
  , activated_photo_orientation_id_(std::numeric_limits<uint>::max())
  , activated_point_cloud_id_(std::numeric_limits<uint>::max())
{
  timer_ = new QTimer(this);
  blocks_tree_widget_ = new BlocksTreeWidget(this);
  QTreeWidgetItem* header_item =
    new QTreeWidgetItem(QStringList()<<tr("Name")<<tr("Progress"));
  blocks_tree_widget_->setHeaderItem(header_item);
  AddWidget(blocks_tree_widget_);

  progress_bar_ = new QProgressBar(this);
  progress_bar_->hide();

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
  QObject::connect(action_add_workflow_, &QAction::triggered,
                   this, &BlocksPane::OnActionAddWorkflowTriggered);
  QObject::connect(timer_, &QTimer::timeout, this, &BlocksPane::OnTimeout);
  QObject::connect(blocks_tree_widget_, &QTreeWidget::itemDoubleClicked,
                   this, &BlocksPane::OnItemDoubleClicked);

  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::BlockItemSelected,
    this, &BlocksPane::OnBlockItemSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::PhotosInOneBlockSelected,
    this, &BlocksPane::OnPhotosInOneBlockSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::PhotosItemSelected,
    this, &BlocksPane::OnPhotosItemSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::FeatureMatchItemSelected,
    this, &BlocksPane::OnFeatureMatchItemSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::PhotoOrientationItemSelected,
    this, &BlocksPane::OnPhotoOrientationItemSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::PointCloudItemSelected,
    this, &BlocksPane::OnPointCloudItemSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::SurfaceModelItemSelected,
    this, &BlocksPane::OnSurfaceModelItemSelected);
  QObject::connect(
    blocks_tree_widget_, &BlocksTreeWidget::TextureItemSelected,
    this, &BlocksPane::OnTextureItemSelected);

}

void BlocksPane::Response(int request_flag, void* response)
{
  switch (request_flag)
  {
  case db::DatabaseMediator::REQUEST_OPEN_DATABASE:
    {
      while (1)
      {
        db::RequestGetAllBlocks blocks_request;
        db::ResponseGetAllBlocks blocks_response;
        ((MainWindow*)parent())->database_mediator().Request(
          this, db::DatabaseMediator::REQUEST_GET_ALL_BLOCKS,
          blocks_request, blocks_response, false);
        if (blocks_response.error_code != db::DatabaseMediator::NO_ERROR)
          break;

        auto itr_block = blocks_response.records.begin();
        auto itr_block_end = blocks_response.records.end();
        for (; itr_block != itr_block_end; ++itr_block)
        {
          uint block_id = uint(itr_block->first);
          std::string block_std_name =
            itr_block->second[db::BlockResource::BLOCK_FIELD_NAME].ToString();
          QString block_name = QString::fromLocal8Bit(block_std_name.c_str());
          blocks_tree_widget_->AddBlock(block_id, block_name);

          hs::recon::db::RequestGetPhotosInBlock request_get_photos_in_block;
          hs::recon::db::ResponseGetPhotosInBlock response_get_photos_in_block;
          request_get_photos_in_block.block_id =
            db::Database::Identifier(block_id);
          ((MainWindow*)parent())->database_mediator().Request(
            this, db::DatabaseMediator::REQUEST_GET_PHOTOS_IN_BLOCK,
            request_get_photos_in_block, response_get_photos_in_block, false);

          if (response_get_photos_in_block.error_code !=
              hs::recon::db::Database::NO_ERROR)
          {
            continue;
          }
          auto itr_photo = response_get_photos_in_block.records.begin();
          auto itr_photo_end = response_get_photos_in_block.records.end();
          BlocksTreeWidget::StringMap photo_names;
          for (; itr_photo != itr_photo_end; ++itr_photo)
          {
            std::string photo_path =
              itr_photo->second[db::PhotoResource::PHOTO_FIELD_PATH].ToString();
            QFileInfo file_info(QString::fromLocal8Bit(photo_path.c_str()));
            photo_names[uint(itr_photo->first)] = file_info.fileName();
          }
          blocks_tree_widget_->AddPhotosToBlock(block_id, photo_names);
        }

        hs::recon::db::RequestGetAllFeatureMatches feature_matches_request;
        hs::recon::db::ResponseGetAllFeatureMatches feature_matches_response;
        ((MainWindow*)parent())->database_mediator().Request(
          this, db::DatabaseMediator::REQUEST_GET_ALL_FEATURE_MATCHES,
          feature_matches_request, feature_matches_response, false);
        if (feature_matches_response.error_code !=
            db::DatabaseMediator::NO_ERROR)
          break;

        auto itr_feature_match = feature_matches_response.records.begin();
        auto itr_feature_match_end = feature_matches_response.records.end();
        for (; itr_feature_match != itr_feature_match_end; ++itr_feature_match)
        {
          uint feature_match_id = itr_feature_match->first;
          uint block_id =
            uint(itr_feature_match->second[
              db::FeatureMatchResource::FEATURE_MATCH_FIELD_BLOCK_ID].ToInt());
          std::string feature_match_std_name =
            itr_feature_match->second[
              db::FeatureMatchResource::FEATURE_MATCH_FIELD_NAME].ToString();
          QString feature_match_name =
            QString::fromLocal8Bit(feature_match_std_name.c_str());
          int flag =
            itr_feature_match->second[
              db::FeatureMatchResource::FEATURE_MATCH_FIELD_FLAG].ToInt();
          if (blocks_tree_widget_->AddFeatureMatch(
                block_id, feature_match_id, feature_match_name) == 0 &&
              flag == db::FeatureMatchResource::FLAG_NOT_COMPLETED)
          {
            QTreeWidgetItem* item =
              blocks_tree_widget_->FeatureMatchItem(feature_match_id);
            item->setDisabled(false);
          }
        }

        db::RequestGetAllPhotoOrientations photo_orientations_request;
        db::ResponseGetAllPhotoOrientations photo_orientations_response;
        ((MainWindow*)parent())->database_mediator().Request(
          this, db::DatabaseMediator::REQUEST_GET_ALL_PHOTO_ORIENTATIONS,
          photo_orientations_request, photo_orientations_response, false);
        if (photo_orientations_response.error_code !=
            db::DatabaseMediator::NO_ERROR)
          break;

        auto itr_photo_orientation =
          photo_orientations_response.records.begin();
        auto itr_photo_orientation_end =
          photo_orientations_response.records.end();
        for (; itr_photo_orientation != itr_photo_orientation_end;
             ++itr_photo_orientation)
        {
          uint photo_orientation_id = itr_photo_orientation->first;
          uint feature_match_id =
            uint(itr_photo_orientation->second[
              db::PhotoOrientationResource::
                PHOTO_ORIENTATION_FIELD_FEATURE_MATCH_ID].ToInt());
          std::string photo_orientation_std_name =
            itr_photo_orientation->second[
              db::PhotoOrientationResource::
                PHOTO_ORIENTATION_FIELD_NAME].ToString();
          QString photo_orientation_name =
            QString::fromLocal8Bit(photo_orientation_std_name.c_str());
          int flag =
            itr_photo_orientation->second[
              db::PhotoOrientationResource::
                PHOTO_ORIENTATION_FIELD_FLAG].ToInt();
          if (blocks_tree_widget_->AddPhotoOrientation(
                feature_match_id, photo_orientation_id,
                photo_orientation_name) == 0 &&
              flag == db::PhotoOrientationResource::FLAG_NOT_COMPLETED)
          {
            QTreeWidgetItem* item =
              blocks_tree_widget_->PhotoOrientationItem(photo_orientation_id);
            item->setDisabled(false);
          }
        }

        //读取点云数据
        db::RequestGetAllPointClouds point_cloud_request;
        db::ResponseGetAllPointClouds point_cloud_response;
        ((MainWindow*)parent())->database_mediator().Request(
          this, db::DatabaseMediator::REQUEST_GET_ALL_POINT_CLOUDS,
          point_cloud_request, point_cloud_response, false);
        if (point_cloud_response.error_code !=
          db::DatabaseMediator::NO_ERROR)
          break;

        auto itr_point_cloud =
          point_cloud_response.records.begin();
        auto itr_point_cloud_end =
          point_cloud_response.records.end();
        for (; itr_point_cloud != itr_point_cloud_end;
          ++itr_point_cloud)
        {
          uint point_cloud_id = itr_point_cloud->first;
          uint photo_orientation_id =
            uint(itr_point_cloud->second[
            db::PointCloudResource::
            POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID].ToInt());
          std::string point_cloud_std_name =
            itr_point_cloud->second[
            db::PointCloudResource::
            POINT_CLOUD_FIELD_NAME].ToString();
          QString point_cloud_name =
          QString::fromLocal8Bit(point_cloud_std_name.c_str());
          int flag =
          itr_point_cloud->second[
            db::PointCloudResource::
            POINT_CLOUD_FIELD_FLAG].ToInt();
          if (blocks_tree_widget_->AddPointCloud(
            photo_orientation_id, point_cloud_id,
            point_cloud_name) == 0 &&
            flag == db::PointCloudResource::FLAG_NOT_COMPLETED)
          {
            QTreeWidgetItem* item =
              blocks_tree_widget_->PointCloudItem(point_cloud_id);
            item->setDisabled(false);
          }
        }

        break;
      }

      break;
    }
  }
}

void BlocksPane::OnTimeout()
{
  if (workflow_queue_.empty())
  {
    std::cout<<"Finish Working!\n";
    progress_bar_->hide();
    timer_->stop();
  }
  else
  {
    if (current_step_)
    {
      int state = current_step_->state();
      switch (state)
      {
      case hs::recon::workflow::WorkflowStep::STATE_READY:
        {
          std::cout<<"Starting!\n";
          WorkflowConfig& workflow = workflow_queue_.front();
          WorkflowStepEntry& workflow_step_entry = workflow.step_queue.front();
          current_step_->Start(workflow_step_entry.config.get());
          QTreeWidgetItem* item = nullptr;
          switch (workflow_step_entry.config->type())
          {
          case workflow::STEP_FEATURE_MATCH:
            {
              item = blocks_tree_widget_->FeatureMatchItem(
                       workflow_step_entry.id);
              break;
            }
          case workflow::STEP_PHOTO_ORIENTATION:
            {
              item = blocks_tree_widget_->PhotoOrientationItem(
                       workflow_step_entry.id);
              break;
            }
          case workflow::STEP_POINT_CLOUD:
            {
              item = blocks_tree_widget_->PointCloudItem(
                       workflow_step_entry.id);
              break;
            }
          case workflow::STEP_SURFACE_MODEL:
            {
              item = blocks_tree_widget_->SurfaceModelItem(
                       workflow_step_entry.id);
              break;
            }
          case workflow::STEP_TEXTURE:
            {
              item = blocks_tree_widget_->TextureItem(
                       workflow_step_entry.id);
              break;
            }
          }
          if (item)
          {
            std::cout<<"Showing progress bar\n";
            blocks_tree_widget_->setItemWidget(item, 1, progress_bar_);
            progress_bar_->show();
            progress_bar_->setMinimum(0);
            progress_bar_->setMaximum(100);
            progress_bar_->setValue(0);
          }
          break;
        }
      case hs::recon::workflow::WorkflowStep::STATE_WORKING:
        {
          float complete_ratio = current_step_->GetCompleteRatio();
          std::cout<<"complete_ratio:"<<complete_ratio<<"\n";
          progress_bar_->setValue(int(complete_ratio * 100));
          break;
        }
      case hs::recon::workflow::WorkflowStep::STATE_ERROR:
        {
          //终止该Workflow
          std::cout<<"Error!\n";
          workflow_queue_.pop();
          current_step_ = nullptr;
          progress_bar_->hide();
          break;
        }
      case hs::recon::workflow::WorkflowStep::STATE_FINISHED:
        {
          std::cout<<"Finished!\n";
          progress_bar_->setValue(100);
          WorkflowConfig& workflow_config = workflow_queue_.front();
          WorkflowStepEntry workflow_step_entry =
            workflow_config.step_queue.front();

          switch (workflow_step_entry.config->type())
          {
          case workflow::STEP_FEATURE_MATCH:
            {
              db::RequestUpdateFeatureMatchFlag request;
              db::ResponseUpdateFeatureMatchFlag response;
              request.id = db::Database::Identifier(workflow_step_entry.id);
              request.flag = db::FeatureMatchResource::FLAG_COMPLETED;
              ((MainWindow*)parent())->database_mediator().Request(
                this, db::DatabaseMediator::REQUEST_UPDATE_FEATURE_MATCH_FLAG,
                request, response, true);
              if (response.error_code == db::DatabaseMediator::NO_ERROR)
              {
                QTreeWidgetItem* item =
                  blocks_tree_widget_->FeatureMatchItem(
                    workflow_step_entry.id);
                if (item)
                {
                  item->setDisabled(false);
                }
              }
              break;
            }
          case workflow::STEP_PHOTO_ORIENTATION:
            {
              db::RequestUpdatePhotoOrientationFlag request;
              db::ResponseUpdatePhotoOrientationFlag response;
              request.id = db::Database::Identifier(workflow_step_entry.id);
              request.flag = db::PhotoOrientationResource::FLAG_COMPLETED;
              ((MainWindow*)parent())->database_mediator().Request(
                this,
                db::DatabaseMediator::REQUEST_UPDATE_PHOTO_ORIENTATION_FLAG,
                request, response, true);
              if (response.error_code == db::DatabaseMediator::NO_ERROR)
              {
                QTreeWidgetItem* item =
                  blocks_tree_widget_->PhotoOrientationItem(
                    workflow_step_entry.id);
                if (item)
                {
                  item->setDisabled(false);
                  ActivatePhotoOrientationItem(item);
                  emit PhotoOrientationActivated(
                    activated_photo_orientation_id_);
                }
              }
              break;
            }
          case workflow::STEP_POINT_CLOUD:
            {
              db::RequestUpdatePointCloudFlag request;
              db::ResponseUpdatePointCloudFlag response;
              request.id = db::Database::Identifier(workflow_step_entry.id);
              request.flag = db::PointCloudResource::FLAG_COMPLETED;
              ((MainWindow*)parent())->database_mediator().Request(
                this,
                db::DatabaseMediator::REQUEST_UPDATE_POINT_CLOUD_FLAG,
                request, response, true);
              if (response.error_code == db::DatabaseMediator::NO_ERROR)
              {
                QTreeWidgetItem* item =
                  blocks_tree_widget_->PointCloudItem(
                  workflow_step_entry.id);
                if (item)
                {
                  item->setDisabled(false);
                  ActivatePointCloudItem(item);
                  emit PointCloudActivated(
                    activated_point_cloud_id_);
                }
              }
              break;
            }
          case workflow::STEP_SURFACE_MODEL:
            {
              break;
            }
          case workflow::STEP_TEXTURE:
            {
              break;
            }
          }

          workflow_config.step_queue.pop();

          current_step_ = nullptr;
          if (workflow_config.step_queue.empty())
          {
            workflow_queue_.pop();
          }
          break;
        }
      }
    }
    else
    {
      //开始第一个Workflow的第一个step
      if (workflow_queue_.empty())
      {
        std::cout<<"Finish Working!\n";
        progress_bar_->hide();
        timer_->stop();
      }
      else
      {
        std::cout<<"Setting!\n";
        WorkflowConfig& workflow_config = workflow_queue_.front();
        if (workflow_config.step_queue.empty())
        {
          std::cout<<"Step queue empty!\n";
        }
        WorkflowStepEntry& workflow_step_entry =
          workflow_config.step_queue.front();
        SetWorkflowStep(workflow_config.workflow_intermediate_directory,
                        workflow_step_entry);
      }
    }
  }
}

void BlocksPane::OnItemDoubleClicked(QTreeWidgetItem* item, int column)
{
  switch (item->type())
  {
  case BlocksTreeWidget::PHOTO_ORIENTATION:
    {
      ActivatePhotoOrientationItem(item);
      emit PhotoOrientationActivated(activated_photo_orientation_id_);
      break;
    }
  case BlocksTreeWidget::POINT_CLOUD:
    {
      ActivatePointCloudItem(item);
      emit PointCloudActivated(activated_point_cloud_id_);
      break;
    }
  }
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

  BlockPhotosSelectDialog::BlockInfoEntry block_info_entry;
  BlockPhotosSelectDialog dialog(block_info_entry, group_entries);
  if (dialog.exec())
  {
    //添加块
    block_info_entry = dialog.GetBlockInfo();
    hs::recon::db::RequestAddBlock request_add_block;
    hs::recon::db::ResponseAddBlock response_add_block;
    request_add_block.name = block_info_entry.name.toLocal8Bit().data();
    request_add_block.description =
      block_info_entry.description.toLocal8Bit().data();

    ((MainWindow*)parent())->database_mediator().Request(
      this, hs::recon::db::DatabaseMediator::REQUEST_ADD_BLOCK,
      request_add_block, response_add_block, true);
    if (response_add_block.error_code != db::DatabaseMediator::NO_ERROR)
    {
      QMessageBox msg_box;
      msg_box.setText(tr("Block name exists!"));
      msg_box.exec();
      return;
    }
    blocks_tree_widget_->AddBlock(uint(response_add_block.added_id),
                                  block_info_entry.name);

    //添加照片与块关系
    std::vector<uint> selected_photo_ids;
    dialog.GetSelectedPhotoIds(selected_photo_ids);
    hs::recon::db::RequestAddPhotosToBlock request_add_photos_to_block;
    hs::recon::db::ResponseAddPhotosToBlock response_add_photos_to_block;
    request_add_photos_to_block.block_id =
      hs::recon::db::Database::Identifier(response_add_block.added_id);
    auto itr_selected_photo_id = selected_photo_ids.begin();
    auto itr_selected_photo_id_end = selected_photo_ids.end();
    for (; itr_selected_photo_id != itr_selected_photo_id_end;
         ++itr_selected_photo_id)
    {
      request_add_photos_to_block.photo_ids.push_back(
        hs::recon::db::Database::Identifier(*itr_selected_photo_id));
    }
    ((MainWindow*)parent())->database_mediator().Request(
      this, hs::recon::db::DatabaseMediator::REQUEST_ADD_PHOTOS_TO_BLOCK,
      request_add_photos_to_block, response_add_photos_to_block, true);
    if (response_add_photos_to_block.error_code !=
        hs::recon::db::DatabaseMediator::NO_ERROR)
    {
      QMessageBox msg_box;
      msg_box.setText(tr("Add photos to block failed!"));
      msg_box.exec();
    }
    else
    {
      BlocksTreeWidget::StringMap photo_names;
      auto itr_added_photo = response_add_photos_to_block.photo_ids.begin();
      auto itr_added_photo_end = response_add_photos_to_block.photo_ids.end();
      for (; itr_added_photo != itr_added_photo_end; ++itr_added_photo)
      {
        hs::recon::db::RequestGetPhoto request_get_photo;
        hs::recon::db::ResponseGetPhoto response_get_photo;
        request_get_photo.id = *itr_added_photo;
        ((MainWindow*)parent())->database_mediator().Request(
          this, hs::recon::db::DatabaseMediator::REQUEST_GET_PHOTO,
          request_get_photo, response_get_photo, false);
        if (response_get_photo.error_code !=
            hs::recon::db::DatabaseMediator::NO_ERROR)
        {
          QMessageBox msg_box;
          msg_box.setText(tr("Photo not exists!"));
          msg_box.exec();
          return;
        }
        std::string photo_path =
          response_get_photo.record[
            hs::recon::db::PhotoResource::PHOTO_FIELD_PATH].ToString();
        QFileInfo file_info(QString::fromLocal8Bit(photo_path.c_str()));
        photo_names[uint(*itr_added_photo)] = file_info.fileName();
      }
      blocks_tree_widget_->AddPhotosToBlock(uint(response_add_block.added_id),
                                            photo_names);
    }

  }
}

void BlocksPane::OnActionAddWorkflowTriggered()
{
  typedef hs::recon::db::Database::Identifier Identifier;
  int configure_type = 0;
  if (item_selected_mask_[BLOCK_SELECTED])
  {
    configure_type = WorkflowConfigureWidget::CONFIGURE_FEATURE_MATCH;
  }
  else if (item_selected_mask_[FEATURE_MATCH_SELECTED])
  {
    configure_type = WorkflowConfigureWidget::CONFIGURE_PHOTO_ORIENTATION;
  }
  else if (item_selected_mask_[PHOTO_ORIENTATION_SELECTED])
  {
    configure_type = WorkflowConfigureWidget::CONFIGURE_POINT_CLOUD;
  }
  else if (item_selected_mask_[POINT_CLOUD_SELECTED])
  {
    configure_type = WorkflowConfigureWidget::CONFIGURE_SURFACE_MODEL;
  }
  else if (item_selected_mask_[SURFACE_MODEL_SELECTED])
  {
    configure_type = WorkflowConfigureWidget::CONFIGURE_TEXTURE;
  }
  else
  {
    return;
  }

  WorkflowConfigureDialog workflow_configure_dialog(configure_type);
  if (workflow_configure_dialog.exec())
  {
    QSettings settings;
    QString intermediate_directory =
      settings.value(tr("intermediate_directory")).toString();
    WorkflowConfig workflow_config;
    workflow_config.workflow_intermediate_directory =
      intermediate_directory.toLocal8Bit().data();
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    workflow_config.workflow_intermediate_directory +=
      std::string("/") + boost::uuids::to_string(uuid) + "/";
    //TODO: create_directories return false even that directories created!
    if (!boost::filesystem::create_directories(boost::filesystem::path(
          workflow_config.workflow_intermediate_directory)))
    {
      //QMessageBox msg_box;
      //msg_box.setText(tr("Fail to create intermediate directory."));
      //msg_box.exec();
      //return;
    }

    //添加feature match
    int first_configure_type = workflow_configure_dialog.FirstConfigureType();
    int last_configure_type = workflow_configure_dialog.LastConfigureType();
    uint block_id = selected_block_id_;
    uint feature_match_id = selected_feature_match_id_;
    uint photo_orientation_id = selected_photo_orientation_id_;
    uint point_cloud_id = selected_point_cloud_id_;
    bool add_feature_match = true;
    bool add_photo_orientation = true;
    bool add_point_cloud = true;
    bool add_surface_model = true;
    bool add_texture = true;
    if (first_configure_type !=
        WorkflowConfigureWidget::CONFIGURE_FEATURE_MATCH)
    {
      add_feature_match = false;
    }

    if (add_feature_match)
    {
      hs::recon::workflow::FeatureMatchConfigPtr feature_match_config(
        new hs::recon::workflow::FeatureMatchConfig);
      workflow_configure_dialog.FetchFeatureMatchConfig(*feature_match_config);
      if (AddFeatureMatchStep(block_id, feature_match_config,
                              workflow_config) != 0)
      {
        add_photo_orientation = false;
      }
      else
      {
        feature_match_id = workflow_config.step_queue.front().id;
      }
    }

    if (first_configure_type >
        WorkflowConfigureWidget::CONFIGURE_PHOTO_ORIENTATION ||
        last_configure_type <
        WorkflowConfigureWidget::CONFIGURE_PHOTO_ORIENTATION)
    {
      add_photo_orientation = false;
    }

    if (add_photo_orientation)
    {
      hs::recon::workflow::PhotoOrientationConfigPtr photo_orientation_config(
        new hs::recon::workflow::PhotoOrientationConfig);
      workflow_configure_dialog.FetchPhotoOrientationConfig(
        *photo_orientation_config);
      if (AddPhotoOrientationStep(feature_match_id, photo_orientation_config,
                                  workflow_config) != 0)
      {
        add_photo_orientation = false;
      }
      else
      {
        photo_orientation_id = workflow_config.step_queue.front().id;
      }
    }

    //添加点云
    if (first_configure_type >
      WorkflowConfigureWidget::CONFIGURE_POINT_CLOUD ||
      last_configure_type <
      WorkflowConfigureWidget::CONFIGURE_POINT_CLOUD)
    {
      add_point_cloud = false;
    }

    if (add_point_cloud)
    {
      hs::recon::workflow::PointCloudConfigPtr point_cloud_config(
        new hs::recon::workflow::PointCloudConfig);
      workflow_configure_dialog.FetchPointCloudConfig(
        *point_cloud_config);
      if (AddPointCloudStep(photo_orientation_id, point_cloud_config,
                            workflow_config) != 0)
      {
        add_point_cloud = false;
      }
      else
      {
        point_cloud_id = workflow_config.step_queue.front().id;
      }
    }

    if (!workflow_config.step_queue.empty())
    {
      workflow_queue_.push(workflow_config);
      timer_->start(500);
    }

  }
}

void BlocksPane::OnBlockItemSelected(uint block_id)
{
  action_remove_block_->setEnabled(true);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_block_id_ = block_id;
  item_selected_mask_.reset();
  item_selected_mask_.set(BLOCK_SELECTED);
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
  item_selected_mask_.reset();
  item_selected_mask_.set(PHOTOS_SELECTED);
}

void BlocksPane::OnFeatureMatchItemSelected(uint feature_match_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_feature_match_id_ = feature_match_id;
  item_selected_mask_.reset();
  item_selected_mask_.set(FEATURE_MATCH_SELECTED);
}

void BlocksPane::OnPhotoOrientationItemSelected(uint photo_orientation_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_photo_orientation_id_ = photo_orientation_id;
  item_selected_mask_.reset();
  item_selected_mask_.set(PHOTO_ORIENTATION_SELECTED);
}

void BlocksPane::OnPointCloudItemSelected(uint point_cloud_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_point_cloud_id_ = point_cloud_id;
  item_selected_mask_.reset();
  item_selected_mask_.set(POINT_CLOUD_SELECTED);
}

void BlocksPane::OnSurfaceModelItemSelected(uint surface_model_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_surface_model_id_ = surface_model_id;
  item_selected_mask_.reset();
  item_selected_mask_.set(SURFACE_MODEL_SELECTED);
}

void BlocksPane::OnTextureItemSelected(uint texture_id)
{
  action_remove_block_->setEnabled(false);
  action_remove_photos_->setEnabled(false);
  action_add_workflow_->setEnabled(true);
  selected_texture_id_ = texture_id;
  item_selected_mask_.reset();
  item_selected_mask_.set(TEXTURE_SELECTED);
}

void BlocksPane::ActivatePhotoOrientationItem(
  QTreeWidgetItem* photo_orientation_item)
{
  QTreeWidgetItem* last_photo_orientation_item =
    blocks_tree_widget_->PhotoOrientationItem(activated_photo_orientation_id_);
  if (last_photo_orientation_item == photo_orientation_item) return;
  if (last_photo_orientation_item)
  {
    last_photo_orientation_item->setBackground(0, backup_background_);
  }

  backup_background_ = photo_orientation_item->background(0);
  photo_orientation_item->setBackground(0, QBrush(QColor(200, 110, 90)));
  activated_photo_orientation_id_ =
    photo_orientation_item->data(0, Qt::UserRole).toUInt();
}

void BlocksPane::ActivatePointCloudItem(
  QTreeWidgetItem* point_cloud_item)
{
  QTreeWidgetItem* last_point_cloud_item =
    blocks_tree_widget_->PointCloudItem(activated_point_cloud_id_);
  if (last_point_cloud_item == point_cloud_item) return;
  if (last_point_cloud_item)
  {
    last_point_cloud_item->setBackground(0, backup_background_);
  }

  backup_background_ = point_cloud_item->background(0);
  point_cloud_item->setBackground(0, QBrush(QColor(200, 110, 90)));
  activated_point_cloud_id_ =
    point_cloud_item->data(0, Qt::UserRole).toUInt();
}

int BlocksPane::SetWorkflowStep(
  const std::string& workflow_intermediate_directory,
  WorkflowStepEntry& workflow_step_entry)
{
  std::cout<<"type:"<<workflow_step_entry.config->type()<<"\n";
  switch (workflow_step_entry.config->type())
  {
  case workflow::STEP_FEATURE_MATCH:
    {
      current_step_ = SetFeatureMatchStep(workflow_intermediate_directory,
                                          workflow_step_entry);
      break;
    }
  case workflow::STEP_PHOTO_ORIENTATION:
    {
      current_step_ = SetPhotoOrientationStep(workflow_intermediate_directory,
                                              workflow_step_entry);
      break;
    }
  case workflow::STEP_POINT_CLOUD:
    {
      current_step_ = SetPointCloudStep(workflow_intermediate_directory,
                                        workflow_step_entry);
      break;
    }
  case workflow::STEP_SURFACE_MODEL:
    {
      current_step_ = SetSurfaceModelStep(workflow_intermediate_directory,
                                          workflow_step_entry);
      break;
    }
  case workflow::STEP_TEXTURE:
    {
      current_step_ = SetTextureStep(workflow_intermediate_directory,
                                     workflow_step_entry);
      break;
    }
  }
  return 0;
}

int BlocksPane::AddFeatureMatchStep(
  uint block_id,
  workflow::FeatureMatchConfigPtr feature_match_config,
  WorkflowConfig& workflow_config)
{
  //添加feature match
  typedef db::Database::Identifier Identifier;
  hs::recon::db::RequestAddFeatureMatch request_add_feature_match;
  hs::recon::db::ResponseAddFeatureMatch response_add_feature_match;
  request_add_feature_match.block_id = Identifier(block_id);
  ((MainWindow*)parent())->database_mediator().Request(
    this, hs::recon::db::DatabaseMediator::REQUEST_ADD_FEATURE_MATCH,
    request_add_feature_match, response_add_feature_match, true);
  if (response_add_feature_match.error_code ==
      hs::recon::db::DatabaseMediator::NO_ERROR)
  {
    uint feature_match_id = uint(response_add_feature_match.feature_match_id);
    QString feature_match_name =
      QString::fromLocal8Bit(response_add_feature_match.name.c_str());
    blocks_tree_widget_->AddFeatureMatch(block_id,
                                         feature_match_id,
                                         feature_match_name);
    QTreeWidgetItem* feature_match_item =
      blocks_tree_widget_->FeatureMatchItem(feature_match_id);
    if (feature_match_item)
    {
      feature_match_item->setDisabled(true);
    }
    WorkflowStepEntry feature_match_step_entry;
    feature_match_step_entry.id = uint(feature_match_id);
    feature_match_step_entry.config = feature_match_config;
    workflow_config.step_queue.push(feature_match_step_entry);
    return 0;
  }
  else
  {
    return -1;
  }
}

int BlocksPane::AddPhotoOrientationStep(
  uint feature_match_id,
  workflow::PhotoOrientationConfigPtr photo_orientation_config,
  WorkflowConfig& workflow_config)
{
  //添加photo orientation
  typedef db::Database::Identifier Identifier;
  hs::recon::db::RequestAddPhotoOrientation request_add_photo_orientation;
  hs::recon::db::ResponseAddPhotoOrientation response_add_photo_orientation;
  request_add_photo_orientation.feature_match_id =
    Identifier(feature_match_id);
  ((MainWindow*)parent())->database_mediator().Request(
    this, hs::recon::db::DatabaseMediator::REQUEST_ADD_PHOTO_ORIENTATION,
    request_add_photo_orientation, response_add_photo_orientation, true);
  if (response_add_photo_orientation.error_code ==
      hs::recon::db::DatabaseMediator::NO_ERROR)
  {
    uint photo_orientation_id =
      uint(response_add_photo_orientation.photo_orientation_id);
    QString photo_orientation_name =
      QString::fromLocal8Bit(response_add_photo_orientation.name.c_str());
    blocks_tree_widget_->AddPhotoOrientation(feature_match_id,
                                             photo_orientation_id,
                                             photo_orientation_name);
    QTreeWidgetItem* photo_orientation_item =
      blocks_tree_widget_->PhotoOrientationItem(photo_orientation_id);
    if (photo_orientation_item)
    {
      photo_orientation_item->setDisabled(true);
    }
    WorkflowStepEntry photo_orientation_step_entry;
    photo_orientation_step_entry.id = photo_orientation_id;
    photo_orientation_step_entry.config = photo_orientation_config;
    workflow_config.step_queue.push(photo_orientation_step_entry);
    return 0;
  }
  else
  {
    return -1;
  }
}

int BlocksPane::AddPointCloudStep(
  uint photo_orientation_id,
  workflow::PointCloudConfigPtr point_cloud_config,
  WorkflowConfig& workflow_config)
{
  typedef db::Database::Identifier Identifier;
  hs::recon::db::RequestAddPointCloud request_add_point_cloud;
  hs::recon::db::ResponseAddPointCloud response_add_point_cloud;
  request_add_point_cloud.photo_orientation_id =
    Identifier(photo_orientation_id);
  ((MainWindow*)parent())->database_mediator().Request(
    this, hs::recon::db::DatabaseMediator::REQUEST_ADD_POINT_CLOUD,
    request_add_point_cloud, response_add_point_cloud, true);
  if (response_add_point_cloud.error_code ==
    hs::recon::db::DatabaseMediator::NO_ERROR)
  {
    uint point_cloud_id =
      uint(response_add_point_cloud.point_cloud_id);
    QString point_cloud_name =
      QString::fromLocal8Bit(response_add_point_cloud.name.c_str());
    blocks_tree_widget_->AddPointCloud(photo_orientation_id,
                                       point_cloud_id,
                                       point_cloud_name);
    QTreeWidgetItem* point_cloud_item =
      blocks_tree_widget_->PointCloudItem(point_cloud_id);
    if (point_cloud_item)
    {
      point_cloud_item->setDisabled(true);
    }
    WorkflowStepEntry point_cloud_step_entry;
    point_cloud_step_entry.id = point_cloud_id;
    point_cloud_step_entry.config = point_cloud_config;
    workflow_config.step_queue.push(point_cloud_step_entry);
    return 0;
  }
  else
  {
    return -1;
  }
}

BlocksPane::WorkflowStepPtr BlocksPane::SetFeatureMatchStep(
  const std::string& workflow_intermediate_directory,
  WorkflowStepEntry& workflow_step_entry)
{
  typedef hs::recon::db::Database::Identifier Identifier;
  typedef hs::recon::workflow::FeatureMatchConfig::PosEntry PosEntry;
  while (1)
  {

    //获取特征匹配数据
    hs::recon::db::RequestGetFeatureMatch request_feature_match;
    hs::recon::db::ResponseGetFeatureMatch response_feature_match;
    request_feature_match.id = Identifier(workflow_step_entry.id);
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_FEATURE_MATCH,
      request_feature_match, response_feature_match, false);

    if (response_feature_match.error_code != hs::recon::db::Database::NO_ERROR)
    {
      break;
    }

    //获取特征匹配所在块包含的影像
    Identifier block_id =
      Identifier(
        response_feature_match.record[
          db::FeatureMatchResource::FEATURE_MATCH_FIELD_BLOCK_ID].ToInt());
    std::string feature_match_path =
      response_feature_match.record[
        db::FeatureMatchResource::FEATURE_MATCH_FIELD_PATH].ToString();
    std::string matches_path =
      feature_match_path + "matches.txt";

    hs::recon::db::RequestGetPhotosInBlock request_get_photos_in_block;
    hs::recon::db::ResponseGetPhotosInBlock response_get_photos_in_block;
    request_get_photos_in_block.block_id = block_id;
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_PHOTOS_IN_BLOCK,
      request_get_photos_in_block, response_get_photos_in_block, false);

    if (response_get_photos_in_block.error_code !=
        hs::recon::db::Database::NO_ERROR)
    {
      break;
    }
    auto itr_photo = response_get_photos_in_block.records.begin();
    auto itr_photo_end = response_get_photos_in_block.records.end();
    std::vector<std::string> image_paths;
    std::vector<std::string> key_paths;
    std::vector<std::string> descriptor_paths;
    std::map<size_t, PosEntry> pos_entries;
    double invalid_value = -1e-100;
    for (size_t i = 0; itr_photo != itr_photo_end; ++itr_photo, i++)
    {
      image_paths.push_back(
        itr_photo->second[db::PhotoResource::PHOTO_FIELD_PATH].ToString());
      key_paths.push_back(
        boost::str(boost::format("%1%%2%.key") %
                   feature_match_path %
                   itr_photo->first));
      descriptor_paths.push_back(
        boost::str(boost::format("%1%%2%.desc") %
                   workflow_intermediate_directory %
                   itr_photo->first));
      PosEntry pos_entry;
      pos_entry.x =
        itr_photo->second[db::PhotoResource::PHOTO_FIELD_POS_X].ToFloat();
      pos_entry.y =
        itr_photo->second[db::PhotoResource::PHOTO_FIELD_POS_Y].ToFloat();
      pos_entry.z =
        itr_photo->second[db::PhotoResource::PHOTO_FIELD_POS_Z].ToFloat();
      if (pos_entry.x > invalid_value &&
          pos_entry.y > invalid_value &&
          pos_entry.z > invalid_value)
      {
        pos_entries[i] = pos_entry;
      }
    }
    QSettings settings;
    QString number_of_threads_key = tr("number_of_threads");
    uint number_of_threads = settings.value(number_of_threads_key,
                                            QVariant(uint(1))).toUInt();
    workflow::FeatureMatchConfigPtr feature_match_config =
      std::static_pointer_cast<workflow::FeatureMatchConfig>(
        workflow_step_entry.config);
    feature_match_config->set_image_paths(image_paths);
    feature_match_config->set_key_paths(key_paths);
    feature_match_config->set_descripor_paths(descriptor_paths);
    feature_match_config->set_pos_entries(pos_entries);
    feature_match_config->set_matches_path(matches_path);
    feature_match_config->set_number_of_threads(int(number_of_threads));

    break;
  }
  return WorkflowStepPtr(new workflow::OpenCVFeatureMatch);
}

BlocksPane::WorkflowStepPtr BlocksPane::SetPhotoOrientationStep(
  const std::string& workflow_intermediate_directory,
  WorkflowStepEntry& workflow_step_entry)
{
  typedef hs::recon::db::Database::Identifier Identifier;
  typedef workflow::PhotoOrientationConfig::IntrinsicParams
          IntrinsicParams;
  typedef workflow::PhotoOrientationConfig::IntrinsicParamsContainer
          IntrinsicParamsContainer;
  while (1)
  {
    //获取相机朝向数据
    hs::recon::db::RequestGetPhotoOrientation request_photo_orientation;
    hs::recon::db::ResponseGetPhotoOrientation response_photo_orientation;
    request_photo_orientation.id = Identifier(workflow_step_entry.id);
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_PHOTO_ORIENTATION,
      request_photo_orientation, response_photo_orientation, false);
    if (response_photo_orientation.error_code !=
        hs::recon::db::Database::NO_ERROR)
    {
      break;
    }
    std::string photo_orientation_path =
      response_photo_orientation.record[
        db::PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_PATH].ToString();

    //获取特征匹配数据
    int field_id =
      db::PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FEATURE_MATCH_ID;
    Identifier feature_match_id =
      Identifier(
        response_photo_orientation.record[field_id].ToInt());
    hs::recon::db::RequestGetFeatureMatch request_feature_match;
    hs::recon::db::ResponseGetFeatureMatch response_feature_match;
    request_feature_match.id = feature_match_id;
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_FEATURE_MATCH,
      request_feature_match, response_feature_match, false);

    //获取特征匹配所在块包含的影像
    Identifier block_id =
      Identifier(
        response_feature_match.record[
          db::FeatureMatchResource::FEATURE_MATCH_FIELD_BLOCK_ID].ToInt());
    std::string feature_match_path =
      response_feature_match.record[
        db::FeatureMatchResource::FEATURE_MATCH_FIELD_PATH].ToString();
    std::string matches_path =
      feature_match_path + "matches.txt";

    hs::recon::db::RequestGetPhotosInBlock request_get_photos_in_block;
    hs::recon::db::ResponseGetPhotosInBlock response_get_photos_in_block;
    request_get_photos_in_block.block_id = block_id;
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_PHOTOS_IN_BLOCK,
      request_get_photos_in_block, response_get_photos_in_block, false);

    if (response_get_photos_in_block.error_code !=
        hs::recon::db::Database::NO_ERROR)
    {
      break;
    }
    auto itr_photo = response_get_photos_in_block.records.begin();
    auto itr_photo_end = response_get_photos_in_block.records.end();
    size_t number_of_photos = response_get_photos_in_block.records.size();
    std::vector<std::string> image_paths;
    std::vector<std::string> key_paths;
    std::vector<int> image_ids;
    hs::sfm::ObjectIndexMap image_intrinsic_map(number_of_photos);
    IntrinsicParamsContainer intrinsic_params_set;
    std::vector<int> intrinsic_ids;
    for (size_t i = 0; itr_photo != itr_photo_end; ++itr_photo, i++)
    {
      image_paths.push_back(
        itr_photo->second[db::PhotoResource::PHOTO_FIELD_PATH].ToString());
      key_paths.push_back(
        boost::str(boost::format("%1%%2%.key") %
                   feature_match_path %
                   itr_photo->first));
      image_ids.push_back(int(itr_photo->first));

      int photogroup_id =
        itr_photo->second[db::PhotoResource::PHOTO_FIELD_PHOTOGROUP_ID].ToInt();
      bool found = false;
      for (size_t j = 0; j < intrinsic_ids.size(); j++)
      {
        if (intrinsic_ids[j] == photogroup_id)
        {
          found = true;
          image_intrinsic_map.SetObjectId(i, j);
          break;
        }
      }
      if (!found)
      {
        intrinsic_ids.push_back(int(photogroup_id));
        image_intrinsic_map.SetObjectId(i, intrinsic_ids.size() - 1);
        //获取照片组数据
        hs::recon::db::RequestGetPhotogroup request_photogroup;
        hs::recon::db::ResponseGetPhotogroup response_photogroup;
        request_photogroup.id = Identifier(photogroup_id);
        ((MainWindow*)parent())->database_mediator().Request(
          this, db::DatabaseMediator::REQUEST_GET_PHOTOGROUP,
          request_photogroup, response_photogroup, false);
        double focal_length =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_FOCAL_LENGTH].ToFloat();
        double pixel_size =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_X_SIZE].ToFloat();
        double principal_x =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_X].ToFloat();
        double principal_y =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_Y].ToFloat();
        double radial1 =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL1].ToFloat();
        double radial2 =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL2].ToFloat();
        double radial3 =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_RADIAL3].ToFloat();
        double decentering1 =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING1].ToFloat();
        double decentering2 =
          response_photogroup.record[
            db::PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING2].ToFloat();
        IntrinsicParams intrinsic_params(focal_length / pixel_size,
                                         0,
                                         principal_x,
                                         principal_y,
                                         1,
                                         radial1,
                                         radial2,
                                         radial3);
        std::cout<<"focal_length:"<<focal_length<<"\n";
        intrinsic_params_set.push_back(intrinsic_params);
      }
    }
    QSettings settings;
    QString number_of_threads_key = tr("number_of_threads");
    uint number_of_threads = settings.value(number_of_threads_key,
      QVariant(uint(1))).toUInt();
    workflow::PhotoOrientationConfigPtr photo_orientation_config =
      std::static_pointer_cast<workflow::PhotoOrientationConfig>(
        workflow_step_entry.config);
    photo_orientation_config->set_image_intrinsic_map(image_intrinsic_map);
    photo_orientation_config->set_matches_path(matches_path);
    photo_orientation_config->set_image_paths(image_paths);
    photo_orientation_config->set_key_paths(key_paths);
    photo_orientation_config->set_image_ids(image_ids);
    photo_orientation_config->set_intrinsic_params_set(intrinsic_params_set);
    photo_orientation_config->set_intrinsic_ids(intrinsic_ids);
    photo_orientation_config->set_intrinsic_path(
      response_photo_orientation.intrinsic_path);
    photo_orientation_config->set_extrinsic_path(
      response_photo_orientation.extrinsic_path);
    photo_orientation_config->set_point_cloud_path(
      response_photo_orientation.point_cloud_path);
    photo_orientation_config->set_workspace_path(
      response_photo_orientation.workspace_path);
    photo_orientation_config->set_number_of_threads(uint(number_of_threads));

    break;
  }
  return WorkflowStepPtr(new workflow::IncrementalPhotoOrientation);
}

BlocksPane::WorkflowStepPtr BlocksPane::SetPointCloudStep(
  const std::string& workflow_intermediate_directory,
  WorkflowStepEntry& workflow_step_entry)
{
  typedef hs::recon::db::Database::Identifier Identifier;
  while (1)
  {

    workflow::PointCloudConfigPtr point_cloud_config =
      std::static_pointer_cast<workflow::PointCloudConfig>(
      workflow_step_entry.config);

    //获取点云数据
    hs::recon::db::RequestGetPointCloud request_point_cloud;
    hs::recon::db::ResponseGetPointCloud response_point_cloud;
    request_point_cloud.id = Identifier(workflow_step_entry.id);
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_POINT_CLOUD,
      request_point_cloud, response_point_cloud, false);
    if (response_point_cloud.error_code !=
      hs::recon::db::Database::NO_ERROR)
    {
      break;
    }
    std::string point_cloud_path =
      response_point_cloud.record[
        db::PointCloudResource::POINT_CLOUD_FIELD_PATH].ToString();
    std::string photo_orientation_path = response_point_cloud.photo_orientation_path;

    QSettings settings;
    QString number_of_threads_key = tr("number_of_threads");
    uint number_of_threads = settings.value(number_of_threads_key,
      QVariant(uint(1))).toUInt();

    point_cloud_config->set_workspace_path(point_cloud_path);
    point_cloud_config->set_photo_orientation_path(photo_orientation_path);
    point_cloud_config->set_number_of_threads(number_of_threads);
    break;
  }//while(1)
  return WorkflowStepPtr(new workflow::PointCloud);
}

BlocksPane::WorkflowStepPtr BlocksPane::SetSurfaceModelStep(
  const std::string& workflow_intermediate_directory,
  WorkflowStepEntry& workflow_step_entry)
{
  return WorkflowStepPtr(new workflow::OpenCVFeatureMatch);
}

BlocksPane::WorkflowStepPtr BlocksPane::SetTextureStep(
  const std::string& workflow_intermediate_directory,
  WorkflowStepEntry& workflow_step_entry)
{
  return WorkflowStepPtr(new workflow::OpenCVFeatureMatch);
}

}
}
}
