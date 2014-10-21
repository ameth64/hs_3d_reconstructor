#ifndef _HS_3D_RECONSTRUCTOR_BLOCKS_TREE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_BLOCKS_TREE_WIDGET_HPP_

#include <map>

#include <QTreeWidget>
#include <QIcon>

namespace hs
{
namespace recon
{

class BlocksTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  typedef std::map<uint, QTreeWidgetItem*> ItemMap;
  enum ItemType
  {
    BLOCK = QTreeWidgetItem::UserType,
    PHOTOS,
    FEATURE_MATCH,
    PHOTO_ORIENTATION,
    POINT_CLOUD,
    SURFACE_MODEL,
    DEM,
    TEXTURE,
    DOM
  };
  BlocksTreeWidget(QWidget* parent = NULL);

public:
  int AddBlock(uint block_id, const QString& block_name);
  int AddFeatureMatch(uint block_id, uint feature_match_id,
                      const QString& feature_match_name);
  int AddPhotoOrientation(uint feature_match_id, uint photo_orientation_id,
                          const QString& photo_orientation_name);
  int AddPointCloud(uint photo_orientation_id, uint point_cloud_id,
                    const QString& point_cloud_name);
  int AddSurfaceModel(uint point_cloud_id, uint surface_model_id,
                      const QString& surface_model_name);
  int AddDEM(uint surface_model_id, uint dem_id, const QString& dem_name);
  int AddTexture(uint surface_model_id, uint texture_id,
                 const QString& texture_name);
  int AddDOM(uint texture_model_id, uint dom_id, const QString& dom_name);

  int DeleteBlock(uint block_id);
  int DeleteFeatureMatch(uint feature_match_id);
  int DeletePhotoOrientation(uint photo_orientation_id);
  int DeletePointCloud(uint point_cloud_id);
  int DeleteSurfaceModel(uint surface_model_id);
  int DeleteDEM(uint dem_id);
  int DeleteTexture(uint texture_id);
  int DeleteDOM(uint dom_id);

  int ChangeBlockName(uint block_id, const QString& block_name);
  int ChangeFeatureMatchName(uint feature_match_id,
                             const QString& feature_match_name);
  int ChangePhotoOrientationName(uint photo_orientation_id,
                                 const QString& photo_orientation_name);
  int ChangePointCloudName(uint point_cloud_id,
                           const QString& point_cloud_name);
  int ChangeSurfaceModelName(uint surface_model_id,
                             const QString& surface_model_name);
  int ChangeDEMName(uint dem_id, QString& dem_name);
  int ChangeTextureName(uint texture_id, QString& texture_name);
  int ChangeDOMName(uint dom_id, QString& dom_name);

signals:
  void BlockSelected(uint block_id);
  void PhotosSelected(uint block_id);
  void FeatureMatchSelected(uint feature_match_id);
  void PhotoOrientationSelected(uint photo_orientation_id);
  void PointCloudSelected(uint point_cloud_id);
  void SurfaceModelSelected(uint surface_model_id);
  void DEMSelected(uint dem_id);
  void TextureSelected(uint texture_id);
  void DOMSelected(uint dom_id);

private:
  QIcon block_icon_;
  QIcon photo_icon_;
  QIcon feature_match_icon_;
  QIcon photo_orientation_icon_;
  QIcon point_cloud_icon_;
  QIcon surface_model_icon_;
  QIcon dem_icon_;
  QIcon texture_icon_;
  QIcon dom_icon_;

  ItemMap block_item_map_;
  ItemMap feature_match_item_map_;
  ItemMap photo_orientation_item_map_;
  ItemMap point_cloud_item_map_;
  ItemMap surface_model_item_map_;
  ItemMap dem_item_map_;
  ItemMap texture_item_map_;
  ItemMap dom_item_map_;
};

}
}

#endif
