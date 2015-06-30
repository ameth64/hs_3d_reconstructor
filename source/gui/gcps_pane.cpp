#include <limits>
#include <fstream>

#include <QToolBar>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QRegExp>

#include <cereal/types/utility.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_sfm/sfm_utility/projective_functions.hpp"
#include "hs_sfm/sfm_utility/similar_transform_estimator.hpp"
#include "hs_sfm/triangulate/multiple_view_maximum_likelihood_estimator.hpp"
#include "hs_sfm/sfm_pipeline/bundle_adjustment_gcp_constrained_optimizor.hpp"
#include "hs_sfm/sfm_pipeline/point_cloud_norm_calculator.hpp"

#include "gui/property_field_asignment_dialog.hpp"
#include "gui/main_window.hpp"
#include "gui/gcp_constrained_optimization_config_dialog.hpp"
#include "gui/progress_dialog.hpp"
#include "gui/gcps_pane.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

GCPsPane::GCPsPane(QWidget* parent)
  : ManagerPane(tr("Ground Control Points"), parent)
  , icon_add_gcp_(":/images/icon_gcp_add.png")
  , icon_remove_gcps_(":/images/icon_gcp_remove.png")
  , icon_import_gcps_(":/images/icon_gcp_import.png")
  , icon_show_measurement_(":/images/icon_gcp_show_measurement.png")
  , icon_show_estimate_(":/images/icon_gcp_show_estimate.png")
  , icon_show_error_(":/images/icon_gcp_show_error.png")
  , icon_gcp_constrained_optimize_(":/images/icon_gcp_constrained_optimize.png")
  , icon_gcp_config_(":/images/icon_gcp_config.png")
  , photo_orientation_id_(std::numeric_limits<uint>::max())
  , current_gcp_id_(std::numeric_limits<uint>::max())
  , similar_scale_(Scalar(1))
  , similar_rotation_(Rotation())
  , similar_translate_(Point3D::Zero())
  , gcp_planar_accuracy_(0.005)
  , gcp_height_accuracy_(0.01)
  , tiepoint_feature_accuracy_(0.1)
  , gcp_marker_accuracy_(4.0)
  , is_gcp_constrained_(false)
{
  gcps_table_widget_ = new GCPsTableWidget(this);
  tiepoint_measure_widget_ = new TiepointMeasureWidget(this);
  AddWidget(gcps_table_widget_);
  AddWidget(tiepoint_measure_widget_);

  action_add_gcp_ =
    new QAction(icon_add_gcp_, tr("Add Ground Control Point"), this);
  action_add_gcp_->setEnabled(true);
  action_remove_gcps_ =
    new QAction(icon_remove_gcps_, tr("Remove Ground Control Points"), this);
  action_remove_gcps_->setEnabled(false);
  action_import_gcps_ =
    new QAction(icon_import_gcps_, tr("Import Ground Control Points"), this);
  action_import_gcps_->setEnabled(true);
  action_show_measurement_ =
    new QAction(icon_show_measurement_, tr("Show Measurement"), this);
  action_show_measurement_->setEnabled(true);
  action_show_estimate_ =
    new QAction(icon_show_estimate_, tr("Show Estimate"), this);
  action_show_estimate_->setEnabled(true);
  action_show_error_ =
    new QAction(icon_show_error_, tr("Show Error"), this);
  action_show_error_->setEnabled(true);
  action_gcp_constrained_optimize_ =
    new QAction(icon_gcp_constrained_optimize_,
                tr("GCP Constrained Optimize"), this);
  action_gcp_constrained_optimize_->setEnabled(false);
  action_gcp_config_ =
    new QAction(icon_gcp_config_, tr("GCP Configuration"), this);
  action_gcp_config_->setEnabled(true);

  tool_bar_ = new QToolBar(this);
  tool_bar_->addAction(action_add_gcp_);
  tool_bar_->addAction(action_remove_gcps_);
  tool_bar_->addAction(action_import_gcps_);
  tool_bar_->addAction(action_show_measurement_);
  tool_bar_->addAction(action_show_estimate_);
  tool_bar_->addAction(action_show_error_);
  tool_bar_->addAction(action_gcp_constrained_optimize_);
  tool_bar_->addAction(action_gcp_config_);
  main_window_->addToolBar(tool_bar_);

  QObject::connect(action_add_gcp_, &QAction::triggered,
                   this, &GCPsPane::OnActionAddGCPTriggered);
  QObject::connect(action_remove_gcps_, &QAction::triggered,
                   gcps_table_widget_,
                   &GCPsTableWidget::DeleteGCPsBySelectedItems);
  QObject::connect(action_import_gcps_, &QAction::triggered,
                   this, &GCPsPane::OnActionImportGCPTriggered);
  QObject::connect(action_show_measurement_, &QAction::triggered,
                   this, &GCPsPane::OnActionShowMeasurementTriggered);
  QObject::connect(action_show_estimate_, &QAction::triggered,
                   this, &GCPsPane::OnActionShowEstimateTriggered);
  QObject::connect(action_show_error_, &QAction::triggered,
                   this, &GCPsPane::OnActionShowErrorTriggered);
  QObject::connect(action_gcp_constrained_optimize_, &QAction::triggered,
                   this, &GCPsPane::OnActionGCPConstrainedOptimizeTriggered);
  QObject::connect(action_gcp_config_, &QAction::triggered,
                   this, &GCPsPane::OnActionGCPConfigTriggered);

  QObject::connect(gcps_table_widget_, &GCPsTableWidget::GCPsSelected,
                   this, &GCPsPane::OnGCPsSelected);
  QObject::connect(gcps_table_widget_, &QTableWidget::itemClicked,
                   this, &GCPsPane::OnActionTableItemClicked);
  QObject::connect(gcps_table_widget_, &GCPsTableWidget::SeletedGCPsDeleted,
                   this, &GCPsPane::OnGCPsDeleteed);

  QObject::connect(tiepoint_measure_widget_,
                   &TiepointMeasureWidget::TransmissionMeasured,
                   this, &GCPsPane::OnPhotoMeasured);
  QObject::connect(tiepoint_measure_widget_,
                   &TiepointMeasureWidget::TransmissionMeasureDeleted,
                   this, &GCPsPane::OnPhotoMeasureDeleted);
  QObject::connect(gcps_table_widget_, &GCPsTableWidget::GCPUpdated,
                   this, &GCPsPane::OnGCPUpdated);
}

void GCPsPane::Response(int request_flag, void* response)
{
  switch (request_flag)
  {
  case db::DatabaseMediator::REQUEST_OPEN_DATABASE:
    {
      db::ResponseOpenDatabase* response_open =
        static_cast<db::ResponseOpenDatabase*>(response);
      if (response_open->error_code == db::DatabaseMediator::NO_ERROR)
      {
        while (1)
        {
          db::RequestGetAllGCPs gcps_request;
          db::ResponseGetAllGCPs gcps_response;
          ((MainWindow*)parent())->database_mediator().Request(
            this, db::DatabaseMediator::REQUEST_GET_ALL_GCPS,
            gcps_request, gcps_response, false);
          if (gcps_response.error_code != db::DatabaseMediator::NO_ERROR)
            break;

          auto itr_record = gcps_response.records.begin();
          auto itr_record_end = gcps_response.records.end();
          GCPsTableWidget::GCPContainer gcps;
          gcp_measures_.clear();
          for (; itr_record != itr_record_end; ++itr_record)
          {
            GCPsTableWidget::GCPEntry gcp;
            gcp.id = uint(itr_record->first);
            std::string name =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_NAME].ToString();
            gcp.name = QString::fromLocal8Bit(name.c_str());
            db::Database::Float x =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_X].ToFloat();
            db::Database::Float y =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_Y].ToFloat();
            db::Database::Float z =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_Z].ToFloat();
            gcp.measure_pos[0] = GCPsTableWidget::Float(x);
            gcp.measure_pos[1] = GCPsTableWidget::Float(y);
            gcp.measure_pos[2] = GCPsTableWidget::Float(z);
            gcp.estimate_pos[0] =
              -std::numeric_limits<GCPsTableWidget::Float>::max();
            gcp.estimate_pos[1] =
              -std::numeric_limits<GCPsTableWidget::Float>::max();
            gcp.estimate_pos[2] =
              -std::numeric_limits<GCPsTableWidget::Float>::max();
            gcp.type = GCPsTableWidget::GCPEntry::NOT_USED;
            gcps[gcp.id] = gcp;

            GCPMeasure gcp_measure;
            gcp_measure.gcp_id = gcp.id;
            gcp_measure.type = GCPsTableWidget::GCPEntry::NOT_USED;
            gcp_measure.measure_pos[0] = gcp.measure_pos[0];
            gcp_measure.measure_pos[1] = gcp.measure_pos[1];
            gcp_measure.measure_pos[2] = gcp.measure_pos[2];
            gcp_measure.estimate_pos[0] = gcp.estimate_pos[0];
            gcp_measure.estimate_pos[1] = gcp.estimate_pos[1];
            gcp_measure.estimate_pos[2] = gcp.estimate_pos[2];
            gcp_measures_[gcp_measure.gcp_id] = gcp_measure;
          }
          gcps_table_widget_->ClearGCPs();
          gcps_table_widget_->AddGCPs(gcps);

          break;
        }
      }
      break;
    }
  case db::DatabaseMediator::REQUEST_CLOSE_DATABASE:
    {
      db::ResponseCloseDatabase* response_close =
        static_cast<db::ResponseCloseDatabase*>(response);
      if (response_close->error_code == db::DatabaseMediator::NO_ERROR)
      {
        gcps_table_widget_->ClearGCPs();
        tiepoint_measure_widget_->SetTiepointPhotos(
          TiepointMeasureWidget::TiepointPhotoContainer());
        action_gcp_constrained_optimize_->setEnabled(false);
        photo_entries_.clear();
        gcp_measures_.clear();
        intrinsic_params_set_.clear();
        photo_orientation_id_ = std::numeric_limits<uint>::max();
        current_gcp_id_ = std::numeric_limits<uint>::max();
        similar_scale_ = 1.0;
        similar_rotation_.SetIdentity();
        similar_translate_.setZero();
      }
      break;
    }
  }
}

void GCPsPane::UpdatePhotoOrientation(uint photo_orientation_id)
{
  if (photo_orientation_id == photo_orientation_id_)
  {
    return;
  }
  photo_orientation_id_ = photo_orientation_id;
  hs::recon::db::RequestGetPhotoOrientation request_photo_orientation;
  hs::recon::db::ResponseGetPhotoOrientation response_photo_orientation;
  request_photo_orientation.id =
    db::Database::Identifier(photo_orientation_id_);
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_PHOTO_ORIENTATION,
    request_photo_orientation, response_photo_orientation, false);
  if (response_photo_orientation.error_code !=
      hs::recon::db::Database::NO_ERROR)
  {
    return;
  }
  int flag =
    response_photo_orientation.record[
      db::PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FLAG].ToInt();
  if (flag & db::PhotoOrientationResource::FLAG_GCP_CONSTRAINED)
  {
    is_gcp_constrained_ = true;
  }
  else
  {
    is_gcp_constrained_ = false;
  }

  std::string photo_orientation_path =
    ((MainWindow*)parent())->database_mediator().GetPhotoOrientationPath(
      request_photo_orientation.id);

  {
    std::ifstream similar_file(
      response_photo_orientation.similar_transform_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(similar_file);
    archive(similar_scale_, similar_rotation_, similar_translate_);
  }

  intrinsic_params_set_.clear();
  {
    std::ifstream intrinsic_file(
      response_photo_orientation.intrinsic_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(intrinsic_file);
    archive(intrinsic_params_set_);
  }

  ExtrinsicParamsMap extrinsic_params_map;
  {
    std::ifstream extrinsic_file(
      response_photo_orientation.extrinsic_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(extrinsic_file);
    archive(extrinsic_params_map);
  }
  std::ifstream extrinsic_file(
    response_photo_orientation.extrinsic_path.c_str());
  if (!extrinsic_file) return;
  hs::imgio::whole::ImageIO image_io;
  for (const auto& extrinsic_params : extrinsic_params_map)
  {
    PhotoEntry photo_entry;
    size_t photo_id = extrinsic_params.first.first;
    size_t intrinsic_id = extrinsic_params.first.second;
    auto itr_intrinsic_params = intrinsic_params_set_.find(intrinsic_id);
    if (itr_intrinsic_params == intrinsic_params_set_.end()) return;
    photo_entry.intrinsic_params = itr_intrinsic_params->second;
    photo_entry.intrinsic_id = intrinsic_id;
    photo_entry.extrinsic_params = extrinsic_params.second;

    db::RequestGetPhoto request_photo;
    db::ResponseGetPhoto response_photo;
    request_photo.id = db::Database::Identifier(photo_id);
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_GET_PHOTO,
      request_photo, response_photo, false);
    if (response_photo.error_code != db::DatabaseMediator::NO_ERROR)
    {
      continue;
    }
    photo_entry.path =
      response_photo.record[
        db::PhotoResource::PHOTO_FIELD_PATH].ToString();
    photo_entry.thumbnail_path =
      ((MainWindow*)parent())->database_mediator().GetThumbnailPath(
        request_photo.id);
    size_t width, height;
    if (image_io.GetImageDimension(photo_entry.path, width, height) != 0)
    {
      continue;
    }
    photo_entry.image_width = int(width);
    photo_entry.image_height = int(height);

    photo_entries_[uint(photo_id)] = photo_entry;
  }

  //获取像控点相关数据
  db::RequestGetPhotoMeasuresInPhotoOrientation request_photo_measure;
  db::ResponseGetPhotoMeasuresInPhotoOrientation response_photo_measure;
  request_photo_measure.photo_orientation_id =
    db::Identifier(photo_orientation_id_);
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_PHOTO_MEASURES_IN_PHOTO_ORIENTATION,
    request_photo_measure, response_photo_measure, false);
  if (response_photo_measure.error_code == db::DatabaseMediator::NO_ERROR)
  {
    auto itr_gcp_measure = gcp_measures_.begin();
    auto itr_gcp_measure_end = gcp_measures_.end();
    for (; itr_gcp_measure != itr_gcp_measure_end; ++itr_gcp_measure)
    {
      itr_gcp_measure->second.photo_measures.clear();
      itr_gcp_measure->second.estimate_pos[0] =
        -std::numeric_limits<Scalar>::max();
      itr_gcp_measure->second.estimate_pos[1] =
        -std::numeric_limits<Scalar>::max();
      itr_gcp_measure->second.estimate_pos[2] =
        -std::numeric_limits<Scalar>::max();
      gcps_table_widget_->SetGCPType(itr_gcp_measure->first,
                                     GCPsTableWidget::GCPEntry::NOT_USED);
      gcps_table_widget_->SetGCPTypeEditable(itr_gcp_measure->first, false);
    }
    auto itr_gcp_response = response_photo_measure.gcp_measures.begin();
    auto itr_gcp_response_end = response_photo_measure.gcp_measures.end();
    for (; itr_gcp_response != itr_gcp_response_end; ++itr_gcp_response)
    {
      if (gcp_measures_.find(uint(itr_gcp_response->first)) ==
          gcp_measures_.end())
      {
        return;
      }
      GCPMeasure& gcp_measure = gcp_measures_[uint(itr_gcp_response->first)];
      gcp_measure.gcp_id = uint(itr_gcp_response->second.gcp_id);
      gcp_measure.type = itr_gcp_response->second.type;
      gcps_table_widget_->SetGCPType(gcp_measure.gcp_id, gcp_measure.type);
      auto itr_photo_measure = itr_gcp_response->second.photo_measures.begin();
      auto itr_photo_measure_end =
        itr_gcp_response->second.photo_measures.end();
      for (; itr_photo_measure != itr_photo_measure_end; ++itr_photo_measure)
      {
        PhotoMeasure photo_measure;
        photo_measure.photo_id = uint(itr_photo_measure->second.photo_id);
        photo_measure.photo_measure_id =
          uint(itr_photo_measure->second.photo_measure_id);
        photo_measure.x = Scalar(itr_photo_measure->second.x);
        photo_measure.y = Scalar(itr_photo_measure->second.y);
        gcp_measure.photo_measures[uint(itr_photo_measure->first)] =
          photo_measure;
      }

      if (gcp_measure.photo_measures.size() >= 2)
      {
        Point3D point;
        if (TriangulatePoint(gcp_measure.photo_measures, point) == 0)
        {
          point = similar_scale_ * (similar_rotation_ * point) +
                  similar_translate_;
          gcp_measure.estimate_pos[0] = point[0];
          gcp_measure.estimate_pos[1] = point[1];
          gcp_measure.estimate_pos[2] = point[2];

          gcps_table_widget_->SetGCPTypeEditable(
            gcp_measure.gcp_id, true);
        }
      }
    }

    ComputeSimilarTransform();
  }
}

void GCPsPane::FilterPhotosByPoints(const Point3FContainer& points)
{
  PhotoEntryContainer filtered_photo_entries;
  auto itr_point = points.begin();
  auto itr_point_end = points.end();
  for (; itr_point != itr_point_end; ++itr_point)
  {
    auto itr_photo_entry = photo_entries_.begin();
    auto itr_photo_entry_end = photo_entries_.end();
    for (; itr_photo_entry != itr_photo_entry_end; ++itr_photo_entry)
    {
      EIGEN_VECTOR(Scalar, 3) point;
      point << Scalar((*itr_point)[0]),
               Scalar((*itr_point)[1]),
               Scalar((*itr_point)[2]);

      EIGEN_VECTOR(Scalar, 2) key =
        hs::sfm::ProjectiveFunctions<Scalar>::
          WorldPointProjectToImageKeyNoDistort(
            itr_photo_entry->second.intrinsic_params,
            itr_photo_entry->second.extrinsic_params,
            point);
      if (key[0] > Scalar(0) &&
          key[0] < Scalar(itr_photo_entry->second.image_width) &&
          key[1] > Scalar(0) &&
          key[1] < Scalar(itr_photo_entry->second.image_height))
      {
        key = 
          hs::sfm::ProjectiveFunctions<Scalar>::
            WorldPointProjectToImageKey(
              itr_photo_entry->second.intrinsic_params,
              itr_photo_entry->second.extrinsic_params,
              point);
        if (key[0] > Scalar(0) &&
            key[0] < Scalar(itr_photo_entry->second.image_width) &&
            key[1] > Scalar(0) &&
            key[1] < Scalar(itr_photo_entry->second.image_height))
        {
          filtered_photo_entries.insert(*itr_photo_entry);
        }
      }
    }
  }

  TiepointMeasureWidget::TiepointPhotoContainer tiepoint_photos;
  auto itr_photo_entry = filtered_photo_entries.begin();
  auto itr_photo_entry_end = filtered_photo_entries.end();
  for (; itr_photo_entry != itr_photo_entry_end; ++itr_photo_entry)
  {
    TiepointPhoto tiepoint_photo;
    tiepoint_photo.photo_id = itr_photo_entry->first;
    tiepoint_photo.photo_path =
      QString::fromLocal8Bit(itr_photo_entry->second.path.c_str());
    tiepoint_photo.thumbnail_path =
      QString::fromLocal8Bit(
        itr_photo_entry->second.thumbnail_path.c_str());
    tiepoint_photo.width = itr_photo_entry->second.image_width;
    tiepoint_photo.height = itr_photo_entry->second.image_height;

    tiepoint_photos[itr_photo_entry->first] = tiepoint_photo;
  }
  tiepoint_measure_widget_->SetTiepointPhotos(tiepoint_photos);
}

void GCPsPane::OnActionAddGCPTriggered()
{
  while(true)
  {
    //get all gcp for generate gcp name
    db::RequestGetAllGCPs request_get_gcps;
    db::ResponseGetAllGCPs response_get_gcps;
    int error =
      ((MainWindow*)parent())->database_mediator().Request(
        this, db::DatabaseMediator::REQUEST_GET_ALL_GCPS,
        request_get_gcps, response_get_gcps, true);
    //采用正则表达式寻找gcp名
    QRegExp rx("gcp\\((\\d+)\\)");
    std::set<int> gcp_default_names;
    for (auto iter = response_get_gcps.records.begin();
         iter != response_get_gcps.records.end(); ++iter)
    {
      std::string temp = iter->second[db::GroundControlPointResource::GCP_FIELD_NAME].ToString();
      QString name = QString::fromStdString(
        iter->second[db::GroundControlPointResource::GCP_FIELD_NAME].ToString());
      if (name.contains(rx))
      {
        int pos = rx.indexIn(name);
        if(pos > -1) 
        {
          name = rx.cap(1);
        }
        gcp_default_names.insert(name.toInt());
      }
    }
    
    db::RequestAddGCP request_add_gcp;
    db::ResponseAddGCP response_add_gcp;
    int num_gcp = 1;
    //如果存在gcp默认名,取set最后一个元素加1
    if (!gcp_default_names.empty())
    {
      auto iter_gcp_num = gcp_default_names.rbegin();
      num_gcp = *iter_gcp_num + 1;
    }
    
    request_add_gcp.gcp.name = "gcp(" +  std::to_string(num_gcp) + ")";
    request_add_gcp.gcp.description = "";
    request_add_gcp.gcp.x = 0;
    request_add_gcp.gcp.y = 0;
    request_add_gcp.gcp.z = 0;

    error = 
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_ADD_GCP,
      request_add_gcp, response_add_gcp, true);

    if(error != db::DatabaseMediator::NO_ERROR)
    {
      QMessageBox box;
      box.setText(tr("delete GCP error!"));
      box.exec();
      break;
    }

    if(response_add_gcp.added_records.empty())
    {
      break;
    }

    GCPsTableWidget::GCPEntry gcp;
    auto iter = response_add_gcp.added_records.begin();
    if (iter == response_add_gcp.added_records.end())
    {
      break;
    }
    
    gcp.id = iter->second[
      db::GroundControlPointResource::GCP_FIELD_ID].ToInt();
    gcp.name = QString::fromStdString(request_add_gcp.gcp.name);
    gcp.measure_pos[0] = 0;
    gcp.measure_pos[1] = 0;
    gcp.measure_pos[2] = 0;
    gcp.estimate_pos[0] = -std::numeric_limits<GCPsTableWidget::Float>::max();
    gcp.estimate_pos[1] = -std::numeric_limits<GCPsTableWidget::Float>::max();
    gcp.estimate_pos[2] = -std::numeric_limits<GCPsTableWidget::Float>::max();
    gcp.type = GCPsTableWidget::GCPEntry::NOT_USED;

    gcps_table_widget_->AddGCP(gcp);
    break;
  }
}

void GCPsPane::OnActionImportGCPTriggered()
{
  typedef PropertyFieldAsignmentDialog::AssignedFieldValues
          AssignedFieldValues;

  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Character-separated values (*.csv *.txt)"));
  if (dialog.exec())
  {
    QString file_path = dialog.selectedFiles()[0];
    std::string std_file_path = file_path.toLocal8Bit().data();
    std::ifstream file(std_file_path);
    if (file)
    {
      std::string file_content =
        std::string(std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>());
      QStringList labels;
      labels<<tr("Name")
            <<tr("X")
            <<tr("Y")
            <<tr("Z");
      PropertyFieldAsignmentDialog assignment_dialog(
        file_content, labels, this);
      if (assignment_dialog.exec())
      {
        AssignedFieldValues assigned_field_values;
        assignment_dialog.GetAssignedFieldValues(assigned_field_values);
        auto itr_name = assigned_field_values.find(labels[0]);
        if (itr_name != assigned_field_values.end())
        {
          db::RequestAddGCPs request_add_gcps;
          db::ResponseAddGCPs response_add_gcps;
          size_t number_of_gcps_to_add = itr_name->second.size();
          db::RequestAddGCPs::GCPEntry default_entry;
          default_entry.x = -std::numeric_limits<db::Database::Float>::max();
          default_entry.y = -std::numeric_limits<db::Database::Float>::max();
          default_entry.z = -std::numeric_limits<db::Database::Float>::max();
          request_add_gcps.gcps.resize(number_of_gcps_to_add, default_entry);
          for (size_t i = 0; i < number_of_gcps_to_add; i++)
          {
            request_add_gcps.gcps[i].name =
              itr_name->second[int(i)].toLocal8Bit().data();
          }
          auto itr_x = assigned_field_values.find(labels[1]);
          if (itr_x != assigned_field_values.end() &&
              itr_x->second.size() == number_of_gcps_to_add)
          {
            for (size_t i = 0; i < number_of_gcps_to_add; i++)
            {
              request_add_gcps.gcps[i].x =
                db::Database::Float(itr_x->second[int(i)].toDouble());
            }
          }
          auto itr_y = assigned_field_values.find(labels[2]);
          if (itr_y != assigned_field_values.end() &&
              itr_y->second.size() == number_of_gcps_to_add)
          {
            for (size_t i = 0; i < number_of_gcps_to_add; i++)
            {
              request_add_gcps.gcps[i].y =
                db::Database::Float(itr_y->second[int(i)].toDouble());
            }
          }
          auto itr_z = assigned_field_values.find(labels[3]);
          if (itr_z != assigned_field_values.end() &&
              itr_z->second.size() == number_of_gcps_to_add)
          {
            for (size_t i = 0; i < number_of_gcps_to_add; i++)
            {
              request_add_gcps.gcps[i].z =
                db::Database::Float(itr_z->second[int(i)].toDouble());
            }
          }
          ((MainWindow*)parent())->database_mediator().Request(
            this, db::DatabaseMediator::REQUEST_ADD_GCPS,
            request_add_gcps, response_add_gcps, true);

          auto itr_record = response_add_gcps.added_records.begin();
          auto itr_record_end = response_add_gcps.added_records.end();
          GCPsTableWidget::GCPContainer gcps;
          for (; itr_record != itr_record_end; ++itr_record)
          {
            GCPsTableWidget::GCPEntry gcp;
            gcp.id = uint(itr_record->first);
            std::string name =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_NAME].ToString();
            gcp.name = QString::fromLocal8Bit(name.c_str());
            db::Database::Float x =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_X].ToFloat();
            db::Database::Float y =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_Y].ToFloat();
            db::Database::Float z =
              itr_record->second[
                db::GroundControlPointResource::GCP_FIELD_Z].ToFloat();
            gcp.measure_pos[0] = GCPsTableWidget::Float(x);
            gcp.measure_pos[1] = GCPsTableWidget::Float(y);
            gcp.measure_pos[2] = GCPsTableWidget::Float(z);
            gcp.estimate_pos[0] =
              -std::numeric_limits<GCPsTableWidget::Float>::max();
            gcp.estimate_pos[1] =
              -std::numeric_limits<GCPsTableWidget::Float>::max();
            gcp.estimate_pos[2] =
              -std::numeric_limits<GCPsTableWidget::Float>::max();
            gcp.type = GCPsTableWidget::GCPEntry::NOT_USED;
            gcps[gcp.id] = gcp;

            GCPMeasure gcp_measure;
            gcp_measure.gcp_id = gcp.id;
            gcp_measure.type = GCPsTableWidget::GCPEntry::NOT_USED;
            gcp_measure.measure_pos[0] = gcp.measure_pos[0];
            gcp_measure.measure_pos[1] = gcp.measure_pos[1];
            gcp_measure.measure_pos[2] = gcp.measure_pos[2];
            gcp_measure.estimate_pos[0] = gcp.estimate_pos[0];
            gcp_measure.estimate_pos[1] = gcp.estimate_pos[1];
            gcp_measure.estimate_pos[2] = gcp.estimate_pos[2];
            gcp_measures_[gcp_measure.gcp_id] = gcp_measure;
          }
          gcps_table_widget_->AddGCPs(gcps);
        }
      }
    }
  }
}

void GCPsPane::OnActionShowMeasurementTriggered()
{
  gcps_table_widget_->set_display_mode(GCPsTableWidget::SHOW_MEASUREMENT);
}

void GCPsPane::OnActionShowEstimateTriggered()
{
  gcps_table_widget_->set_display_mode(GCPsTableWidget::SHOW_ESTIMATE);
}

void GCPsPane::OnActionShowErrorTriggered()
{
  gcps_table_widget_->set_display_mode(GCPsTableWidget::SHOW_ERROR);
}

void GCPsPane::OnActionGCPConstrainedOptimizeTriggered()
{
  //判断当前照片定向是否包含点云
  db::RequestGetAllPointClouds request_all_point_clouds;
  db::ResponseGetAllPointClouds response_all_point_clouds;
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_ALL_POINT_CLOUDS,
    request_all_point_clouds, response_all_point_clouds, false);
  bool has_point_cloud = false;
  for (const auto& point_cloud : response_all_point_clouds.records)
  {
    db::Identifier photo_orientation_id =
      db::Identifier(point_cloud.second[
        db::PointCloudResource::POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID].ToInt());
    if (photo_orientation_id == photo_orientation_id_)
    {
      has_point_cloud = true;
      break;
    }
  }

  if (has_point_cloud)
  {
    QMessageBox msg_box;
    msg_box.setText(tr("Current photo orientation contains some point clouds."
                       "Would you like to copy this "
                       "photo orientation to optimize?"));
    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int result = msg_box.exec();
    if (result == QMessageBox::Yes)
    {
      //Copy photo orientation
      db::RequestCopyPhotoOrientation request_copy;
      db::ResponseCopyPhotoOrientation response_copy;
      request_copy.photo_orientation_id = photo_orientation_id_;
      ((MainWindow*)parent())->database_mediator().Request(
        this, db::DatabaseMediator::REQUEST_COPY_PHOTO_ORIENTATION,
        request_copy, response_copy, true);
    }
  }
  else
  {
    ProgressDialog dialog;
    dialog.Start(&GCPsPane::GCPConstrainedOptimize,
                 this, dialog.GetProgressManagerPtr());
    db::RequestUpdatePhotoOrientationParams request_params;
    request_params.photo_orientation_id = photo_orientation_id_;
    db::ResponseUpdatePhotoOrientationParams response_params;
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_UPDATE_PHOTO_ORIENTATION_PARAMS,
      request_params, response_params, true);
  }
}

void GCPsPane::OnActionGCPConfigTriggered()
{
  GCPConstrainedOptimizationConfigDialog dialog(this, 0,
                                                gcp_planar_accuracy_,
                                                gcp_height_accuracy_,
                                                tiepoint_feature_accuracy_,
                                                gcp_marker_accuracy_);

  if (dialog.exec())
  {
    gcp_planar_accuracy_ = dialog.GetGCPPlanarAccuracy();
    gcp_height_accuracy_ = dialog.GetGCPHeightAccuracy();
    tiepoint_feature_accuracy_ = dialog.GetTiepointFeatureAccuracy();
    gcp_marker_accuracy_ = dialog.GetGCPMarkerAccuracy();
  }
}

void GCPsPane::OnActionTableItemClicked(QTableWidgetItem * item)
{
  action_remove_gcps_->setEnabled(true);
}

void GCPsPane::OnPhotoMeasured(uint photo_id, const Point2F& image_pos)
{
  typedef db::Database::Identifier Identifier;
  auto itr_gcp_measure = gcp_measures_.find(current_gcp_id_);
  if (itr_gcp_measure != gcp_measures_.end())
  {
    auto& photo_measures = itr_gcp_measure->second.photo_measures;
    auto itr_photo_measure = photo_measures.find(photo_id);
    if (itr_photo_measure == photo_measures.end())
    {
      PhotoMeasure photo_measure;
      photo_measure.photo_id = photo_id;
      photo_measure.x = Scalar(image_pos[0]);
      photo_measure.y = Scalar(image_pos[1]);

      //该像控点照片测量数据不存在，需添加
      db::RequestAddPhotoMeasure request_add_photo_measure;
      db::ResponseAddPhotoMeasure response_add_photo_measure;
      request_add_photo_measure.photo_orientation_id =
        Identifier(photo_orientation_id_);
      request_add_photo_measure.gcp_id = Identifier(current_gcp_id_);
      request_add_photo_measure.photo_id = Identifier(photo_id);
      request_add_photo_measure.gcp_type = itr_gcp_measure->second.type;
      request_add_photo_measure.x = db::Database::Float(image_pos[0]);
      request_add_photo_measure.y = db::Database::Float(image_pos[1]);
      ((MainWindow*)parent())->database_mediator().Request(
        this, db::DatabaseMediator::REQUEST_ADD_PHOTO_MEASURE,
        request_add_photo_measure, response_add_photo_measure, true);
      if (response_add_photo_measure.error_code ==
          db::DatabaseMediator::NO_ERROR)
      {
        photo_measure.photo_measure_id =
          response_add_photo_measure.photo_measure_id;
        photo_measures[photo_id] = photo_measure;
      }
    }
    else
    {
      //该像控点照片测量数据已存在，需更新
      db::RequestUpdatePhotoMeasurePos request_update;
      db::ResponseUpdatePhotoMeasurePos response_update;
      request_update.photo_measure_id =
        itr_photo_measure->second.photo_measure_id;
      request_update.x = db::Database::Float(image_pos[0]);
      request_update.y = db::Database::Float(image_pos[1]);

      ((MainWindow*)parent())->database_mediator().Request(
        this, db::DatabaseMediator::REQUEST_UPDATE_PHOTO_MEASURE_POS,
        request_update, response_update, true);
      if (response_update.error_code == db::DatabaseMediator::NO_ERROR)
      {
        itr_photo_measure->second.x = Scalar(image_pos[0]);
        itr_photo_measure->second.y = Scalar(image_pos[1]);
      }
    }

    if (photo_measures.size() >= 2)
    {
      Point3D point;
      if (TriangulatePoint(photo_measures, point) == 0)
      {
        point = similar_scale_ * (similar_rotation_ * point) +
                similar_translate_;
        itr_gcp_measure->second.estimate_pos[0] = point[0];
        itr_gcp_measure->second.estimate_pos[1] = point[1];
        itr_gcp_measure->second.estimate_pos[2] = point[2];

        gcps_table_widget_->SetGCPTypeEditable(
          itr_gcp_measure->second.gcp_id, true);
      }
    }

    ComputeSimilarTransform();
    UpdateTiepointWidget();
  }
}

void GCPsPane::OnPhotoMeasureDeleted(uint photo_id)
{
  auto itr_gcp_measure = gcp_measures_.find(current_gcp_id_);
  if (itr_gcp_measure != gcp_measures_.end())
  {
    auto& photo_measures = itr_gcp_measure->second.photo_measures;
    auto itr_photo_measure = photo_measures.find(photo_id);
    if (itr_photo_measure != photo_measures.end())
    {
      photo_measures.erase(itr_photo_measure);

      db::RequestDeletePhotoMeasure request;
      db::ResponseDeletePhotoMeasure response;
      request.gcp_id = db::Identifier(current_gcp_id_);
      request.photo_id = db::Identifier(photo_id);
      request.photo_orientation_id = db::Identifier(photo_orientation_id_);
      ((MainWindow*)parent())->database_mediator().Request(
        this, db::DatabaseMediator::REQUEST_DELETE_PHOTO_MEASURE,
        request, response, true);
    }

    if (photo_measures.size() >= 2)
    {
      Point3D point;
      if (TriangulatePoint(photo_measures, point) == 0)
      {
        point = similar_scale_ * (similar_rotation_ * point) +
                similar_translate_;
        itr_gcp_measure->second.estimate_pos[0] = point[0];
        itr_gcp_measure->second.estimate_pos[1] = point[1];
        itr_gcp_measure->second.estimate_pos[2] = point[2];

        gcps_table_widget_->SetGCPTypeEditable(
          itr_gcp_measure->second.gcp_id, true);
      }
    }

    ComputeSimilarTransform();
    UpdateTiepointWidget();
  }
}

void GCPsPane::OnGCPsSelected(const std::vector<uint>& gcp_ids)
{
  if (gcp_ids.size() == 1)
  {
    const GCPMeasure& gcp_measure = gcp_measures_[gcp_ids[0]];
    current_gcp_id_ = gcp_ids[0];
    UpdateTiepointWidget();
  }
}

void GCPsPane::OnGCPsDeleteed(const std::vector<uint>& gcp_ids)
{
  if (!gcp_ids.empty())
  {
    db::RequestDeleteGCPs request;
    db::ResponseDeleteGCPs response;
    request.gcp_ids = gcp_ids;
    int error = 
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_DELETE_GCPS,
      request, response, true);
    if (error != db::DatabaseMediator::NO_ERROR)
    {
      QMessageBox box;
      box.setText(tr("delete GCP error!"));
      box.exec();
    }  
  }
}

void GCPsPane::OnGCPUpdated(uint gcp_id,
                            const GCPsTableWidget::GCPEntry& gcp_updated)
{
  db::RequestUpdateGCP request;
  db::ResponseUpdateGCP response;
  request.id = db::Identifier(gcp_id);
  request.name = gcp_updated.name.toLocal8Bit().data();
  request.x = db::Float(gcp_updated.measure_pos[0]);
  request.y = db::Float(gcp_updated.measure_pos[1]);
  request.z = db::Float(gcp_updated.measure_pos[2]);
  request.type = gcp_updated.type;
  if (photo_orientation_id_ != std::numeric_limits<uint>::max())
  {
    request.photo_orientation_id = db::Identifier(photo_orientation_id_);
    request.update_photo_measure = true;
  }
  else
  {
    request.update_photo_measure = false;
  }
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_UPDATE_GCP,
    request, response, true);

  auto itr_gcp_measure = gcp_measures_.find(gcp_id);
  if (itr_gcp_measure != gcp_measures_.end())
  {
    itr_gcp_measure->second.measure_pos[0] = gcp_updated.measure_pos[0];
    itr_gcp_measure->second.measure_pos[1] = gcp_updated.measure_pos[1];
    itr_gcp_measure->second.measure_pos[2] = gcp_updated.measure_pos[2];
    itr_gcp_measure->second.estimate_pos[0] = gcp_updated.estimate_pos[0];
    itr_gcp_measure->second.estimate_pos[1] = gcp_updated.estimate_pos[1];
    itr_gcp_measure->second.estimate_pos[2] = gcp_updated.estimate_pos[2];
    itr_gcp_measure->second.type = gcp_updated.type;
  }

  ComputeSimilarTransform();
  UpdateTiepointWidget();
}

void GCPsPane::UpdateTiepointWidget()
{
  typedef EIGEN_VECTOR(Scalar, 2) Key;
  const auto itr_gcp_measure = gcp_measures_.find(current_gcp_id_);
  if (itr_gcp_measure == gcp_measures_.end()) return;
  const GCPMeasure& gcp_measure = gcp_measures_[current_gcp_id_];
  if (gcp_measure.estimate_pos[0] > -1e100 &&
      gcp_measure.estimate_pos[1] > -1e100 &&
      gcp_measure.estimate_pos[2] > -1e100)
  {
    //像控点已存在相对座标，计算相关联的照片
    Point3D point;
    point << gcp_measure.estimate_pos[0],
             gcp_measure.estimate_pos[1],
             gcp_measure.estimate_pos[2];
    TiepointMeasureWidget::TiepointPhotoContainer tiepoint_photos;
    auto itr_photo_entry = photo_entries_.begin();
    auto itr_photo_entry_end = photo_entries_.end();
    for (; itr_photo_entry != itr_photo_entry_end; ++itr_photo_entry)
    {
      TiepointPhoto tiepoint_photo;
      tiepoint_photo.photo_id = itr_photo_entry->first;
      tiepoint_photo.photo_path =
        QString::fromLocal8Bit(itr_photo_entry->second.path.c_str());
      tiepoint_photo.thumbnail_path =
        QString::fromLocal8Bit(
          itr_photo_entry->second.thumbnail_path.c_str());
      tiepoint_photo.width = itr_photo_entry->second.image_width;
      tiepoint_photo.height = itr_photo_entry->second.image_height;

      bool is_add = false;;

      auto itr_photo_measure =
        gcp_measure.photo_measures.find(itr_photo_entry->first);
      if (itr_photo_measure != gcp_measure.photo_measures.end())
      {
        tiepoint_photo.measured_pos[0] =
          TiepointMeasureWidget::Float(itr_photo_measure->second.x);
        tiepoint_photo.measured_pos[1] =
          TiepointMeasureWidget::Float(itr_photo_measure->second.y);
        is_add = true;
      }
      ExtrinsicParams extrinsic_params;
      extrinsic_params.rotation() =
        itr_photo_entry->second.extrinsic_params.rotation() *
        similar_rotation_.Inverse();
      extrinsic_params.position() =
        similar_scale_ * (similar_rotation_ *
                          itr_photo_entry->second.extrinsic_params.position()) +
        similar_translate_;
      Key key =
        hs::sfm::ProjectiveFunctions<Scalar>::
          WorldPointProjectToImageKeyNoDistort(
            itr_photo_entry->second.intrinsic_params,
            extrinsic_params,
            point);
      if (key[0] > Scalar(0) &&
          key[0] < Scalar(itr_photo_entry->second.image_width) &&
          key[1] > Scalar(0) &&
          key[1] < Scalar(itr_photo_entry->second.image_height))
      {
        key = hs::sfm::ProjectiveFunctions<Scalar>::WorldPointProjectToImageKey(
          itr_photo_entry->second.intrinsic_params,
          extrinsic_params,
          point);
        if (key[0] > Scalar(0) &&
            key[0] < Scalar(itr_photo_entry->second.image_width) &&
            key[1] > Scalar(0) &&
            key[1] < Scalar(itr_photo_entry->second.image_height))
        {
          tiepoint_photo.predicated_pos[0] =
            TiepointMeasureWidget::Float(key[0]);
          tiepoint_photo.predicated_pos[1] =
            TiepointMeasureWidget::Float(key[1]);

          is_add = true;
        }
      }
      if (is_add)
      {
        tiepoint_photos[itr_photo_entry->first] = tiepoint_photo;
      }
    }
    tiepoint_measure_widget_->SetTiepointPhotos(tiepoint_photos);
    emit GCPRelateLocationState(true);
  }
  else
  {
    //显示所有照片
    TiepointMeasureWidget::TiepointPhotoContainer tiepoint_photos;
    auto itr_photo_entry = photo_entries_.begin();
    auto itr_photo_entry_end = photo_entries_.end();
    for (; itr_photo_entry != itr_photo_entry_end; ++itr_photo_entry)
    {
      TiepointPhoto tiepoint_photo;
      tiepoint_photo.photo_id = itr_photo_entry->first;
      tiepoint_photo.photo_path =
        QString::fromLocal8Bit(itr_photo_entry->second.path.c_str());
      tiepoint_photo.thumbnail_path =
        QString::fromLocal8Bit(
          itr_photo_entry->second.thumbnail_path.c_str());
      tiepoint_photo.width = itr_photo_entry->second.image_width;
      tiepoint_photo.height = itr_photo_entry->second.image_height;

      auto itr_photo_measure =
        gcp_measure.photo_measures.find(itr_photo_entry->first);
      if (itr_photo_measure != gcp_measure.photo_measures.end())
      {
        tiepoint_photo.measured_pos[0] =
          TiepointMeasureWidget::Float(itr_photo_measure->second.x);
        tiepoint_photo.measured_pos[1] =
          TiepointMeasureWidget::Float(itr_photo_measure->second.y);
      }

      tiepoint_photos[itr_photo_entry->first] = tiepoint_photo;
    }
    tiepoint_measure_widget_->SetTiepointPhotos(tiepoint_photos);
    emit GCPRelateLocationState(false);
  }
}

int GCPsPane::TriangulatePoint(
  const std::map<uint, PhotoMeasure>& photo_measures,
  Point3D& point) const
{
  typedef hs::sfm::triangulate::MultipleViewMaximumLikelihoodEstimator<Scalar>
          Estimator;
  typedef Estimator::IntrinsicParamsContainer IntrinsicParamsContainer;
  typedef Estimator::ExtrinsicParamsContainer ExtrinsicParamsContainer;
  typedef Estimator::Key Key;
  typedef Estimator::KeyContainer KeyContainer;
  //三角化计算相对座标
  IntrinsicParamsContainer intrinsic_params_set;
  ExtrinsicParamsContainer extrinsic_params_set;
  KeyContainer keys;

  auto itr_photo_measure = photo_measures.begin();
  auto itr_photo_measure_end = photo_measures.end();
  for (; itr_photo_measure != itr_photo_measure_end; ++itr_photo_measure)
  {
    auto itr_photo_entry = photo_entries_.find(itr_photo_measure->first);
    if (itr_photo_entry == photo_entries_.end())
    {
      return -1;
    }

    intrinsic_params_set.push_back(
      itr_photo_entry->second.intrinsic_params);
    extrinsic_params_set.push_back(
      itr_photo_entry->second.extrinsic_params);
    Key key;
    key << itr_photo_measure->second.x,
           itr_photo_measure->second.y;
    keys.push_back(key);
  }

  Estimator estimator;
  if (estimator(intrinsic_params_set, extrinsic_params_set,
                keys, point) != 0)
  {
    return -1;
  }
  return 0;
}

int GCPsPane::ComputeSimilarTransform()
{
  typedef hs::sfm::SimilarTransformEstimator<Scalar> Estimator;
  typedef Estimator::PointContainer PointContainer;
  typedef EIGEN_MATRIX(Scalar, 3, 3) RotationMatrix;

  int result = -1;
  while (1)
  {
    if (is_gcp_constrained_)
    {
      result = 0;
      action_gcp_constrained_optimize_->setEnabled(false);
      auto itr_gcp_measure = gcp_measures_.begin();
      auto itr_gcp_measure_end = gcp_measures_.end();
      for (; itr_gcp_measure != itr_gcp_measure_end; ++itr_gcp_measure)
      {
        gcps_table_widget_->UpdateGCPEstimatePos(
          itr_gcp_measure->first,
          itr_gcp_measure->second.estimate_pos[0],
          itr_gcp_measure->second.estimate_pos[1],
          itr_gcp_measure->second.estimate_pos[2]);
      }
      break;
    }

    auto itr_gcp_measure = gcp_measures_.begin();
    auto itr_gcp_measure_end = gcp_measures_.end();
    PointContainer points_relative;
    PointContainer points_absolute;
    for (; itr_gcp_measure != itr_gcp_measure_end; ++itr_gcp_measure)
    {
      if (itr_gcp_measure->second.type ==
          GCPsTableWidget::GCPEntry::CALCULATE_POINT &&
          itr_gcp_measure->second.photo_measures.size() >= 2)
      {
        Point3D point_relative;
        if (TriangulatePoint(itr_gcp_measure->second.photo_measures,
                             point_relative) == 0)
        {
          point_relative =
            similar_scale_ * (similar_rotation_ * point_relative) +
            similar_translate_;
          points_relative.push_back(point_relative);
          Point3D point_absolute;
          point_absolute << itr_gcp_measure->second.measure_pos[0],
                            itr_gcp_measure->second.measure_pos[1],
                            itr_gcp_measure->second.measure_pos[2];
          points_absolute.push_back(point_absolute);
        }
      }
    }

    Estimator estimator;
    Scalar similar_scale;
    Rotation similar_rotation;
    Point3D similar_translate;
    if (estimator(points_relative, points_absolute,
                  similar_rotation, similar_translate, similar_scale) != 0)
    {
      action_gcp_constrained_optimize_->setEnabled(false);
      break;
    }
    else
    {
      action_gcp_constrained_optimize_->setEnabled(true);
    }

    //apply similar transform
    similar_scale_ *= similar_scale;
    RotationMatrix R0 = similar_rotation_;
    RotationMatrix R1 = similar_rotation;
    similar_rotation_ = R1 * R0;
    similar_translate_ =
      similar_scale * (similar_rotation * similar_translate_) +
      similar_translate;

    //Update similar transform
    db::RequestUpdatePhotoOrientationTransform transform_request;
    db::ResponseUpdatePhotoOrientationTransform transform_response;
    transform_request.id = photo_orientation_id_;
    transform_request.scale = similar_scale_;
    transform_request.rotation_x = similar_rotation_[0];
    transform_request.rotation_y = similar_rotation_[1];
    transform_request.rotation_z = similar_rotation_[2];
    transform_request.translate_x = similar_translate_[0];
    transform_request.translate_y = similar_translate_[1];
    transform_request.translate_z = similar_translate_[2];
    ((MainWindow*)parent())->database_mediator().Request(
      this, db::DatabaseMediator::REQUEST_UPDATE_PHOTO_ORIENTATION_TRANSFORM,
      transform_request, transform_response, true);

    //Set GCP estimate pos
    itr_gcp_measure = gcp_measures_.begin();
    itr_gcp_measure_end = gcp_measures_.end();
    for (; itr_gcp_measure != itr_gcp_measure_end; ++itr_gcp_measure)
    {
      if (itr_gcp_measure->second.photo_measures.size() < 2)
      {
        itr_gcp_measure->second.estimate_pos[0] =
          itr_gcp_measure->second.measure_pos[0];
        itr_gcp_measure->second.estimate_pos[1] =
          itr_gcp_measure->second.measure_pos[1];
        itr_gcp_measure->second.estimate_pos[2] =
          itr_gcp_measure->second.measure_pos[2];
      }
      else
      {
        Point3D point;
        if (TriangulatePoint(itr_gcp_measure->second.photo_measures,
                             point) == 0)
        {
          point = similar_scale_ * (similar_rotation_ * point) +
                  similar_translate_;
          itr_gcp_measure->second.estimate_pos[0] = point[0];
          itr_gcp_measure->second.estimate_pos[1] = point[1];
          itr_gcp_measure->second.estimate_pos[2] = point[2];
          gcps_table_widget_->SetGCPTypeEditable(
            itr_gcp_measure->second.gcp_id, true);
        }
        else
        {
          itr_gcp_measure->second.estimate_pos[0] =
            itr_gcp_measure->second.measure_pos[0];
          itr_gcp_measure->second.estimate_pos[1] =
            itr_gcp_measure->second.measure_pos[1];
          itr_gcp_measure->second.estimate_pos[2] =
            itr_gcp_measure->second.measure_pos[2];
        }
      }

      gcps_table_widget_->UpdateGCPEstimatePos(
        itr_gcp_measure->first,
        itr_gcp_measure->second.estimate_pos[0],
        itr_gcp_measure->second.estimate_pos[1],
        itr_gcp_measure->second.estimate_pos[2]);
    }

    result = 0;
    break;
  }

  return result;
}

void GCPsPane::GCPConstrainedOptimize(
  hs::progress::ProgressManager* progress_manager)
{
  typedef hs::sfm::pipeline::BundleAdjustmentGCPConstrainedOptimizor < Scalar >
          Optimizor;
  typedef Optimizor::ExtrinsicParams ExtrinsicParams;
  typedef Optimizor::IntrinsicParams IntrinsicParams;
  typedef Optimizor::IntrinsicParamsContainer IntrinsicParamsContainer;
  typedef Optimizor::ExtrinsicParamsContainer ExtrinsicParamsContainer;
  typedef Optimizor::ImageKeyset ImageKeyset;
  typedef ImageKeyset::Key Key;
  typedef Optimizor::ImageKeysetContainer ImageKeysetContainer;
  typedef Optimizor::Point Point;
  typedef Optimizor::PointContainer PointContainer;
  typedef Optimizor::TrackPointMap TrackPointMap;
  typedef Optimizor::ImageIntrinsicMap ImageIntrinsicMap;
  typedef Optimizor::ImageExtrinsicMap ImageExtrinsicMap;
  typedef hs::sfm::Track Track;
  typedef hs::sfm::TrackContainer TrackContainer;
  typedef hs::sfm::ViewInfoIndexer ViewInfoIndexer;
  typedef hs::graphics::PointCloudData<Scalar> PointCloudData;
  typedef hs::recon::db::Identifier Identifier;
  typedef EIGEN_STD_MAP(size_t, ImageKeyset) ImageKeysetMap;
  typedef hs::sfm::pipeline::PointCloudNormCalculator<Scalar> NormCalculator;

  if (progress_manager)
  {
    progress_manager->AddSubProgress(0.1f);
  }

  //Get photo orientation
  hs::recon::db::RequestGetPhotoOrientation request_photo_orientation;
  hs::recon::db::ResponseGetPhotoOrientation response_photo_orientation;
  request_photo_orientation.id =
    db::Database::Identifier(photo_orientation_id_);
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_GET_PHOTO_ORIENTATION,
    request_photo_orientation, response_photo_orientation, false);
  if (response_photo_orientation.error_code !=
      hs::recon::db::Database::NO_ERROR)
  {
    return;
  }

  //Get feature match
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
  if (response_feature_match.error_code != hs::recon::db::Database::NO_ERROR)
  {
    return;
  }

  //Compute offset
  Point offset = Point::Zero();
  for (const auto& gcp_measure : gcp_measures_)
  {
    offset[0] += gcp_measure.second.measure_pos[0];
    offset[1] += gcp_measure.second.measure_pos[1];
    offset[2] += gcp_measure.second.measure_pos[2];
  }
  offset /= Scalar(gcp_measures_.size());

  //Get image_keysets image_intrinsic_map image_extrinsic_map
  //intrinsic_params_set extrinsic_params_set

  ImageKeysetContainer image_keysets;
  ImageIntrinsicMap image_intrinsic_map;
  ImageExtrinsicMap image_extrinsic_map;
  IntrinsicParamsContainer intrinsic_params_set;
  ExtrinsicParamsContainer extrinsic_params_set;

  std::map<size_t, size_t> reordered_image_ids;
  std::string keysets_path = response_feature_match.keysets_path;
  ImageKeysetMap keyset_map;
  {
    std::ifstream keysets_file(keysets_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(keysets_file);
    archive(keyset_map);
  }
  auto itr_photo = photo_entries_.begin();
  auto itr_photo_end = photo_entries_.end();
  for (size_t i = 0; itr_photo != itr_photo_end; ++itr_photo, ++i)
  {
    auto itr_image_keyset = keyset_map.find(size_t(itr_photo->first));
    if (itr_image_keyset == keyset_map.end()) return;
    image_keysets.push_back(itr_image_keyset->second);

    size_t intrinsic_id = itr_photo->second.intrinsic_id;
    auto itr_intrinsic = intrinsic_params_set_.find(intrinsic_id);
    if (itr_intrinsic == intrinsic_params_set_.end())
    {
      return;
    }

    image_extrinsic_map.AddObject(i);
    ExtrinsicParams extrinsic_params_abs = itr_photo->second.extrinsic_params;
    extrinsic_params_abs.rotation() =
      extrinsic_params_abs.rotation() * similar_rotation_.Inverse();
    extrinsic_params_abs.position() =
      similar_scale_ * (similar_rotation_ * extrinsic_params_abs.position()) +
      (similar_translate_ - offset);
    extrinsic_params_set.push_back(extrinsic_params_abs);
    image_intrinsic_map.AddObject(
      size_t(std::distance(intrinsic_params_set_.begin(), itr_intrinsic)));
    reordered_image_ids[size_t(itr_photo->first)] = i;
  }
  auto itr_intrinsic = intrinsic_params_set_.begin();
  auto itr_intrinsic_end = intrinsic_params_set_.end();
  for (; itr_intrinsic != itr_intrinsic_end; ++itr_intrinsic)
  {
    intrinsic_params_set.push_back(itr_intrinsic->second);
  }

  //Get tracks track_point_map view_info_indexer
  TrackContainer tracks;
  TrackPointMap track_point_map;
  ViewInfoIndexer view_info_indexer;
  {
    std::string tracks_path = response_photo_orientation.tracks_path;
    std::ifstream tracks_file(tracks_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(tracks_file);
    archive(tracks, track_point_map);
  }
  for (size_t i = 0; i < tracks.size(); i++)
  {
    for (size_t j = 0; j < tracks[i].size(); j++)
    {
      size_t image_id = tracks[i][j].first;
      tracks[i][j].first = reordered_image_ids[image_id];
    }
  }
  view_info_indexer.SetViewInfoByTracks(tracks);

  //Get points
  PointCloudData pcd;
  {
    std::ifstream point_cloud_file(response_photo_orientation.point_cloud_path,
                                   std::ios::binary);
    cereal::PortableBinaryInputArchive archive(point_cloud_file);
    archive(pcd);
  }

  PointContainer points(pcd.VertexData().size());
  for (size_t i = 0; i < pcd.VertexData().size(); i++)
  {
    points[i][0] = pcd.VertexData()[i][0];
    points[i][1] = pcd.VertexData()[i][1];
    points[i][2] = pcd.VertexData()[i][2];

    points[i] =
      similar_scale_ * (similar_rotation_ * points[i]) +
      (similar_translate_ - offset);
  }

  //Get image_keysets_gcp tracks_gcp gcps_measure
  ImageKeysetContainer image_keysets_gcp(image_keysets.size());
  TrackContainer tracks_gcp;
  PointContainer gcps_measure;
  auto itr_gcp_measure = gcp_measures_.begin();
  auto itr_gcp_measure_end = gcp_measures_.end();
  for (; itr_gcp_measure != itr_gcp_measure_end; itr_gcp_measure++)
  {
    if (itr_gcp_measure->second.type ==
        GCPsTableWidget::GCPEntry::CALCULATE_POINT &&
        itr_gcp_measure->second.photo_measures.size() >= 2)
    {
      Track track_gcp;
      auto itr_photo_measure = itr_gcp_measure->second.photo_measures.begin();
      auto itr_photo_measure_end = itr_gcp_measure->second.photo_measures.end();
      for (; itr_photo_measure != itr_photo_measure_end; ++itr_photo_measure)
      {
        auto itr_photo_entry =
          photo_entries_.find(itr_photo_measure->second.photo_id);
        if (itr_photo_entry == photo_entries_.end())
        {
          return;
        }

        size_t image_id = reordered_image_ids[size_t(itr_photo_entry->first)];
        size_t key_id = image_keysets_gcp[image_id].size();
        Key key;
        key << itr_photo_measure->second.x,
               itr_photo_measure->second.y;
        track_gcp.push_back(std::make_pair(image_id, key_id));
        image_keysets_gcp[image_id].AddKey(key);
      }
      tracks_gcp.push_back(track_gcp);
      Point gcp_measure;
      gcp_measure << itr_gcp_measure->second.measure_pos[0],
                     itr_gcp_measure->second.measure_pos[1],
                     itr_gcp_measure->second.measure_pos[2];
      gcp_measure -= offset;
      gcps_measure.push_back(gcp_measure);
    }
  }

  if (progress_manager)
  {
    progress_manager->FinishCurrentSubProgress();

    progress_manager->AddSubProgress(0.89f);
  }

  PointContainer gcps_estimate;
  TrackPointMap estimate_measure_map;
  QSettings settings;
  QString number_of_threads_key = tr("number_of_threads");
  uint number_of_threads = settings.value(number_of_threads_key,
                                          QVariant(uint(1))).toUInt();
  Optimizor optimizor(size_t(number_of_threads),
                      gcp_planar_accuracy_,
                      gcp_height_accuracy_,
                      tiepoint_feature_accuracy_,
                      gcp_marker_accuracy_);
  if (optimizor(image_keysets,
                image_intrinsic_map,
                tracks,
                image_extrinsic_map,
                track_point_map,
                view_info_indexer,
                image_keysets_gcp,
                tracks_gcp,
                gcps_measure,
                intrinsic_params_set,
                extrinsic_params_set,
                points,
                gcps_estimate,
                estimate_measure_map) != 0)
  {
    return;
  }

  if (progress_manager)
  {
    progress_manager->FinishCurrentSubProgress();
    progress_manager->AddSubProgress(0.01f);
  }

  //calculate norms.
  PointContainer norms;
  {
    NormCalculator calculator;
    Point up_vector;
    up_vector << Scalar(0),
                 Scalar(0),
                 Scalar(1);
    calculator(points, up_vector, norms);
  }

  TrackPointMap measure_estimate_map(gcps_measure.size());
  for (size_t i = 0; i < estimate_measure_map.Size(); i++)
  {
    if (estimate_measure_map.IsValid(i))
    {
      measure_estimate_map[estimate_measure_map[i]] = i;
    }
  }

  itr_intrinsic = intrinsic_params_set_.begin();
  itr_intrinsic_end = intrinsic_params_set_.end();
  for (size_t i = 0; itr_intrinsic != itr_intrinsic_end; ++i, ++itr_intrinsic)
  {
    itr_intrinsic->second = intrinsic_params_set[i];
  }

  itr_photo = photo_entries_.begin();
  itr_photo_end = photo_entries_.end();
  for (size_t i = 0; itr_photo != itr_photo_end; ++itr_photo, ++i)
  {
    itr_photo->second.extrinsic_params = extrinsic_params_set[i];
    itr_photo->second.intrinsic_params =
      intrinsic_params_set_[itr_photo->second.intrinsic_id];
  }

  similar_rotation_ = Rotation();
  similar_scale_ = Scalar(1);
  similar_translate_ = Point3D::Zero();

  itr_gcp_measure = gcp_measures_.begin();
  itr_gcp_measure_end = gcp_measures_.end();
  for (size_t i = 0; itr_gcp_measure != itr_gcp_measure_end; ++itr_gcp_measure)
  {
    if (itr_gcp_measure->second.type ==
        GCPsTableWidget::GCPEntry::CALCULATE_POINT &&
        itr_gcp_measure->second.photo_measures.size() >= 2)
    {
      if (measure_estimate_map.IsValid(i))
      {
        const Point& gcp_estmate = gcps_estimate[measure_estimate_map[i]];
        itr_gcp_measure->second.estimate_pos[0] = gcp_estmate[0] + offset[0];
        itr_gcp_measure->second.estimate_pos[1] = gcp_estmate[1] + offset[1];
        itr_gcp_measure->second.estimate_pos[2] = gcp_estmate[2] + offset[2];

        gcps_table_widget_->UpdateGCPEstimatePos(
          itr_gcp_measure->first,
          itr_gcp_measure->second.estimate_pos[0],
          itr_gcp_measure->second.estimate_pos[1],
          itr_gcp_measure->second.estimate_pos[2]);
      }
      i++;
    }
    else if (itr_gcp_measure->second.type ==
             GCPsTableWidget::GCPEntry::CHECK_POINT &&
             itr_gcp_measure->second.photo_measures.size() >= 2)
    {
      Point3D check_point_estimate;
      if (TriangulatePoint(itr_gcp_measure->second.photo_measures,
                           check_point_estimate) == 0)
      {
        itr_gcp_measure->second.estimate_pos[0] =
          check_point_estimate[0] + offset[0];
        itr_gcp_measure->second.estimate_pos[1] =
          check_point_estimate[1] + offset[1];
        itr_gcp_measure->second.estimate_pos[2] =
          check_point_estimate[2] + offset[2];
        gcps_table_widget_->UpdateGCPEstimatePos(
          itr_gcp_measure->first,
          itr_gcp_measure->second.estimate_pos[0],
          itr_gcp_measure->second.estimate_pos[1],
          itr_gcp_measure->second.estimate_pos[2]);
      }
    }
  }

  itr_photo = photo_entries_.begin();
  itr_photo_end = photo_entries_.end();
  for (size_t i = 0; itr_photo != itr_photo_end; ++itr_photo, ++i)
  {
    itr_photo->second.extrinsic_params.position() += offset;
  }

  is_gcp_constrained_ = true;

  //update database
  db::RequestUpdatePhotoOrientationTransform request_transform;
  db::ResponseUpdatePhotoOrientationTransform response_transform;
  request_transform.id = db::Identifier(photo_orientation_id_);
  request_transform.rotation_x = similar_rotation_[0];
  request_transform.rotation_y = similar_rotation_[1];
  request_transform.rotation_z = similar_rotation_[2];
  request_transform.scale = similar_scale_;
  request_transform.translate_x = similar_translate_[0];
  request_transform.translate_y = similar_translate_[1];
  request_transform.translate_z = similar_translate_[2];
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_UPDATE_PHOTO_ORIENTATION_TRANSFORM,
    request_transform, response_transform, true);

  if (response_transform.error_code != db::DatabaseMediator::NO_ERROR)
  {
    return;
  }

  db::RequestUpdatePhotoOrientationFlag request_flag;
  db::ResponseUpdatePhotoOrientationFlag response_flag;
  request_flag.id = db::Identifier(photo_orientation_id_);
  request_flag.flag =
    response_photo_orientation.record[
      db::PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FLAG].ToInt();
  request_flag.flag |= db::PhotoOrientationResource::FLAG_GEOREFERENCE;
  request_flag.flag |= db::PhotoOrientationResource::FLAG_GCP_CONSTRAINED;
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_UPDATE_PHOTO_ORIENTATION_FLAG,
    request_flag, response_flag, true);

  if (response_flag.error_code != db::DatabaseMediator::NO_ERROR)
  {
    return;
  }

  db::RequestUpdatePhotoOrientationParams request_params;
  db::ResponseUpdatePhotoOrientationParams response_params;
  request_params.photo_orientation_id = db::Identifier(photo_orientation_id_);
  request_params.intrinsic_params_map_new = intrinsic_params_set_;
  for (const auto& photo_entry : photo_entries_)
  {
    ExtrinsicIndex extrinsic_id;
    extrinsic_id.first = size_t(photo_entry.first);
    extrinsic_id.second = size_t(photo_entry.second.intrinsic_id);
    request_params.extrinsic_params_map_new[extrinsic_id] =
      photo_entry.second.extrinsic_params;
  }
  request_params.points_new = points;
  for (auto& point : request_params.points_new)
  {
    point += offset;
  }
  //TODO:No broadcast here.
  //Because It will cause opengl render in the non-main thread.
  request_params.norms_new = norms;
  ((MainWindow*)parent())->database_mediator().Request(
    this, db::DatabaseMediator::REQUEST_UPDATE_PHOTO_ORIENTATION_PARAMS,
    request_params, response_params, false);

  if (progress_manager)
  {
    progress_manager->FinishCurrentSubProgress();
  }

}

}
}
}
