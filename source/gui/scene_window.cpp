#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <algorithm>

#if 1
#include <QMessageBox>
#endif

#include "hs_graphics/graphics_utility/read_file.hpp"

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
    std::static_pointer_cast<RenderLayer>(photo_orientation_render_layer_));
  AddRenderLayer(
    std::static_pointer_cast<RenderLayer>(sparse_point_cloud_render_layer_));

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

void SceneWindow::UpdatePhotoOrientation()
{
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
    response_photo_orientation.record[
      db::PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_PATH].ToString();

  std::map<uint, IntrinsicEntry> intrinsic_entries;
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
    IntrinsicEntry intrinsic_entry;
    ss>>id>>intrinsic_entry.focal_length;

    db::RequestGetPhotogroup request_group;
    db::ResponseGetPhotogroup response_group;
    request_group.id = db::Database::Identifier(id);
    database_mediator_.Request(
      this, db::DatabaseMediator::REQUEST_GET_PHOTOGROUP,
      request_group, response_group, false);
    if (response_group.error_code != db::DatabaseMediator::NO_ERROR)
    {
      return;
    }
    intrinsic_entry.width =
      response_group.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_WIDTH].ToInt();
    intrinsic_entry.height =
      response_group.record[
        db::PhotogroupResource::PHOTOGROUP_FIELD_HEIGHT].ToInt();

    intrinsic_entries[id] = intrinsic_entry;
  }

  std::ifstream extrinsic_file(
    response_photo_orientation.extrinsic_path.c_str());
  if (!extrinsic_file) return;
  OrientationEntryContainer orientation_entries;
  while (!extrinsic_file.eof())
  {
    std::string line;
    std::getline(extrinsic_file, line);
    if (line.empty())
    {
      break;
    }
    std::stringstream ss(line);

    OrientationEntry orientation_entry;
    uint photo_id;
    uint intrinsic_id;
    ss>>photo_id>>intrinsic_id
      >>orientation_entry.extrinsic_params.position()[0]
      >>orientation_entry.extrinsic_params.position()[1]
      >>orientation_entry.extrinsic_params.position()[2]
      >>orientation_entry.extrinsic_params.rotation()[0]
      >>orientation_entry.extrinsic_params.rotation()[1]
      >>orientation_entry.extrinsic_params.rotation()[2];
    auto itr_intrinsic_entry = intrinsic_entries.find(intrinsic_id);
    if (itr_intrinsic_entry == intrinsic_entries.end()) return;
    orientation_entry.focal_length = itr_intrinsic_entry->second.focal_length;
    orientation_entry.width = itr_intrinsic_entry->second.width;
    orientation_entry.height = itr_intrinsic_entry->second.height;

    orientation_entries.push_back(orientation_entry);
  }

  //读取稀疏点云
  PointCloudData pcd;
  hs::graphics::ReadFile<Float>::ReadPointPlyFile(
    response_photo_orientation.point_cloud_path, pcd);
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
                                            Float(1.0));
  photo_orientation_render_layer_->SetCameraSize(mean_distance * Float(0.1));

  ViewAll();
  RenderNow();
}

void SceneWindow::UpdatePointCloud()
{
  std::cout << "TODO....\n";
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
