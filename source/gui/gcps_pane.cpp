#include <limits>
#include <fstream>
#if 1
#include <iostream>
#include <iomanip>
#include <QMessageBox>
#endif

#include <QToolBar>
#include <QFileDialog>

#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_sfm/sfm_utility/projective_functions.hpp"
#include "hs_sfm/sfm_utility/similar_transform_estimator.hpp"
#include "hs_sfm/triangulate/multiple_view_maximum_likelihood_estimator.hpp"

#include "gui/property_field_asignment_dialog.hpp"
#include "gui/main_window.hpp"
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
  , photo_orientation_id_(std::numeric_limits<uint>::max())
  , current_gcp_id_(std::numeric_limits<uint>::max())
  , similar_scale_(Scalar(1))
  , similar_rotation_(Rotation())
  , similar_translate_(Point3D::Zero())
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

  tool_bar_ = new QToolBar(this);
  tool_bar_->addAction(action_add_gcp_);
  tool_bar_->addAction(action_remove_gcps_);
  tool_bar_->addAction(action_import_gcps_);
  tool_bar_->addAction(action_show_measurement_);
  tool_bar_->addAction(action_show_estimate_);
  tool_bar_->addAction(action_show_error_);
  tool_bar_->addAction(action_gcp_constrained_optimize_);
  main_window_->addToolBar(tool_bar_);

  QObject::connect(action_add_gcp_, &QAction::triggered,
                   this, &GCPsPane::OnActionAddGCPTriggered);
  QObject::connect(action_import_gcps_, &QAction::triggered,
                   this, &GCPsPane::OnActionImportGCPTriggered);
  QObject::connect(action_show_measurement_, &QAction::triggered,
                   this, &GCPsPane::OnActionShowMeasurementTriggered);
  QObject::connect(action_show_estimate_, &QAction::triggered,
                   this, &GCPsPane::OnActionShowEstimateTriggered);
  QObject::connect(action_show_error_, &QAction::triggered,
                   this, &GCPsPane::OnActionShowErrorTriggered);

  QObject::connect(gcps_table_widget_, &GCPsTableWidget::GCPsSelected,
                   this, &GCPsPane::OnGCPsSelected);

  QObject::connect(tiepoint_measure_widget_,
                   &TiepointMeasureWidget::TransmissionMeasured,
                   this, &GCPsPane::OnPhotoMeasured);
  QObject::connect(gcps_table_widget_, &GCPsTableWidget::GCPUpdated,
                   this, &GCPsPane::OnGCPUpdated);
}

void GCPsPane::Response(int request_flag, void* response)
{
  switch (request_flag)
  {
  case db::DatabaseMediator::REQUEST_OPEN_DATABASE:
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
  std::string photo_orientation_path =
    response_photo_orientation.record[
      db::PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_PATH].ToString();

  std::ifstream similar_file(
    response_photo_orientation.similar_transform_path.c_str());
  if (!similar_file) return;
  similar_file>>similar_scale_;
  similar_file>>similar_rotation_[0];
  similar_file>>similar_rotation_[1];
  similar_file>>similar_rotation_[2];
  similar_file>>similar_translate_[0];
  similar_file>>similar_translate_[1];
  similar_file>>similar_translate_[2];
  similar_file.close();

  std::map<uint, IntrinsicParams> intrinsic_params_set;
  std::ifstream intrinsic_file(
    response_photo_orientation.intrinsic_path.c_str());
  if (!intrinsic_file) return;
  while (!intrinsic_file.eof())
  {
    std::string line;
    std::getline(intrinsic_file, line);
    if (line.empty())
    {
      break;
    }
    std::stringstream ss(line);
    uint id;
    Scalar focal_length;
    Scalar skew;
    Scalar principal_x;
    Scalar principal_y;
    Scalar pixel_ratio;
    Scalar k1;
    Scalar k2;
    Scalar k3;
    Scalar d1;
    Scalar d2;
    ss>>id>>focal_length>>skew>>principal_x>>principal_y>>pixel_ratio
      >>k1>>k2>>k3>>d1>>d2;
    IntrinsicParams intrinsic_params(focal_length,
                                     skew,
                                     principal_x,
                                     principal_y,
                                     pixel_ratio,
                                     k1, k2, k3, d1, d2);

    intrinsic_params_set[id] = intrinsic_params;
  }

  std::ifstream extrinsic_file(
    response_photo_orientation.extrinsic_path.c_str());
  if (!extrinsic_file) return;
  hs::imgio::whole::ImageIO image_io;
  while (!extrinsic_file.eof())
  {
    std::string line;
    std::getline(extrinsic_file, line);
    if (line.empty())
    {
      break;
    }
    std::stringstream ss(line);

    PhotoEntry photo_entry;
    uint photo_id;
    uint intrinsic_id;
    ss>>photo_id>>intrinsic_id
      >>photo_entry.extrinsic_params.position()[0]
      >>photo_entry.extrinsic_params.position()[1]
      >>photo_entry.extrinsic_params.position()[2]
      >>photo_entry.extrinsic_params.rotation()[0]
      >>photo_entry.extrinsic_params.rotation()[1]
      >>photo_entry.extrinsic_params.rotation()[2];
    if (photo_entry.extrinsic_params.position()[0] == 0 &&
        photo_entry.extrinsic_params.position()[1] == 0 &&
        photo_entry.extrinsic_params.position()[2] == 0 &&
        photo_entry.extrinsic_params.rotation()[0] == 0 &&
        photo_entry.extrinsic_params.rotation()[1] == 0 &&
        photo_entry.extrinsic_params.rotation()[2] == 0)
    {
      continue;
    }
    auto itr_intrinsic_params = intrinsic_params_set.find(intrinsic_id);
    if (itr_intrinsic_params == intrinsic_params_set.end()) return;
    photo_entry.intrinsic_params = itr_intrinsic_params->second;

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
      response_photo.record[
        db::PhotoResource::PHOTO_FIELD_THUMBNAIL_PATH].ToString();
    size_t width, height;
    if (image_io.GetImageDimension(photo_entry.path, width, height) != 0)
    {
      continue;
    }
    photo_entry.image_width = int(width);
    photo_entry.image_height = int(height);

    photo_entries_[photo_id] = photo_entry;
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
        hs::sfm::ProjectiveFunctions<Scalar>::WorldPointProjectToImageKey(
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

void GCPsPane::OnGCPsSelected(const std::vector<uint>& gcp_ids)
{
  if (gcp_ids.size() == 1)
  {
    const GCPMeasure& gcp_measure = gcp_measures_[gcp_ids[0]];
    current_gcp_id_ = gcp_ids[0];
    UpdateTiepointWidget();
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
        hs::sfm::ProjectiveFunctions<Scalar>::WorldPointProjectToImageKey(
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
      break;
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

}
}
}
