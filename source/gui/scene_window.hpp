#ifndef _HS_3D_RECONSTRUCTOR_SCENE_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_SCENE_WINDOW_HPP_

#include <QAction>
#include <QMenu>

#include "hs_graphics/graphics_qt/scene_3d_opengl_window.hpp"
#include "hs_graphics/graphics_render/photo_orientation_render_layer.hpp"
#include "hs_graphics/graphics_render/point_cloud_render_layer.hpp"
#include "hs_graphics/graphics_render/rectangle_render_layer.hpp"
#include "database/database_mediator.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class SceneWindow : public hs::graphics::qt::Scene3DOpenGLWindow,
                    public hs::recon::db::DatabaseObserver
{
  Q_OBJECT
public:
  typedef hs::graphics::qt::Scene3DOpenGLWindow Base;
  typedef Base::ViewingTransformer ViewingTransformer;
  typedef Base::Float Float;
  typedef Base::RenderLayer RenderLayer;
  typedef hs::graphics::render::PointCloudRenderLayer<Float>
          PointCloudRenderLayer;
  typedef std::shared_ptr<PointCloudRenderLayer>
          PPointCloudRenderLayer;
  typedef hs::graphics::render::PhotoOrientationRenderLayer<Float>
          PhotoOrientationRenderLayer;
  typedef std::shared_ptr<PhotoOrientationRenderLayer>
          PPhotoOrientationRenderLayer;

  typedef hs::graphics::render::RectangleRenderLayer<Float>
          RectangleRenderLayer;
  typedef std::shared_ptr<RectangleRenderLayer> PRectangleRenderLayer;

  typedef PhotoOrientationRenderLayer::OrientationEntry OrientationEntry;
  typedef PhotoOrientationRenderLayer::OrientationEntryContainer
          OrientationEntryContainer;
  typedef PhotoOrientationRenderLayer::Renderer::ExtrinsicParams
          ExtrinsicParams;
  typedef PointCloudRenderLayer::PointCloudData PointCloudData;
  typedef PointCloudData::Vector3 Vector3;
  typedef EIGEN_STD_VECTOR(Vector3) PointContainer;
  struct IntrinsicEntry
  {
    Float focal_length;
    int width;
    int height;
  };

  struct BackupColor
  {
    size_t id;
    Vector3 color;
  };

public:
  enum SceneFlag
  {
    FLAG_SHOW_SPARSE_POINT_CLOUD,
    FLAG_SHOW_DENSE_POINT_CLOUD,
    FLAG_SHOW_SURFACE_MODEL,
    FLAG_SHOW_TEXTURE
  };
public:
  SceneWindow(QWindow* parent, db::DatabaseMediator& database_mediator);
  virtual ~SceneWindow();

  virtual void Response(int request_flag, void* response);

  QAction* action_filter_photos_by_selected_points();

public slots:
  void SetPhotoOrientation(uint photo_orientation_id_);
  void SetPointCloud(uint point_cloud_id);

protected slots:
  void OnMouseClicked(Qt::KeyboardModifiers state_key,
                      Qt::MouseButton mouse_button, QPoint pos);

  void OnStartMouseDragging(Qt::KeyboardModifiers state_key,
                            Qt::MouseButton mouse_button, QPoint pos);

  void OnMouseDragging(Qt::KeyboardModifiers state_key,
                       Qt::MouseButton mouse_button, QPoint offset);

  void OnEndMouseDragging(Qt::KeyboardModifiers state_key,
                          Qt::MouseButton mouse_button, QPoint pos);

protected:
  virtual void Render();

private slots:
  void OnActionFilterPhotosBySelectedPointsTriggered();

private:
  void UpdatePhotoOrientation();
  void UpdatePointCloud();
  void BackupSelectedPointsColor(Float left, Float right,
                                 Float bottom, Float top,
                                 PointCloudData& pcd);

signals:
  void FilterPhotosBySelectedPoints(const PointContainer& selected_points);

private:
  db::DatabaseMediator& database_mediator_;
  uint photo_orientation_id_;
  uint point_cloud_id_;

  bool is_show_photo_orientation_;

  int scene_flag_;

  PPointCloudRenderLayer sparse_point_cloud_render_layer_;
  PPhotoOrientationRenderLayer photo_orientation_render_layer_;
  PRectangleRenderLayer rectangle_render_layer_;

  QPoint start_select_pos_;
  QPoint current_select_pos_;

  std::vector<BackupColor> selected_points_backup_colors_;
  PointContainer selected_points_;

  QAction* action_filter_photos_by_selected_points_;
  QMenu* menu_context_;
};

}
}
}

#endif
