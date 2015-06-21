#include <fstream>

#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "hs_sfm/sfm_utility/projective_functions.hpp"

#include "photo_orientation_info_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotoOrientationInfoWidget::PhotoOrientationInfoWidget(QWidget* parent)
  : QWidget(parent)
  , reprojection_error_(0.0)
  , reprojection_error_computed_(false)
{
  layout_main_ = new QVBoxLayout;
  setLayout(layout_main_);
  layout_general_ = new QVBoxLayout;

  layout_num_used_photo_ = new QHBoxLayout;
  label_num_used_photo_ = new QLabel(tr("Number of used photo:"));
  lineedit_num_used_photo_ = new QLineEdit;
  lineedit_num_used_photo_->setReadOnly(true);
  layout_num_used_photo_->addWidget(label_num_used_photo_);
  layout_num_used_photo_->addWidget(lineedit_num_used_photo_);

  layout_num_pointcloud_ = new QHBoxLayout;
  label_num_pointcloud_ = new QLabel(tr("Number of points:"));
  lineedit_num_pointcloud_ = new QLineEdit;
  lineedit_num_pointcloud_->setReadOnly(true);
  layout_num_pointcloud_->addWidget(label_num_pointcloud_);
  layout_num_pointcloud_->addWidget(lineedit_num_pointcloud_);

  layout_reprojection_error_ = new QHBoxLayout;
  label_reprojection_error_ = new QLabel(tr("Reporjection Error:"));
  lineedit_reprojection_error_ = new QLineEdit;
  lineedit_reprojection_error_->setReadOnly(true);
  layout_reprojection_error_->addWidget(label_reprojection_error_);
  layout_reprojection_error_->addWidget(lineedit_reprojection_error_);

  layout_general_->addLayout(layout_num_used_photo_);
  layout_general_->addLayout(layout_num_pointcloud_);
  layout_general_->addLayout(layout_reprojection_error_);
  layout_main_->addLayout(layout_general_);

  spacer_ = new QSpacerItem(0,0);
  layout_main_->addItem(spacer_);

  layout_intrinsic_param_ = new QVBoxLayout;
  treewidget_intrinsic_param_ = new QTreeWidget;
  treewidget_intrinsic_param_->header()->setHidden(true);
  layout_intrinsic_param_->addWidget(treewidget_intrinsic_param_);
  layout_main_->addLayout(layout_intrinsic_param_);

  timer_reprojection_error_ = new QTimer(this);
}

PhotoOrientationInfoWidget::~PhotoOrientationInfoWidget()
{
  if (reprojection_compute_thread_.joinable())
  {
    reprojection_compute_thread_.join();
  }
}

int PhotoOrientationInfoWidget::Initialize(
  const std::string& keysets_path,
  const std::string& intrinsic_path,
  const std::string& extrinsic_path,
  const std::string& sparse_point_cloud_path,
  const std::string& tracks_path)
{
  //读取特征点集
  KeysetMap keysets;
  {
    std::ifstream keysets_file(keysets_path, std::ios::binary);
    if (!keysets_file) return -1;
    cereal::PortableBinaryInputArchive archive(keysets_file);
    archive(keysets);
  }

  //读取外参数获取num_used_photo
  {
    std::ifstream extrinsic_file(extrinsic_path, std::ios::binary);
    if(!extrinsic_file) return -1;
    cereal::PortableBinaryInputArchive archive_extrinsic(extrinsic_file);
    archive_extrinsic(extrinsic_params_map_);
    lineedit_num_used_photo_->setText(
      QString::number(extrinsic_params_map_.size()));
  }

  //读取稀疏点云获取num_pointcloud
  PointCloudData pcd;
  {
    std::ifstream sparse_point_cloud_file(
      sparse_point_cloud_path, std::ios::binary);
    if(!sparse_point_cloud_file) return -1;
    cereal::PortableBinaryInputArchive archive_sparse_point_cloud(
      sparse_point_cloud_file);
    archive_sparse_point_cloud(pcd);
    lineedit_num_pointcloud_->setText(
      QString::number(pcd.PointCloudSize()));
  }

  //读取内参数
  {
    std::ifstream intrinsic_file(intrinsic_path, std::ios::binary);
    if(!intrinsic_file) return -1;
    cereal::PortableBinaryInputArchive archive_intrinsic(intrinsic_file);
    archive_intrinsic(intrinsic_params_map_);

    treewidget_intrinsic_param_->clear();
    for (auto iter =  intrinsic_params_map_.begin();
              iter != intrinsic_params_map_.end(); ++iter)
    {
      QTreeWidgetItem* top_item = new QTreeWidgetItem;
      top_item->setText(0,tr("Intrinsic Params: ")+QString::number(iter->first));
      treewidget_intrinsic_param_->addTopLevelItem(top_item);
      QTreeWidgetItem* intrinsic_item = new QTreeWidgetItem;
      top_item->addChild(intrinsic_item);
      IntrinsicParaminfoWidget* ipiw =
        new IntrinsicParaminfoWidget(iter->second);
      treewidget_intrinsic_param_->setItemWidget(intrinsic_item, 0, ipiw);
    }
  }

  //读取tracks
  hs::sfm::TrackContainer tracks;
  hs::sfm::ObjectIndexMap track_point_map;
  {
    std::ifstream tracks_file(tracks_path, std::ios::binary);
    if (!tracks_file) return -1;
    cereal::PortableBinaryInputArchive archive(tracks_file);
    archive(tracks, track_point_map);
  }

  lineedit_reprojection_error_->setText(tr("Computing..."));
  reprojection_error_computed_ = false;
  timer_reprojection_error_->start(100);
  QObject::connect(timer_reprojection_error_, &QTimer::timeout,
                   this, &PhotoOrientationInfoWidget::OnTimeOut);

  reprojection_compute_thread_ =
    std::thread(&PhotoOrientationInfoWidget::ComputeReprojectionError,
    this, keysets, pcd, tracks, track_point_map);

  return 0;
}

void PhotoOrientationInfoWidget::ComputeReprojectionError(
  const KeysetMap& keysets,
  const PointCloudData& pcd,
  const hs::sfm::TrackContainer& tracks,
  const hs::sfm::ObjectIndexMap& track_point_map)
{
  typedef Keyset::Key Key;

  std::map<size_t, size_t> image_intrinsic_map;
  for (const auto& extrinsic_params : extrinsic_params_map_)
  {
    image_intrinsic_map[extrinsic_params.first.first] =
      extrinsic_params.first.second;
  }

  reprojection_error_ = 0.0;
  size_t number_of_projection = 0;
  for (size_t track_id = 0; track_id < tracks.size(); track_id++)
  {
    if (track_point_map.IsValid(track_id))
    {
      size_t point_id = track_point_map[track_id];
      for (size_t view_id = 0; view_id < tracks[track_id].size(); view_id++)
      {
        size_t image_id = tracks[track_id][view_id].first;
        size_t key_id = tracks[track_id][view_id].second;

        auto itr_intrinsic_id = image_intrinsic_map.find(image_id);
        if (itr_intrinsic_id == image_intrinsic_map.end()) continue;
        size_t intrinsic_id = itr_intrinsic_id->second;

        auto itr_intrinsic = intrinsic_params_map_.find(intrinsic_id);
        if (itr_intrinsic == intrinsic_params_map_.end()) continue;

        auto itr_extrinsic =
          extrinsic_params_map_.find(std::make_pair(image_id, intrinsic_id));
        if (itr_extrinsic == extrinsic_params_map_.end()) continue;

        auto itr_keyset = keysets.find(image_id);
        if (itr_keyset == keysets.end()) continue;

        Key projected =
          hs::sfm::ProjectiveFunctions<Scalar>::WorldPointProjectToImageKey(
            itr_intrinsic->second,
            itr_extrinsic->second,
            pcd.VertexData()[point_id]);

        reprojection_error_ +=
          (projected - itr_keyset->second[key_id]).norm();
        number_of_projection++;

      }
    }
  }

  reprojection_error_ /= Scalar(number_of_projection);
  reprojection_error_computed_ = true;
}

void PhotoOrientationInfoWidget::OnTimeOut()
{
  if (reprojection_error_computed_)
  {
    lineedit_reprojection_error_->setText(QString::number(reprojection_error_));
    if (timer_reprojection_error_->isActive())
    {
      timer_reprojection_error_->stop();
    }
  }
}

IntrinsicParaminfoWidget::IntrinsicParaminfoWidget(const IntrinsicParams& ip)
{
  layout_main_ = new QVBoxLayout;
  setLayout(layout_main_);

  label_focal_length_ = new QLabel(tr("Focal length:"));
  lineedit_focal_length_ = new QLineEdit;
  lineedit_focal_length_->setText(QString::number(ip.focal_length()));
  lineedit_focal_length_->setReadOnly(true);
  layout_focal_length_ = new QHBoxLayout;
  layout_focal_length_->addWidget(label_focal_length_);
  layout_focal_length_->addWidget(lineedit_focal_length_);
  layout_main_->addLayout(layout_focal_length_);

  label_principal_point_x_ = new QLabel(tr("Principal point x:"));
  lineedit_principal_point_x_ = new QLineEdit;
  lineedit_principal_point_x_->setText(
    QString::number(ip.principal_point_x()));
  lineedit_principal_point_x_->setReadOnly(true);
  layout_principal_point_x_ = new QHBoxLayout;
  layout_principal_point_x_->addWidget(label_principal_point_x_);
  layout_principal_point_x_->addWidget(lineedit_principal_point_x_);
  layout_main_->addLayout(layout_principal_point_x_);

  label_principal_point_y_ = new QLabel(tr("Principal point y:"));
  lineedit_principal_point_y_ = new QLineEdit;
  lineedit_principal_point_y_->setText(
    QString::number(ip.principal_point_y()));
  lineedit_principal_point_y_->setReadOnly(true);
  layout_principal_point_y_ = new QHBoxLayout;
  layout_principal_point_y_->addWidget(label_principal_point_y_);
  layout_principal_point_y_->addWidget(lineedit_principal_point_y_);
  layout_main_->addLayout(layout_principal_point_y_);

  label_skew_ = new QLabel(tr("Skew coefficient:"));
  lineedit_skew_ = new QLineEdit;
  lineedit_skew_->setText(
    QString::number(ip.skew()));
  lineedit_skew_->setReadOnly(true);
  layout_skew_ = new QHBoxLayout;
  layout_skew_->addWidget(label_skew_);
  layout_skew_->addWidget(lineedit_skew_);
  layout_main_->addLayout(layout_skew_);

  label_k1_ = new QLabel(tr("K1:"));
  lineedit_k1_ = new QLineEdit;
  lineedit_k1_->setText(
    QString::number(ip.k1()));
  lineedit_k1_->setReadOnly(true);
  layout_k1_ = new QHBoxLayout;
  layout_k1_->addWidget(label_k1_);
  layout_k1_->addWidget(lineedit_k1_);
  layout_main_->addLayout(layout_k1_);

  label_k2_ = new QLabel(tr("K2:"));
  lineedit_k2_ = new QLineEdit;
  lineedit_k2_->setText(
    QString::number(ip.k2()));
  lineedit_k2_->setReadOnly(true);
  layout_k2_ = new QHBoxLayout;
  layout_k2_->addWidget(label_k2_);
  layout_k2_->addWidget(lineedit_k2_);
  layout_main_->addLayout(layout_k2_);

  label_k3_ = new QLabel(tr("k3:"));
  lineedit_k3_ = new QLineEdit;
  lineedit_k3_->setText(
    QString::number(ip.k3()));
  lineedit_k3_->setReadOnly(true);
  layout_k3_ = new QHBoxLayout;
  layout_k3_->addWidget(label_k3_);
  layout_k3_->addWidget(lineedit_k3_);
  layout_main_->addLayout(layout_k3_);

  label_d1_ = new QLabel(tr("d1:"));
  lineedit_d1_ = new QLineEdit;
  lineedit_d1_->setText(
    QString::number(ip.d1()));
  lineedit_d1_->setReadOnly(true);
  layout_d1_ = new QHBoxLayout;
  layout_d1_->addWidget(label_d1_);
  layout_d1_->addWidget(lineedit_d1_);
  layout_main_->addLayout(layout_d1_);

  label_d2_ = new QLabel(tr("d2:"));
  lineedit_d2_ = new QLineEdit;
  lineedit_d2_->setText(
    QString::number(ip.d2()));
  lineedit_d2_->setReadOnly(true);
  layout_d2_ = new QHBoxLayout;
  layout_d2_->addWidget(label_d2_);
  layout_d2_->addWidget(lineedit_d2_);
  layout_main_->addLayout(layout_d2_);

}

IntrinsicParaminfoWidget::~IntrinsicParaminfoWidget()
{
}


// QString PhotoOrientationInfoWidget::position_unit_texts_[
//   NUMBER_OF_POSITION_UNIT_FLAGS] =
// {
//   QString(tr("")),
//   QString(tr("(degree)")),
//   QString(tr("(meter)"))
// };
// 
// QString PhotoOrientationInfoWidget::rotation_unit_texts_[
//   NUMBER_OF_ROTATION_UNIT_FLAGS] =
// {
//   QString(tr("(degree)")),
//   QString(tr("(radian)"))
// };
// 
// QString PhotoOrientationInfoWidget::column_names_[number_of_columns_] =
// {
//   QString(tr("Photo Name")),
//   QString(tr("X")),
//   QString(tr("Y")),
//   QString(tr("Z")),
//   QString(tr("Pitch")),
//   QString(tr("Roll")),
//   QString(tr("Heading")),
//   QString(tr("Error(Pixel)"))
// };
// 
// PhotoOrientationInfoWidget::PhotoOrientationInfoWidget(QWidget* parent)
//   : QTableWidget(parent),
//     photo_icon_(":/images/icon_photo.png")
// {
//   setSortingEnabled(true);
//   setColumnCount(number_of_columns_);
//   QStringList header_labels;
//   for (int i = 0; i < number_of_columns_; i++)
//   {
//     header_labels<<column_names_[i];
//   }
//   setHorizontalHeaderLabels(header_labels);
// }
// 
// void PhotoOrientationInfoWidget::SetPlanarUnit(PositionUnitFlag unit)
// {
//   QTableWidgetItem* x_item = horizontalHeaderItem(1);
//   QTableWidgetItem* y_item = horizontalHeaderItem(2);
//   QString x_text = column_names_[1] + " " + position_unit_texts_[unit];
//   QString y_text = column_names_[2] + " " + position_unit_texts_[unit];
//   x_item->setText(x_text);
//   y_item->setText(y_text);
// }
// 
// void PhotoOrientationInfoWidget::SetHeightUnit(PositionUnitFlag unit)
// {
//   QTableWidgetItem* z_item = horizontalHeaderItem(3);
//   QString z_text = column_names_[3] + " " + position_unit_texts_[unit];
//   z_item->setText(z_text);
// }
// 
// void PhotoOrientationInfoWidget::SetRotationUnit(RotationUnitFlag unit)
// {
//   QTableWidgetItem* pitch_item = horizontalHeaderItem(4);
//   QTableWidgetItem* roll_item = horizontalHeaderItem(5);
//   QTableWidgetItem* heading_item = horizontalHeaderItem(6);
//   QString pitch_text = column_names_[4] + " " + rotation_unit_texts_[unit];
//   QString roll_text = column_names_[5] + " " + rotation_unit_texts_[unit];
//   QString heading_text = column_names_[6] + " " + rotation_unit_texts_[unit];
//   pitch_item->setText(pitch_text);
//   roll_item->setText(roll_text);
//   heading_item->setText(heading_text);
// }
// 
// void PhotoOrientationInfoWidget::SetEntries(
//   const PhotoOrientationEntryContainer& entries)
// {
//   Clear();
//   setRowCount(int(entries.size()));
//   auto itr_entry = entries.begin();
//   auto itr_entry_end = entries.end();
//   for (int row = 0; itr_entry != itr_entry_end; ++itr_entry, ++row)
//   {
//     QTableWidgetItem* name_item =
//       new QTableWidgetItem(photo_icon_, itr_entry->second.photo_name);
//     name_item->setFlags(name_item->flags() ^ Qt::ItemIsEditable);
//     name_item->setData(Qt::UserRole, QVariant(itr_entry->first));
//     setItem(row, 0, name_item);
// 
//     QTableWidgetItem* x_item =
//       new QTableWidgetItem(QString::number(itr_entry->second.pos_x));
//     x_item->setFlags(x_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 1, x_item);
// 
//     QTableWidgetItem* y_item =
//       new QTableWidgetItem(QString::number(itr_entry->second.pos_y));
//     y_item->setFlags(y_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 2, y_item);
// 
//     QTableWidgetItem* z_item =
//       new QTableWidgetItem(QString::number(itr_entry->second.pos_z));
//     z_item->setFlags(z_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 3, z_item);
// 
//     QTableWidgetItem* pitch_item =
//       new QTableWidgetItem(QString::number(itr_entry->second.pitch));
//     pitch_item->setFlags(pitch_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 4, pitch_item);
// 
//     QTableWidgetItem* roll_item =
//       new QTableWidgetItem(QString::number(itr_entry->second.roll));
//     roll_item->setFlags(roll_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 5, roll_item);
// 
//     QTableWidgetItem* heading_item =
//       new QTableWidgetItem(QString::number(itr_entry->second.heading));
//     heading_item->setFlags(heading_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 6, heading_item);
// 
//     QTableWidgetItem* error_item =
//       new QTableWidgetItem(QString::number(
//                              itr_entry->second.reprojection_error));
//     error_item->setFlags(error_item->flags() ^ Qt::ItemIsEditable);
//     setItem(row, 7, error_item);
//   }
// }
// 
// void PhotoOrientationInfoWidget::Clear()
// {
//   int row_count = rowCount();
//   for (int i = 0; i < row_count; i++)
//   {
//     removeRow(0);
//   }
// }

}//namespace gui
}
}
