#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <algorithm>

#if 1
#include <QMessageBox>
#endif

#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "hs_graphics/graphics_utility/read_file.hpp"
#include "hs_sfm/sfm_utility/camera_type.hpp"

#include "gui/scene_window.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

SceneWindow::SceneWindow(QWindow* parent,
                         db::DatabaseMediator& database_mediator)
  : Base(parent)
  , database_mediator_(database_mediator)
  , sparse_point_cloud_render_layer_(new PointCloudRenderLayer)
  , photo_orientation_render_layer_(new PhotoOrientationRenderLayer)
  , rectangle_render_layer_(new RectangleRenderLayer)
  , is_show_photo_orientation_(true)
  , scene_flag_(FLAG_SHOW_SPARSE_POINT_CLOUD)
{
  action_filter_photos_by_selected_points_ =
    new QAction(tr("Filter Photos By Selected Points"), this);
  action_filter_photos_by_selected_points_->setEnabled(false);

  menu_context_ = new QMenu;
  menu_context_->addAction(action_filter_photos_by_selected_points_);

  AddRenderLayer(
    std::static_pointer_cast<RenderLayer>(sparse_point_cloud_render_layer_));
  AddRenderLayer(
    std::static_pointer_cast<RenderLayer>(photo_orientation_render_layer_));

  QObject::connect(this, &OpenGLWindow::MouseClicked,
                   this, &SceneWindow::OnMouseClicked);
  QObject::connect(this, &OpenGLWindow::StartMouseDragging,
                   this, &SceneWindow::OnStartMouseDragging);
  QObject::connect(this, &OpenGLWindow::MouseDragging,
                   this, &SceneWindow::OnMouseDragging);
  QObject::connect(this, &OpenGLWindow::EndMouseDragging,
                   this, &SceneWindow::OnEndMouseDragging);

  QObject::connect(action_filter_photos_by_selected_points_,
                   &QAction::triggered,
                   this,
                   &SceneWindow::OnActionFilterPhotosBySelectedPointsTriggered);
}

SceneWindow::~SceneWindow()
{
  delete menu_context_;
}

void SceneWindow::Response(int request_flag, void* response)
{

}

QAction* SceneWindow::action_filter_photos_by_selected_points()
{
  return action_filter_photos_by_selected_points_;
}

void SceneWindow::SetPhotoOrientation(uint photo_orientation_id)
{
  photo_orientation_id_ = photo_orientation_id;
  UpdatePhotoOrientation();
}

void SceneWindow::SetPointCloud(uint point_cloud_id)
{
  point_cloud_id_ = point_cloud_id;
  UpdatePointCloud();
}

void SceneWindow::OnMouseClicked(Qt::KeyboardModifiers state_key,
                                 Qt::MouseButton mouse_button, QPoint pos)
{
  if (state_key == Qt::NoModifier && mouse_button == Qt::RightButton)
  {
    QPoint qt_pos = pos;
    qt_pos.setY(height() - qt_pos.y() - 1);

    menu_context_->popup(mapToGlobal(qt_pos));
  }
  else
  {
    menu_context_->hide();
  }
}

void SceneWindow::OnStartMouseDragging(
  Qt::KeyboardModifiers state_key, Qt::MouseButton mouse_button, QPoint pos)
{
  if (state_key == Qt::AltModifier && mouse_button == Qt::LeftButton)
  {
    start_select_pos_ = pos;
    current_select_pos_ = pos;
  }
}

void SceneWindow::OnMouseDragging(
  Qt::KeyboardModifiers state_key, Qt::MouseButton mouse_button, QPoint offset)
{
  if (state_key == Qt::AltModifier && mouse_button == Qt::LeftButton)
  {
    current_select_pos_ += offset;
    Float left = Float(std::min(start_select_pos_.x(),
                                current_select_pos_.x())) /
                 Float(viewing_transformer_.viewport_width()) * Float(2) -
                 Float(1);
    Float right = Float(std::max(start_select_pos_.x(),
                                 current_select_pos_.x())) /
                  Float(viewing_transformer_.viewport_width()) * Float(2) -
                  Float(1);
    Float bottom = Float(std::min(start_select_pos_.y(),
                                  current_select_pos_.y())) /
                   Float(viewing_transformer_.viewport_height()) * Float(2) -
                   Float(1);
    Float top = Float(std::max(start_select_pos_.y(),
                               current_select_pos_.y())) /
                Float(viewing_transformer_.viewport_height()) * Float(2) -
                Float(1);
    rectangle_render_layer_->SetRectangle(left, right, bottom, top);
    rectangle_render_layer_->SetColor(Float(0.1),
                                      Float(0.8),
                                      Float(0.2),
                                      Float(0.5));

    //rectangle_render_layer_->Render();
    RenderNow();
  }
}

void SceneWindow::OnEndMouseDragging(
  Qt::KeyboardModifiers state_key, Qt::MouseButton mouse_button, QPoint pos)
{
  //if (state_key == Qt::AltModifier && mouse_button == Qt::LeftButton)
  {
    Float left = Float(std::min(start_select_pos_.x(),
                                current_select_pos_.x())) /
                 Float(viewing_transformer_.viewport_width()) * Float(2) -
                 Float(1);
    Float right = Float(std::max(start_select_pos_.x(),
                                 current_select_pos_.x())) /
                  Float(viewing_transformer_.viewport_width()) * Float(2) -
                  Float(1);
    Float bottom = Float(std::min(start_select_pos_.y(),
                                  current_select_pos_.y())) /
                   Float(viewing_transformer_.viewport_height()) * Float(2) -
                   Float(1);
    Float top = Float(std::max(start_select_pos_.y(),
                               current_select_pos_.y())) /
                Float(viewing_transformer_.viewport_height()) * Float(2) -
                Float(1);
    //TODO:TOO MANY COPYS!
    PointCloudData pcd = sparse_point_cloud_render_layer_->pcd();
    BackupSelectedPointsColor(left, right, bottom, top, pcd);

    if (!selected_points_backup_colors_.empty())
    {
      Vector3 color;
      color << Float(0.1),
               Float(0.8),
               Float(0.2);
      for (size_t i = 0; i < selected_points_backup_colors_.size(); i++)
      {
        pcd.ColorData()[selected_points_backup_colors_[i].id] =
          color;
      }
    }
    sparse_point_cloud_render_layer_->SetupPointCloudData(pcd);

    start_select_pos_ = QPoint();
    current_select_pos_ = QPoint();
    rectangle_render_layer_->ResetRectangle();
    RenderNow();
  }
}

void SceneWindow::Render()
{
  glViewport(viewing_transformer_.viewport_x(),
             viewing_transformer_.viewport_y(),
             viewing_transformer_.viewport_width(),
             viewing_transformer_.viewport_height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  for (PRenderLayer render_layer = render_layers_.head();
       render_layer != nullptr; render_layer = render_layer->next())
  {
    render_layer->Render(viewing_transformer_);
  }

  rectangle_render_layer_->Render();

  glFlush();
}

void SceneWindow::OnActionFilterPhotosBySelectedPointsTriggered()
{
  emit FilterPhotosBySelectedPoints(selected_points_);
}

struct XCompare
{
  typedef EIGEN_VECTOR(SceneWindow::Float, 3) Vector3;
  bool operator() (const Vector3& vertex1, const Vector3& vertex2) const
  {
    return vertex1[0] < vertex2[0];
  }
};

struct YCompare
{
  typedef EIGEN_VECTOR(SceneWindow::Float, 3) Vector3;
  bool operator() (const Vector3& vertex1, const Vector3& vertex2) const
  {
    return vertex1[1] < vertex2[1];
  }
};
struct ZCompare
{
  typedef EIGEN_VECTOR(SceneWindow::Float, 3) Vector3;
  bool operator() (const Vector3& vertex1, const Vector3& vertex2) const
  {
    return vertex1[2] < vertex2[2];
  }
};

struct DistanceCompare
{
  typedef EIGEN_VECTOR(SceneWindow::Float, 3) Vector3;

  DistanceCompare(const Vector3& center)
    : center_(center) {}

  bool operator() (const Vector3& vertex1, const Vector3& vertex2) const
  {
    return (vertex1 - center_).norm() < (vertex2 - center_).norm();
  }

  const Vector3& center_;
};

void SceneWindow::UpdatePhotoOrientation()
{
  typedef hs::sfm::CameraIntrinsicParams<double> SFMIntrinsicParams;
  typedef EIGEN_STD_MAP(size_t, SFMIntrinsicParams) SFMIntrinsicParamsMap;
  typedef hs::sfm::CameraExtrinsicParams<double> SFMExtrinsicParams;
  typedef std::pair<size_t, size_t> SFMExtrinsicIndex;
  typedef EIGEN_STD_MAP(SFMExtrinsicIndex, SFMExtrinsicParams)
          SFMExtrinsicParamsMap;

  hs::recon::db::RequestGetPhotoOrientation request_photo_orientation;
  hs::recon::db::ResponseGetPhotoOrientation response_photo_orientation;
  request_photo_orientation.id =
    db::Database::Identifier(photo_orientation_id_);
  database_mediator_.Request(
    this, db::DatabaseMediator::REQUEST_GET_PHOTO_ORIENTATION,
    request_photo_orientation, response_photo_orientation, false);
  if (response_photo_orientation.error_code !=
      hs::recon::db::Database::NO_ERROR)
  {
    return;
  }
  std::string photo_orientation_path =
    database_mediator_.GetPhotoOrientationPath(request_photo_orientation.id);

  SFMIntrinsicParamsMap intrinsic_map;
  {
    std::ifstream intrinsic_file(response_photo_orientation.intrinsic_path,
                                 std::ios::binary);
    cereal::PortableBinaryInputArchive archive(intrinsic_file);
    archive(intrinsic_map);
  }

  std::map<uint, IntrinsicEntry> intrinsic_entries;
  for (const auto& intrinsic_params : intrinsic_map)
  {
    db::RequestGetPhotogroup request_group;
    db::ResponseGetPhotogroup response_group;
    request_group.id = db::Database::Identifier(intrinsic_params.first);
    database_mediator_.Request(
      this, db::DatabaseMediator::REQUEST_GET_PHOTOGROUP,
      request_group, response_group, false);
    if (response_group.error_code != db::DatabaseMediator::NO_ERROR)
    {
      return;
    }
    IntrinsicEntry intrinsic_entry;
    intrinsic_entry.focal_length =
      Float(intrinsic_params.second.focal_length());
    intrinsic_entry.width =
      response_group.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_WIDTH].ToInt();
    intrinsic_entry.height =
      response_group.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_HEIGHT].ToInt();

    intrinsic_entries[uint(intrinsic_params.first)] = intrinsic_entry;
  }

  SFMExtrinsicParamsMap extrinsic_params_map;
  {
    std::ifstream extrinsic_file(response_photo_orientation.extrinsic_path,
                                 std::ios::binary);
    cereal::PortableBinaryInputArchive archive(extrinsic_file);
    archive(extrinsic_params_map);
  }

  OrientationEntryContainer orientation_entries;
  for (auto extrinsic_params : extrinsic_params_map)
  {
    OrientationEntry orientation_entry;
    auto itr_intrinsic_entry =
      intrinsic_entries.find(uint(extrinsic_params.first.second));
    if (itr_intrinsic_entry == intrinsic_entries.end()) return;
    orientation_entry.focal_length = itr_intrinsic_entry->second.focal_length;
    orientation_entry.width = itr_intrinsic_entry->second.width;
    orientation_entry.height = itr_intrinsic_entry->second.height;
    orientation_entry.extrinsic_params.position()[0] =
      Float(extrinsic_params.second.position()[0]);
    orientation_entry.extrinsic_params.position()[1] =
      Float(extrinsic_params.second.position()[1]);
    orientation_entry.extrinsic_params.position()[2] =
      Float(extrinsic_params.second.position()[2]);
    orientation_entry.extrinsic_params.rotation()[0] =
      Float(extrinsic_params.second.rotation()[0]);
    orientation_entry.extrinsic_params.rotation()[1] =
      Float(extrinsic_params.second.rotation()[1]);
    orientation_entry.extrinsic_params.rotation()[2] =
      Float(extrinsic_params.second.rotation()[2]);
    orientation_entries.push_back(orientation_entry);
  }

  //读取稀疏点云
  PointCloudData pcd;
  {
    std::ifstream point_cloud_file(response_photo_orientation.point_cloud_path,
                                   std::ios::binary);
    cereal::PortableBinaryInputArchive archive(point_cloud_file);
    archive(pcd);
  }

  ////计算中位数点
  //std::vector<Vector3> vertex_data_order = pcd.VertexData();
  //Vector3 median_point;
  //std::sort(vertex_data_order.begin(), vertex_data_order.end(), XCompare());
  //median_point[0] = vertex_data_order[vertex_data_order.size() / 2][0];
  //std::sort(vertex_data_order.begin(), vertex_data_order.end(), YCompare());
  //median_point[1] = vertex_data_order[vertex_data_order.size() / 2][1];
  //std::sort(vertex_data_order.begin(), vertex_data_order.end(), ZCompare());
  //median_point[2] = vertex_data_order[vertex_data_order.size() / 2][2];
  //std::sort(vertex_data_order.begin(), vertex_data_order.end(),
  //          DistanceCompare(median_point));
  //size_t end = size_t(std::ceil(double(median_point.size()) * 0.9));
  //Vector3 min, max;
  //min << std::numeric_limits<Float>::max(),
  //       std::numeric_limits<Float>::max(),
  //       std::numeric_limits<Float>::max();
  //max << -std::numeric_limits<Float>::max(),
  //       -std::numeric_limits<Float>::max(),
  //       -std::numeric_limits<Float>::max();
  //for (size_t i = 0; i < end; i++)
  //{
  //  min[0] = std::min(min[0], vertex_data_order[i][0]);
  //  min[1] = std::min(min[1], vertex_data_order[i][1]);
  //  min[2] = std::min(min[2], vertex_data_order[i][2]);
  //  max[0] = std::max(max[0], vertex_data_order[i][0]);
  //  max[1] = std::max(max[1], vertex_data_order[i][1]);
  //  max[2] = std::max(max[2], vertex_data_order[i][2]);
  //}
  //pcd.SetBoundingBox(min, max);

  Vector3 min, max;
  min << std::numeric_limits<Float>::max(),
         std::numeric_limits<Float>::max(),
         std::numeric_limits<Float>::max();
  max << -std::numeric_limits<Float>::max(),
         -std::numeric_limits<Float>::max(),
         -std::numeric_limits<Float>::max();
  for (const auto& point : pcd.VertexData())
  {
    min[0] = std::min(min[0], point[0]);
    min[1] = std::min(min[1], point[1]);
    min[2] = std::min(min[2], point[2]);
    max[0] = std::max(max[0], point[0]);
    max[1] = std::max(max[1], point[1]);
    max[2] = std::max(max[2], point[2]);
  }
  pcd.SetBoundingBox(min, max);

  sparse_point_cloud_render_layer_->SetupPointCloudData(pcd);

  //计算中心点，以及各张照片距离中心点的平均距离
  const auto& vertex_data = pcd.VertexData();
  Vector3 center = Vector3::Zero();
  for (size_t i = 0; i < vertex_data.size(); i++)
  {
    center += vertex_data[i];
  }
  center /= Float(vertex_data.size());

  Float mean_distance = 0;
  for (size_t i = 0; i < orientation_entries.size(); i++)
  {
    mean_distance +=
      (orientation_entries[i].extrinsic_params.position() - center).norm();
  }
  mean_distance /= Float(orientation_entries.size());

  photo_orientation_render_layer_->SetOrientationEntries(orientation_entries);
  photo_orientation_render_layer_->SetColor(Float(0.2),
                                            Float(0.3),
                                            Float(0.8),
                                            Float(0.5));
  photo_orientation_render_layer_->SetCameraSize(mean_distance * Float(0.1));

  ViewAll();
  RenderNow();
}

void SceneWindow::UpdatePointCloud()
{
  hs::recon::db::RequestGetPointCloud request_point_cloud;
  hs::recon::db::ResponseGetPointCloud response_point_cloud;
  request_point_cloud.id =
    db::Database::Identifier(point_cloud_id_);
  database_mediator_.Request(
    this, db::DatabaseMediator::REQUEST_GET_POINT_CLOUD,
    request_point_cloud, response_point_cloud, false);
  if (response_point_cloud.error_code !=
    hs::recon::db::Database::NO_ERROR)
  {
    return;
  }
  std::string dense_pointcloud_path =
    response_point_cloud.dense_pointcloud_path;

  //读取密集点云
  PointCloudData pcd;
  hs::graphics::ReadFile<Float>::ReadPointPlyFile(
    dense_pointcloud_path, pcd);
  sparse_point_cloud_render_layer_->SetupPointCloudData(pcd);

  ViewAll();
  RenderNow();

}


void SceneWindow::BackupSelectedPointsColor(Float left, Float right,
                                            Float bottom, Float top,
                                            PointCloudData& pcd)
{
  //Restore colors
  for (size_t i = 0; i < selected_points_backup_colors_.size(); i++)
  {
    pcd.ColorData()[selected_points_backup_colors_[i].id] =
      selected_points_backup_colors_[i].color;
  }

  if (left < right && bottom < top)
  {
    ViewingTransformer::TransformMatrix transform_matrix =
      viewing_transformer_.ViewingTransformMatrix();

    selected_points_backup_colors_.clear();
    selected_points_.clear();
    for (size_t i = 0; i < pcd.VertexData().size(); i++)
    {
      EIGEN_VECTOR(Float, 4) point_h;
      point_h << pcd.VertexData()[i][0],
                 pcd.VertexData()[i][1],
                 pcd.VertexData()[i][2],
                 Float(1);
      point_h = transform_matrix * point_h;
      point_h /= point_h[3];

      if (point_h[0] > left && point_h[0] < right &&
          point_h[1] > bottom && point_h[1] < top)
      {
        BackupColor backup_color;
        backup_color.id = i;
        backup_color.color = pcd.ColorData()[i];
        selected_points_backup_colors_.push_back(backup_color);
        selected_points_.push_back(pcd.VertexData()[i]);

      }
    }
  }
}

}
}
}
