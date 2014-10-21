#include <limits>

#include <QHeaderView>
#include <QFont>

#include <QMessageBox>

#include "blocks_tree_widget.hpp"

namespace hs
{
namespace recon
{

BlocksTreeWidget::BlocksTreeWidget(QWidget* parent)
  : QTreeWidget(parent),
    block_icon_(QIcon(":/images/icon_block.png")),
    photo_icon_(QIcon(":/images/icon_photo.png")),
    feature_match_icon_(QIcon(":/images/icon_feature_match.png")),
    photo_orientation_icon_(QIcon(":/images/icon_photo_orientation.png")),
    point_cloud_icon_(QIcon(":/images/icon_point_cloud.png")),
    surface_model_icon_(QIcon(":/images/icon_surface_model.png")),
    dem_icon_(QIcon(":/images/icon_dem.png")),
    texture_icon_(QIcon(":/images/icon_texture.png")),
    dom_icon_(QIcon(":/images/icon_dom.png"))
{
  setSelectionMode(QAbstractItemView::SingleSelection);
  setExpandsOnDoubleClick(false);
  header()->close();
}

int BlocksTreeWidget::AddBlock(uint block_id, const QString& block_name)
{
  if (block_item_map_.find(block_id) != block_item_map_.end()) return -1;
  QTreeWidgetItem* block_item =
    new QTreeWidgetItem(this, QStringList()<<block_name, BLOCK);
  block_item->setData(0, Qt::UserRole, QVariant(block_id));
  block_item->setIcon(0, block_icon_);
  addTopLevelItem(block_item);
  QTreeWidgetItem* photo_item =
    new QTreeWidgetItem(block_item, QStringList()<<tr("Photos"), PHOTOS);
  photo_item->setData(0, Qt::UserRole, QVariant(block_id));
  photo_item->setIcon(0, photo_icon_);
  block_item->addChild(photo_item);
  block_item->setExpanded(true);

  block_item_map_[block_id] = block_item;

  return 0;
}
int BlocksTreeWidget::AddFeatureMatch(uint block_id, uint feature_match_id,
                                      const QString& feature_match_name)
{
  if (block_item_map_.find(block_id) == block_item_map_.end() ||
      feature_match_item_map_.find(feature_match_id) !=
      feature_match_item_map_.end()) return -1;

  QTreeWidgetItem* block_item = block_item_map_[block_id];

  QTreeWidgetItem* feature_match_item =
    new QTreeWidgetItem(block_item, QStringList()<<feature_match_name,
                        FEATURE_MATCH);
  feature_match_item->setData(0, Qt::UserRole, QVariant(feature_match_id));
  feature_match_item->setIcon(0, feature_match_icon_);
  feature_match_item->setExpanded(true);
  block_item->addChild(feature_match_item);

  feature_match_item_map_[feature_match_id] = feature_match_item;

  return 0;
}
int BlocksTreeWidget::AddPhotoOrientation(
  uint feature_match_id, uint photo_orientation_id,
  const QString& photo_orientation_name)
{
  if (feature_match_item_map_.find(feature_match_id) ==
      feature_match_item_map_.end() ||
      photo_orientation_item_map_.find(photo_orientation_id) !=
      photo_orientation_item_map_.end()) return -1;

  QTreeWidgetItem* feature_match_item =
    feature_match_item_map_[feature_match_id];

  QTreeWidgetItem* photo_orientation_item =
    new QTreeWidgetItem(feature_match_item,
                        QStringList()<<photo_orientation_name,
                        PHOTO_ORIENTATION);
  photo_orientation_item->setData(0, Qt::UserRole,
                                  QVariant(photo_orientation_id));
  photo_orientation_item->setIcon(0, photo_orientation_icon_);
  photo_orientation_item->setExpanded(true);
  feature_match_item->addChild(photo_orientation_item);

  photo_orientation_item_map_[photo_orientation_id] = photo_orientation_item;

  return 0;
}
int BlocksTreeWidget::AddPointCloud(uint photo_orientation_id,
                                    uint point_cloud_id,
                                    const QString& point_cloud_name)
{
  if (photo_orientation_item_map_.find(photo_orientation_id) ==
      photo_orientation_item_map_.end() ||
      point_cloud_item_map_.find(point_cloud_id) != point_cloud_item_map_.end())
  {
    return -1;
  }

  QTreeWidgetItem* photo_orientation_item =
    photo_orientation_item_map_[photo_orientation_id];

  QTreeWidgetItem* point_cloud_item =
    new QTreeWidgetItem(photo_orientation_item,
                        QStringList()<<point_cloud_name, POINT_CLOUD);
  point_cloud_item->setData(0, Qt::UserRole, QVariant(point_cloud_id));
  point_cloud_item->setIcon(0, point_cloud_icon_);
  point_cloud_item->setExpanded(true);
  photo_orientation_item->addChild(point_cloud_item);

  point_cloud_item_map_[point_cloud_id] = point_cloud_item;
  return 0;
}
int BlocksTreeWidget::AddSurfaceModel(uint point_cloud_id,
                                      uint surface_model_id,
                                      const QString& surface_model_name)
{
  if (point_cloud_item_map_.find(point_cloud_id) ==
      point_cloud_item_map_.end() ||
      surface_model_item_map_.find(surface_model_id) !=
      surface_model_item_map_.end())
  {
    return -1;
  }

  QTreeWidgetItem* point_cloud_item =
    point_cloud_item_map_[point_cloud_id];

  QTreeWidgetItem* surface_model_item =
    new QTreeWidgetItem(point_cloud_item,
                        QStringList()<<surface_model_name, SURFACE_MODEL);
  surface_model_item->setData(0, Qt::UserRole, QVariant(surface_model_id));
  surface_model_item->setIcon(0, surface_model_icon_);
  surface_model_item->setExpanded(true);
  point_cloud_item->addChild(surface_model_item);

  surface_model_item_map_[surface_model_id] = surface_model_item;
  return 0;
}
int BlocksTreeWidget::AddDEM(uint surface_model_id, uint dem_id,
                             const QString& dem_name)
{
  if (surface_model_item_map_.find(surface_model_id) ==
      surface_model_item_map_.end() ||
      dem_item_map_.find(dem_id) !=
      dem_item_map_.end())
  {
    return -1;
  }

  QTreeWidgetItem* surface_model_item =
    surface_model_item_map_[surface_model_id];

  QTreeWidgetItem* dem_item = new QTreeWidgetItem(DEM);
  dem_item->setText(0, dem_name);
  dem_item->setData(0, Qt::UserRole, QVariant(dem_id));
  dem_item->setIcon(0, dem_icon_);
  dem_item->setExpanded(true);
  int child_count = surface_model_item->childCount();
  int i = 0;
  for (; i < child_count; i++)
  {
    QTreeWidgetItem* child = surface_model_item->child(i);
    if (child->type() == TEXTURE)
    {
      break;
    }
  }
  if (i >= child_count)
  {
    surface_model_item->addChild(dem_item);
  }
  else
  {
    surface_model_item->insertChild(i, dem_item);
  }

  dem_item_map_[dem_id] = dem_item;
  return 0;
}
int BlocksTreeWidget::AddTexture(uint surface_model_id, uint texture_id,
                                 const QString& texture_name)
{
  if (surface_model_item_map_.find(surface_model_id) ==
      surface_model_item_map_.end() ||
      texture_item_map_.find(texture_id) !=
      texture_item_map_.end())
  {
    return -1;
  }

  QTreeWidgetItem* surface_model_item =
    surface_model_item_map_[surface_model_id];

  QTreeWidgetItem* texture_item =
    new QTreeWidgetItem(surface_model_item,
                        QStringList()<<texture_name, TEXTURE);
  texture_item->setData(0, Qt::UserRole, QVariant(texture_id));
  texture_item->setIcon(0, texture_icon_);
  texture_item->setExpanded(true);
  surface_model_item->addChild(texture_item);

  texture_item_map_[texture_id] = texture_item;
  return 0;
}
int BlocksTreeWidget::AddDOM(uint texture_id, uint dom_id,
                             const QString& dom_name)
{
  if (texture_item_map_.find(texture_id) ==
      texture_item_map_.end() ||
      dom_item_map_.find(dom_id) !=
      dom_item_map_.end())
  {
    return -1;
  }

  QTreeWidgetItem* texture_item =
    texture_item_map_[texture_id];

  QTreeWidgetItem* dom_item =
    new QTreeWidgetItem(texture_item,
                        QStringList()<<dom_name, DOM);
  dom_item->setData(0, Qt::UserRole, QVariant(dom_id));
  dom_item->setIcon(0, dom_icon_);
  dom_item->setExpanded(true);
  texture_item->addChild(dom_item);

  dom_item_map_[dom_id] = dom_item;
  return 0;
}

int BlocksTreeWidget::DeleteBlock(uint block_id)
{
  return 0;
}
int BlocksTreeWidget::DeleteFeatureMatch(uint feature_match)
{
  return 0;
}
int BlocksTreeWidget::DeletePhotoOrientation(uint photo_orientation_id)
{
  return 0;
}
int BlocksTreeWidget::DeletePointCloud(uint point_cloud_id)
{
  return 0;
}
int BlocksTreeWidget::DeleteSurfaceModel(uint surface_model_id)
{
  return 0;
}
int BlocksTreeWidget::DeleteDEM(uint dem_id)
{
  return 0;
}
int BlocksTreeWidget::DeleteTexture(uint texture_id)
{
  return 0;
}
int BlocksTreeWidget::DeleteDOM(uint dom_id)
{
  return 0;
}

int BlocksTreeWidget::ChangeBlockName(uint block_id, const QString& block_name)
{
  return 0;
}
int BlocksTreeWidget::ChangeFeatureMatchName(uint feature_match_id,
                                             const QString& feature_match_name)
{
  return 0;
}
int BlocksTreeWidget::ChangePhotoOrientationName(
  uint photo_orientation_id, const QString& photo_orientation_name)
{
  return 0;
}
int BlocksTreeWidget::ChangePointCloudName(
  uint point_cloud_id, const QString& point_cloud_name)
{
  return 0;
}
int BlocksTreeWidget::ChangeSurfaceModelName(
  uint surface_model_id, const QString& surface_model_name)
{
  return 0;
}
int BlocksTreeWidget::ChangeDEMName(uint dem_id, QString& dem_name)
{
  return 0;
}
int BlocksTreeWidget::ChangeTextureName(uint texture_id, QString& texture_name)
{
  return 0;
}
int BlocksTreeWidget::ChangeDOMName(uint dom_id, QString& dom_name)
{
  return 0;
}

}
}
