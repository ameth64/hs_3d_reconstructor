#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_INFO_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_INFO_WIDGET_HPP_

#include <map>
#include <string>
#include <thread>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QLineEdit>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QHeaderView>
#include <QTimer>

#include "hs_sfm/sfm_utility/key_type.hpp"
#include "hs_sfm/sfm_utility/camera_type.hpp"
#include "hs_sfm/sfm_utility/match_type.hpp"
#include "hs_graphics/graphics_utility/pointcloud_data.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotoOrientationInfoWidget: public QWidget
{
  Q_OBJECT
public:
  typedef double Scalar;

  typedef hs::sfm::ImageKeys<Scalar> Keyset;
  typedef EIGEN_STD_MAP(size_t, Keyset) KeysetMap;

  typedef hs::sfm::CameraIntrinsicParams<Scalar> IntrinsicParams;
  typedef EIGEN_STD_MAP(size_t, IntrinsicParams) IntrinsicParamsMap;

  typedef std::pair<size_t, size_t> ExtrinsicIndex;
  typedef hs::sfm::CameraExtrinsicParams<Scalar> ExtrinsicParams;
  typedef EIGEN_STD_MAP(ExtrinsicIndex, ExtrinsicParams)
          ExtrinsicParamsMap;

  typedef hs::graphics::PointCloudData<Scalar> PointCloudData;

  PhotoOrientationInfoWidget(QWidget* parent = 0);
  ~PhotoOrientationInfoWidget();

  int Initialize(const std::string& keysets_path,
                 const std::string& intrinsic_path,
                 const std::string& extrinsic_path,
                 const std::string& sparse_point_cloud_path,
                 const std::string& tracks_path);

  void ComputeReprojectionError();

  void OnTimeOut();

protected:

private:
  QLabel* label_num_used_photo_;
  QLabel* label_reprojection_error_;
  QLabel* label_num_pointcloud_;

  QLineEdit *lineedit_num_used_photo_;
  QLineEdit *lineedit_reprojection_error_;
  QLineEdit *lineedit_num_pointcloud_;

  QVBoxLayout* layout_main_;
  QVBoxLayout* layout_general_;
  QVBoxLayout* layout_intrinsic_param_;
  QHBoxLayout* layout_num_used_photo_;
  QHBoxLayout* layout_reprojection_error_;
  QHBoxLayout* layout_num_pointcloud_;

  QSpacerItem* spacer_;

  QTreeWidget* treewidget_intrinsic_param_;

  QTimer* timer_reprojection_error_;

  IntrinsicParamsMap intrinsic_params_map_;
  ExtrinsicParamsMap extrinsic_params_map_;

  Scalar reprojection_error_;
  int reprojection_error_computed_;
  std::thread reprojection_compute_thread_;

  KeysetMap keysets_;
  PointCloudData pcd_;
  hs::sfm::TrackContainer tracks_;
  hs::sfm::ObjectIndexMap track_point_map_;
};

class IntrinsicParaminfoWidget: public QWidget
{
  Q_OBJECT
public:
  typedef PhotoOrientationInfoWidget::IntrinsicParams IntrinsicParams;

  IntrinsicParaminfoWidget(const IntrinsicParams& ip);
  ~IntrinsicParaminfoWidget();

private:
  QLabel* label_focal_length_;
  QLabel* label_principal_point_x_;
  QLabel* label_principal_point_y_;
  QLabel* label_skew_;
  QLabel* label_k1_;
  QLabel* label_k2_;
  QLabel* label_k3_;
  QLabel* label_d1_;
  QLabel* label_d2_;

  QLineEdit* lineedit_focal_length_;
  QLineEdit* lineedit_principal_point_x_;
  QLineEdit* lineedit_principal_point_y_;
  QLineEdit* lineedit_skew_;
  QLineEdit* lineedit_k1_;
  QLineEdit* lineedit_k2_;
  QLineEdit* lineedit_k3_;
  QLineEdit* lineedit_d1_;
  QLineEdit* lineedit_d2_;

  QVBoxLayout* layout_main_;
  QHBoxLayout* layout_focal_length_;
  QHBoxLayout* layout_principal_point_x_;
  QHBoxLayout* layout_principal_point_y_;
  QHBoxLayout* layout_skew_;
  QHBoxLayout* layout_k1_;
  QHBoxLayout* layout_k2_;
  QHBoxLayout* layout_k3_;
  QHBoxLayout* layout_d1_;
  QHBoxLayout* layout_d2_;


};

// class PhotoOrientationInfoWidget : public QTableWidget
// {
//   Q_OBJECT
// public:
//   typedef double Float;
//   enum PositionUnitFlag
//   {
//     POSITION_UNIT_NONE = 0,
//     POSITION_UNIT_DEGREE,
//     POSITION_UNIT_METER,
//     NUMBER_OF_POSITION_UNIT_FLAGS
//   };
//   enum RotationUnitFlag
//   {
//     ROTATION_UNIT_DEGREE = 0,
//     ROTATION_UNIT_RADIAN,
//     NUMBER_OF_ROTATION_UNIT_FLAGS
//   };
// public:
//   static QString position_unit_texts_[NUMBER_OF_POSITION_UNIT_FLAGS];
//   static QString rotation_unit_texts_[NUMBER_OF_ROTATION_UNIT_FLAGS];
//   static const int number_of_columns_ = 8;
//   static QString column_names_[number_of_columns_];
// public:
//   struct PhotoOrientationEntry
//   {
//     uint photo_id;
//     QString photo_name;
//     Float pos_x;
//     Float pos_y;
//     Float pos_z;
//     Float pitch;
//     Float roll;
//     Float heading;
//     Float reprojection_error;
//   };
//   typedef std::map<uint, PhotoOrientationEntry> PhotoOrientationEntryContainer;
//   PhotoOrientationInfoWidget(QWidget* parent = 0);
// 
//   void SetPlanarUnit(PositionUnitFlag unit);
//   void SetHeightUnit(PositionUnitFlag unit);
//   void SetRotationUnit(RotationUnitFlag unit);
//   void SetEntries(const PhotoOrientationEntryContainer& entries);
//   void Clear();
// 
// private:
//   QIcon photo_icon_;
// };

}//namespace gui
}
}

#endif
