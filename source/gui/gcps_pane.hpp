#ifndef _HS_3D_RECONSTRUCTOR_GUI_GCPS_PANE_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_GCPS_PANE_HPP_

#include <map>

#include <QAction>
#include <QIcon>

#include "hs_math/geometry/rotation.hpp"
#include "hs_sfm/sfm_utility/camera_type.hpp"
#include "hs_progress/progress_utility/progress_manager.hpp"

#include "database/database_mediator.hpp"
#include "gui/manager_pane.hpp"
#include "gui/gcps_table_widget.hpp"
#include "gui/tiepoint_measure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class GCPsPane : public ManagerPane, public hs::recon::db::DatabaseObserver
{
  Q_OBJECT
public:
  typedef double Scalar;
  typedef hs::sfm::CameraIntrinsicParams<Scalar> IntrinsicParams;
  typedef hs::sfm::CameraExtrinsicParams<Scalar> ExtrinsicParams;
  typedef EIGEN_STD_MAP(size_t, IntrinsicParams) IntrinsicParamsMap;
  typedef std::pair<size_t, size_t> ExtrinsicIndex;
  typedef EIGEN_STD_MAP(ExtrinsicIndex, ExtrinsicParams) ExtrinsicParamsMap;

  struct PhotoEntry
  {
    std::string path;
    std::string thumbnail_path;
    size_t intrinsic_id;
    IntrinsicParams intrinsic_params;
    ExtrinsicParams extrinsic_params;
    int image_width;
    int image_height;
  };
  typedef EIGEN_STD_MAP(uint, PhotoEntry) PhotoEntryContainer;

  struct PhotoMeasure
  {
    uint photo_id;
    uint photo_measure_id;
    Scalar x;
    Scalar y;
  };
  struct GCPMeasure
  {
    uint gcp_id;
    int type;
    Scalar measure_pos[3];
    Scalar estimate_pos[3];
    std::map<uint, PhotoMeasure> photo_measures;
  };
  typedef std::map<uint, GCPMeasure> GCPMeasureContainer;

  typedef hs::math::geometry::Rotation3D<Scalar> Rotation;

  typedef EIGEN_VECTOR(Scalar, 3) Point3D;

  typedef EIGEN_VECTOR(float, 3) Point3F;
  typedef EIGEN_STD_VECTOR(Point3F) Point3FContainer;

  typedef EIGEN_VECTOR(float, 2) Point2F;
  typedef EIGEN_STD_VECTOR(Point2F) Point2FContainer;
public:
  GCPsPane(QWidget* parent = 0);

  virtual void Response(int request_flag, void* response);

public slots:
  void UpdatePhotoOrientation(uint photo_orientation_id);
  void FilterPhotosByPoints(const Point3FContainer& points);

private slots:
  void OnActionAddGCPTriggered();
  void OnActionImportGCPTriggered();
  void OnActionShowMeasurementTriggered();
  void OnActionShowEstimateTriggered();
  void OnActionShowErrorTriggered();
  void OnActionGCPConstrainedOptimizeTriggered();
  void OnActionGCPConfigTriggered();
  void OnActionTableItemClicked(QTableWidgetItem * item);
  void OnPhotoMeasured(uint photo_id, const Point2F& image_pos);
  void OnPhotoMeasureDeleted(uint photo_id);
  void OnGCPsSelected(const std::vector<uint>& gcp_ids);
  void OnGCPsDeleteed(const std::vector<uint>& gcp_ids);
  void OnGCPUpdated(uint gcp_id, const GCPsTableWidget::GCPEntry& gcp_updated);

private:
  void UpdateTiepointWidget();
  int TriangulatePoint(
    const std::map<uint, PhotoMeasure>& photo_measures,
    Point3D& point) const;
  int ComputeSimilarTransform();
  void GCPConstrainedOptimize(
    hs::progress::ProgressManager* progress_manager = nullptr);

signals:
  void GCPRelateLocationState(bool is_calculated);

private:
  GCPsTableWidget* gcps_table_widget_;
  TiepointMeasureWidget* tiepoint_measure_widget_;

  bool is_gcp_constrained_;

  QIcon icon_add_gcp_;
  QIcon icon_remove_gcps_;
  QIcon icon_import_gcps_;
  QIcon icon_show_measurement_;
  QIcon icon_show_estimate_;
  QIcon icon_show_error_;
  QIcon icon_gcp_constrained_optimize_;
  QIcon icon_gcp_config_;

  QAction* action_add_gcp_;
  QAction* action_remove_gcps_;
  QAction* action_import_gcps_;
  QAction* action_show_measurement_;
  QAction* action_show_estimate_;
  QAction* action_show_error_;
  QAction* action_gcp_constrained_optimize_;
  QAction* action_gcp_config_;

  PhotoEntryContainer photo_entries_;
  GCPMeasureContainer gcp_measures_;
  IntrinsicParamsMap intrinsic_params_set_;

  uint photo_orientation_id_;
  uint current_gcp_id_;

  Scalar gcp_planar_accuracy_;
  Scalar gcp_height_accuracy_;
  Scalar tiepoint_feature_accuracy_;
  Scalar gcp_marker_accuracy_;

  Scalar similar_scale_;
  Rotation similar_rotation_;
  Point3D similar_translate_;
};

}
}
}

#endif
