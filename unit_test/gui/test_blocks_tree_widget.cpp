#include <cstring>
#include <limits>

#include <QtTest/QtTest>
#include <QApplication>

#include "gui/blocks_tree_widget.hpp"

#include "test_utility.hpp"

namespace test
{

class TestBlocksTreeWidgetSignal : public QObject
{
  Q_OBJECT
public:
  enum Status
  {
    BLOCK_ITEM_SELECTED = 0,
    PHOTOS_ITEM_SELECTED,
    FEATURE_MATCH_ITEM_SELECTED,
    PHOTO_ORIENTATION_ITEM_SELECTED,
    POINT_CLOUD_ITEM_SELECTED,
    SURFACE_MODEL_ITEM_SELECTED,
    DEM_ITEM_SELECTED,
    TEXTURE_ITEM_SELECTED,
    DOM_ITEM_SELECTED,
    NUMBER_OF_STATUS
  };
public:
  uint id(Status status) const;
  const std::vector<uint>& photo_ids() const;

public slots:
  void OnBlockItemSelected(uint block_id);
  void OnPhotosInOneBlockSelected(uint block_id,
                                  const std::vector<uint>& photo_ids);
  void OnPhotosItemSelected(uint block_id);
  void OnFeatureMatchItemSelected(uint feature_match_id);
  void OnPhotoOrientationItemSelected(uint photo_orientation_id);
  void OnPointCloudItemSelected(uint point_cloud_id);
  void OnSurfaceModelItemSelected(uint surface_model_id);
  void OnDEMItemSelected(uint dem_id);
  void OnTextureItemSelected(uint texture_id);
  void OnDOMItemSelected(uint dom_id);

private:
  void Reset();

private:
  uint ids_[NUMBER_OF_STATUS];
  std::vector<uint> photo_ids_;
};

uint TestBlocksTreeWidgetSignal::id(Status status) const
{
  return ids_[status];
}

void TestBlocksTreeWidgetSignal::OnBlockItemSelected(uint block_id)
{
  Reset();
  ids_[BLOCK_ITEM_SELECTED] = block_id;
}
void TestBlocksTreeWidgetSignal::OnPhotosItemSelected(uint block_id)
{
  Reset();
  ids_[PHOTOS_ITEM_SELECTED] = block_id;
}
void TestBlocksTreeWidgetSignal::OnFeatureMatchItemSelected(
  uint feature_match_id)
{
  Reset();
  ids_[FEATURE_MATCH_ITEM_SELECTED] = feature_match_id;
}
void TestBlocksTreeWidgetSignal::OnPhotoOrientationItemSelected(
  uint photo_orientation_id)
{
  Reset();
  ids_[PHOTO_ORIENTATION_ITEM_SELECTED] = photo_orientation_id;
}
void TestBlocksTreeWidgetSignal::OnPointCloudItemSelected(uint point_cloud_id)
{
  Reset();
  ids_[POINT_CLOUD_ITEM_SELECTED] = point_cloud_id;
}
void TestBlocksTreeWidgetSignal::OnSurfaceModelItemSelected(
  uint surface_model_id)
{
  Reset();
  ids_[SURFACE_MODEL_ITEM_SELECTED] = surface_model_id;
}
void TestBlocksTreeWidgetSignal::OnDEMItemSelected(uint dem_id)
{
  Reset();
  ids_[DEM_ITEM_SELECTED] = dem_id;
}
void TestBlocksTreeWidgetSignal::OnTextureItemSelected(uint texture_id)
{
  Reset();
  ids_[TEXTURE_ITEM_SELECTED] = texture_id;
}
void TestBlocksTreeWidgetSignal::OnDOMItemSelected(uint dom_id)
{
  Reset();
  ids_[DOM_ITEM_SELECTED] = dom_id;
}

void TestBlocksTreeWidgetSignal::OnPhotosInOneBlockSelected(
  uint block_id, const std::vector<uint>& photo_ids)
{
  photo_ids_ = photo_ids;
}

void TestBlocksTreeWidgetSignal::Reset()
{
  std::memset(ids_, std::numeric_limits<uint>::max(), sizeof(ids_));
}

class TestBlocksTreeWidget : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::BlocksTreeWidget BlocksTreeWidget;
  struct DOMNode
  {
    uint id;
    QString name;
  };
  struct TextureNode
  {
    uint id;
    QString name;
    std::map<uint, DOMNode> doms;
  };
  struct DEMNode
  {
    uint id;
    QString name;
  };
  struct SurfaceModelNode
  {
    uint id;
    QString name;
    std::map<uint, TextureNode> textures;
    std::map<uint, DEMNode> dems;
  };
  struct PointCloudNode
  {
    uint id;
    QString name;
    std::map<uint, SurfaceModelNode> surface_models;
  };
  struct PhotoOrientationNode
  {
    uint id;
    QString name;
    std::map<uint, PointCloudNode> point_clouds;
  };
  struct FeatureMatchNode
  {
    uint id;
    QString name;
    std::map<uint, PhotoOrientationNode> photo_orientations;
  };
  struct PhotoNode
  {
    uint id;
    QString name;
  };
  struct BlockNode
  {
    uint id;
    QString name;
    std::map<uint, PhotoNode> photos;
    std::map<uint, FeatureMatchNode> feature_matches;
  };
  typedef std::map<uint, BlockNode> BlockNodeContainer;

  static bool ValidateBlocksTreeWidget(const BlockNodeContainer& blocks,
                                       BlocksTreeWidget* blocks_widget);
  static void GenerateBlocksTreeWidget(uint number_of_blocks,
                                       uint number_of_photos_per_block,
                                       uint number_of_feature_matches,
                                       uint number_of_photo_orientations,
                                       uint number_of_point_clouds,
                                       uint number_of_surface_models,
                                       uint number_of_dems,
                                       uint number_of_textures,
                                       uint number_of_doms,
                                       BlockNodeContainer& blocks,
                                       BlocksTreeWidget* blocks_widget);
  static bool TestTreeWidgetItemSignal(
    QTreeWidgetItem* item, TestBlocksTreeWidgetSignal* signal_tester);

private slots:
  void TestAdding();
  void TestDeleting();
  //void TestChangingName();
  //void TestSelecting();
};

bool TestBlocksTreeWidget::ValidateBlocksTreeWidget(
  const BlockNodeContainer& blocks, BlocksTreeWidget* blocks_widget)
{
  QIcon block_icon(QIcon(":/images/icon_block.png"));
  QIcon photos_icon(QIcon(":/images/icon_photos.png"));
  QIcon photo_icon(QIcon(":/images/icon_photo.png"));
  QIcon feature_match_icon(QIcon(":/images/icon_feature_match.png"));
  QIcon photo_orientation_icon(QIcon(":/images/icon_photo_orientation.png"));
  QIcon point_cloud_icon(QIcon(":/images/icon_point_cloud.png"));
  QIcon surface_model_icon(QIcon(":/images/icon_surface_model.png"));
  QIcon dem_icon(QIcon(":/images/icon_dem.png"));
  QIcon texture_icon(QIcon(":/images/icon_texture.png"));
  QIcon dom_icon(QIcon(":/images/icon_dom.png"));

  size_t number_of_blocks = blocks.size();
  if (number_of_blocks != size_t(blocks_widget->topLevelItemCount()))
    return false;
  auto itr_block = blocks.begin();
  auto itr_block_end = blocks.end();
  for (; itr_block != itr_block_end; ++itr_block)
  {
    QTreeWidgetItem* block_item = blocks_widget->BlockItem(itr_block->first);
    if (itr_block->first != block_item->data(0, Qt::UserRole).toUInt())
      return false;
    if (!IconCompare(block_item->icon(0), block_icon)) return false;
    if (itr_block->second.name != block_item->text(0)) return false;
    if (itr_block->second.feature_matches.size() !=
        size_t(block_item->childCount() - 1)) return false;

    QTreeWidgetItem* photos_item = blocks_widget->PhotosItem(itr_block->first);
    if (photos_item->text(0) != tr("Photos")) return false;
    if (!IconCompare(photos_item->icon(0), photos_icon)) return false;

    auto itr_photo = itr_block->second.photos.begin();
    auto itr_photo_end = itr_block->second.photos.end();
    for (; itr_photo != itr_photo_end; ++itr_photo)
    {
      QTreeWidgetItem* photo_item = blocks_widget->PhotoItem(itr_photo->first);
      if (itr_photo->first !=
          photo_item->data(0, Qt::UserRole).toUInt()) return false;
      if (itr_photo->second.name != photo_item->text(0)) return false;
      if (!IconCompare(photo_item->icon(0), photo_icon)) return false;
    }

    auto itr_feature_match = itr_block->second.feature_matches.begin();
    auto itr_feature_match_end = itr_block->second.feature_matches.end();
    for (; itr_feature_match != itr_feature_match_end; ++itr_feature_match)
    {
      QTreeWidgetItem* feature_match_item =
        blocks_widget->FeatureMatchItem(itr_feature_match->first);
      if (itr_feature_match->first !=
          feature_match_item->data(0, Qt::UserRole).toUInt()) return false;
      if (itr_feature_match->second.name != feature_match_item->text(0))
        return false;
      if (!IconCompare(feature_match_item->icon(0), feature_match_icon))
        return false;
      if (itr_feature_match->second.photo_orientations.size() !=
          size_t(feature_match_item->childCount())) return false;

      auto itr_photo_orientation =
        itr_feature_match->second.photo_orientations.begin();
      auto itr_photo_orientation_end =
        itr_feature_match->second.photo_orientations.end();
      for (; itr_photo_orientation != itr_photo_orientation_end;
           ++itr_photo_orientation)
      {
        QTreeWidgetItem* photo_orientation_item =
          blocks_widget->PhotoOrientationItem(itr_photo_orientation->first);
        if (itr_photo_orientation->first !=
            photo_orientation_item->data(0, Qt::UserRole).toUInt())
          return false;
        if (itr_photo_orientation->second.name !=
            photo_orientation_item->text(0)) return false;
        if (!IconCompare(photo_orientation_item->icon(0),
                         photo_orientation_icon)) return false;
        if (itr_photo_orientation->second.point_clouds.size() !=
            size_t(photo_orientation_item->childCount())) return false;

        auto itr_point_cloud =
          itr_photo_orientation->second.point_clouds.begin();
        auto itr_point_cloud_end =
          itr_photo_orientation->second.point_clouds.end();
        for (; itr_point_cloud != itr_point_cloud_end; ++itr_point_cloud)
        {
          QTreeWidgetItem* point_cloud_item =
            blocks_widget->PointCloudItem(itr_point_cloud->first);
          if (itr_point_cloud->first !=
              point_cloud_item->data(0, Qt::UserRole).toUInt()) return false;
          if (itr_point_cloud->second.name != point_cloud_item->text(0))
            return false;
          if (!IconCompare(point_cloud_item->icon(0), point_cloud_icon))
            return false;
          if (itr_point_cloud->second.surface_models.size() !=
              size_t(point_cloud_item->childCount())) return false;

          auto itr_surface_model =
            itr_point_cloud->second.surface_models.begin();
          auto itr_surface_model_end =
            itr_point_cloud->second.surface_models.end();
          for (; itr_surface_model != itr_surface_model_end;
               ++itr_surface_model)
          {
            QTreeWidgetItem* surface_model_item =
              blocks_widget->SurfaceModelItem(itr_surface_model->first);
            if (itr_surface_model->first !=
                surface_model_item->data(0, Qt::UserRole).toUInt())
              return false;
            if (itr_surface_model->second.name != surface_model_item->text(0))
              return false;
            if (!IconCompare(surface_model_item->icon(0), surface_model_icon))
              return false;
            if (itr_surface_model->second.dems.size() +
                itr_surface_model->second.textures.size() !=
                size_t(surface_model_item->childCount())) return false;

            auto itr_dem = itr_surface_model->second.dems.begin();
            auto itr_dem_end = itr_surface_model->second.dems.end();
            for (; itr_dem != itr_dem_end; ++itr_dem)
            {
              QTreeWidgetItem* dem_item =
                blocks_widget->DEMItem(itr_dem->first);
              if (itr_dem->first != dem_item->data(0, Qt::UserRole).toUInt())
                return false;
              if (itr_dem->second.name != dem_item->text(0)) return false;
              if (!IconCompare(dem_item->icon(0), dem_icon)) return false;
            }

            auto itr_texture = itr_surface_model->second.textures.begin();
            auto itr_texture_end = itr_surface_model->second.textures.end();
            for (; itr_texture != itr_texture_end; ++itr_texture)
            {
              QTreeWidgetItem* texture_item =
                blocks_widget->TextureItem(itr_texture->first);
              if (itr_texture->first !=
                  texture_item->data(0, Qt::UserRole).toUInt())
                return false;
              if (itr_texture->second.name != texture_item->text(0))
                return false;
              if (!IconCompare(texture_item->icon(0), texture_icon))
                return false;
              if (itr_texture->second.doms.size() !=
                  size_t(texture_item->childCount())) return false;

              auto itr_dom = itr_texture->second.doms.begin();
              auto itr_dom_end = itr_texture->second.doms.end();
              for (; itr_dom != itr_dom_end; ++itr_dom)
              {
                QTreeWidgetItem* dom_item =
                  blocks_widget->DOMItem(itr_dom->first);
                if (itr_dom->first != dom_item->data(0, Qt::UserRole).toUInt())
                  return false;
                if (itr_dom->second.name != dom_item->text(0))
                  return false;
                if (!IconCompare(dom_item->icon(0), dom_icon))
                  return false;
              }
            }
          }

        }
      }
    }
  }

  return true;
}

void TestBlocksTreeWidget::GenerateBlocksTreeWidget(
  uint number_of_blocks,
  uint number_of_photos_per_block,
  uint number_of_feature_matches,
  uint number_of_photo_orientations,
  uint number_of_point_clouds,
  uint number_of_surface_models,
  uint number_of_dems,
  uint number_of_textures,
  uint number_of_doms,
  BlockNodeContainer& blocks,
  BlocksTreeWidget* blocks_widget)
{
  blocks.clear();
  uint block_id = 1;
  uint photo_id = 1;
  uint feature_match_id = 1;
  uint photo_orientation_id = 1;
  uint point_cloud_id = 1;
  uint surface_model_id = 1;
  uint dem_id = 1;
  uint texture_id = 1;
  uint dom_id = 1;
  BlockNode block;
  PhotoNode photo;
  FeatureMatchNode feature_match;
  PhotoOrientationNode photo_orientation;
  PointCloudNode point_cloud;
  SurfaceModelNode surface_model;
  DEMNode dem;
  TextureNode texture;
  DOMNode dom;

  for (uint block_count = 0; block_count < number_of_blocks;
       block_id++, block_count++)
  {
    block.feature_matches.clear();
    block.id = block_id;
    block.name = QString(tr("test_block_%1")).arg(block_id);
    blocks_widget->AddBlock(block.id, block.name);

    block.photos.clear();
    BlocksTreeWidget::StringMap photo_names;
    for (uint photo_count = 0; photo_count < number_of_photos_per_block;
         photo_id++, photo_count++)
    {
      photo.id = photo_id;
      photo.name = QString(tr("IMG%1.jpg")).arg(photo_id);
      photo_names[photo_id] = photo.name;
      block.photos[photo_id] = photo;
    }
    blocks_widget->AddPhotosToBlock(block_id, photo_names);

    for (uint feature_match_count = 0;
         feature_match_count < number_of_feature_matches;
         feature_match_id++, feature_match_count++)
    {
      feature_match.photo_orientations.clear();
      feature_match.id = feature_match_id;
      feature_match.name =
        QString(tr("test_feature_match_%1")).arg(feature_match_id);
      blocks_widget->AddFeatureMatch(block.id, feature_match.id,
                                     feature_match.name);
      for (uint photo_orientation_count = 0;
           photo_orientation_count < number_of_photo_orientations;
           photo_orientation_id++, photo_orientation_count++)
      {
        photo_orientation.point_clouds.clear();
        photo_orientation.id = photo_orientation_id;
        photo_orientation.name =
          QString(tr("test_photo_orientation_%1")).arg(photo_orientation_id);
        blocks_widget->AddPhotoOrientation(feature_match.id,
                                           photo_orientation.id,
                                           photo_orientation.name);
        for (uint point_cloud_count = 0;
             point_cloud_count < number_of_point_clouds;
             point_cloud_id++, point_cloud_count++)
        {
          point_cloud.surface_models.clear();
          point_cloud.id = point_cloud_id;
          point_cloud.name =
            QString(tr("test_point_cloud_%1")).arg(point_cloud_id);
          blocks_widget->AddPointCloud(photo_orientation.id,
                                       point_cloud.id,
                                       point_cloud.name);
          for (int surface_model_count = 0;
               surface_model_count < number_of_surface_models;
               surface_model_id++, surface_model_count++)
          {
            surface_model.dems.clear();
            surface_model.textures.clear();
            surface_model.id = surface_model_id;
            surface_model.name =
              QString(tr("test_surface_model_%1")).arg(surface_model_id);
            blocks_widget->AddSurfaceModel(point_cloud.id, surface_model.id,
                                           surface_model.name);
            //交叉加入dem和texture，以测试dem节点要在texture之前
            uint dem_count = 0;
            uint texture_count = 0;
            for (; dem_count < number_of_dems ||
                   texture_count < number_of_textures;
                 dem_id++, dem_count++, texture_id++, texture_count++)
            {
              if (dem_count < number_of_dems)
              {
                dem.id = dem_id;
                dem.name = QString(tr("test_dem_%1")).arg(dem_id);
                blocks_widget->AddDEM(surface_model.id, dem_id, dem.name);
                surface_model.dems[dem.id] = dem;
              }

              if (texture_count < number_of_textures)
              {
                texture.doms.clear();
                texture.id = texture_id;
                texture.name = QString(tr("test_texture_%1")).arg(texture_id);
                blocks_widget->AddTexture(surface_model.id, texture.id,
                                          texture.name);
                for (uint dom_count = 0; dom_count < number_of_doms;
                     dom_id++, dom_count++)
                {
                  dom.id = dom_id;
                  dom.name = QString(tr("test_dom_%1")).arg(dom_id);
                  blocks_widget->AddDOM(texture.id, dom.id, dom.name);
                  texture.doms[dom.id] = dom;
                }
                surface_model.textures[texture.id] = texture;
              }
            }
            point_cloud.surface_models[surface_model.id] = surface_model;
          }
          photo_orientation.point_clouds[point_cloud.id] = point_cloud;
        }
        feature_match.photo_orientations[photo_orientation.id] =
          photo_orientation;
      }
      block.feature_matches[feature_match.id] = feature_match;
    }
    blocks[block.id] = block;
  }
}

//bool TestBlocksTreeWidget::TestTreeWidgetItemSignal(
//  QTreeWidgetItem* item, TestBlocksTreeWidgetSignal* signal_tester)
//{
//  item->setSelected(true);
//  switch (item->type())
//  {
//  case BlocksTreeWidget::BLOCK:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::BLOCK_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::PHOTOS:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::PHOTOS_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::FEATURE_MATCH:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::FEATURE_MATCH_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::PHOTO_ORIENTATION:
//    {
//      if (signal_tester->id(
//            TestBlocksTreeWidgetSignal::PHOTO_ORIENTATION_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::POINT_CLOUD:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::POINT_CLOUD_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::SURFACE_MODEL:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::SURFACE_MODEL_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::DEM:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::DEM_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::TEXTURE:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::TEXTURE_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  case BlocksTreeWidget::DOM:
//    {
//      if (signal_tester->id(TestBlocksTreeWidgetSignal::DOM_SELECTED) !=
//          item->data(0, Qt::UserRole)) return false;
//      break;
//    }
//  default:
//    {
//      return false;
//      break;
//    }
//  }
//
//  int number_of_children = item->childCount();
//  if (number_of_children > 0)
//  {
//    for (int i = 0; i < number_of_children; i++)
//    {
//      if (TestTreeWidgetItemSignal(item->child(i), signal_tester))
//      {
//        return false;
//      }
//    }
//    return true;
//  }
//  else
//  {
//    return true;
//  }
//}

void TestBlocksTreeWidget::TestAdding()
{
  BlockNodeContainer blocks;
  BlocksTreeWidget blocks_widget;
  uint number_of_blocks = 2;
  uint number_of_photos_per_block = 100;
  uint number_of_feature_matches = 2;
  uint number_of_photo_orientations = 2;
  uint number_of_point_clouds = 2;
  uint number_of_surface_models = 2;
  uint number_of_dems = 2;
  uint number_of_textures = 2;
  uint number_of_doms = 2;

  GenerateBlocksTreeWidget(number_of_blocks,
                           number_of_photos_per_block,
                           number_of_feature_matches,
                           number_of_photo_orientations,
                           number_of_point_clouds,
                           number_of_surface_models,
                           number_of_dems,
                           number_of_textures,
                           number_of_doms,
                           blocks,
                           &blocks_widget);

  blocks_widget.show();
  qApp->exec();

  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));
}

void TestBlocksTreeWidget::TestDeleting()
{
  BlockNodeContainer blocks;
  BlocksTreeWidget blocks_widget;
  uint number_of_blocks = 2;
  uint number_of_photos_per_block = 100;
  uint number_of_feature_matches = 2;
  uint number_of_photo_orientations = 2;
  uint number_of_point_clouds = 2;
  uint number_of_surface_models = 2;
  uint number_of_dems = 2;
  uint number_of_textures = 2;
  uint number_of_doms = 2;

  GenerateBlocksTreeWidget(number_of_blocks,
                           number_of_photos_per_block,
                           number_of_feature_matches,
                           number_of_photo_orientations,
                           number_of_point_clouds,
                           number_of_surface_models,
                           number_of_dems,
                           number_of_textures,
                           number_of_doms,
                           blocks,
                           &blocks_widget);

  blocks_widget.DeleteBlock(2);
  blocks.erase(2);
  for (uint i = 101; i <= 200; i++)
  {
    QCOMPARE(blocks_widget.PhotoItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 3; i <= 4; i++)
  {
    QCOMPARE(blocks_widget.FeatureMatchItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 5; i <= 8; i++)
  {
    QCOMPARE(blocks_widget.PhotoOrientationItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 9; i <= 16; i++)
  {
    QCOMPARE(blocks_widget.PointCloudItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 17; i <= 32; i++)
  {
    QCOMPARE(blocks_widget.SurfaceModelItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 33; i <= 64; i++)
  {
    QCOMPARE(blocks_widget.DEMItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 33; i <= 64; i++)
  {
    QCOMPARE(blocks_widget.TextureItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 65; i <= 128; i++)
  {
    QCOMPARE(blocks_widget.DOMItem(i), (QTreeWidgetItem*)(0));
  }
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  std::vector<uint> photo_ids;
  for (uint photo_id = 1; photo_id < 50; photo_id++)
  {
    photo_ids.push_back(photo_id);
    blocks[1].photos.erase(photo_id);
  }
  blocks_widget.DeletePhotosFromBlock(1, photo_ids);
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeleteFeatureMatch(1);
  blocks[1].feature_matches.erase(1);
  for (uint i = 1; i <= 2; i++)
  {
    QCOMPARE(blocks_widget.PhotoOrientationItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 1; i <= 4; i++)
  {
    QCOMPARE(blocks_widget.PointCloudItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 1; i <= 8; i++)
  {
    QCOMPARE(blocks_widget.SurfaceModelItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 1; i <= 16; i++)
  {
    QCOMPARE(blocks_widget.DEMItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 1; i <= 16; i++)
  {
    QCOMPARE(blocks_widget.TextureItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 1; i <= 32; i++)
  {
    QCOMPARE(blocks_widget.DOMItem(i), (QTreeWidgetItem*)(0));
  }
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeletePhotoOrientation(3);
  blocks[1].feature_matches[2].photo_orientations.erase(3);
  for (uint i = 5; i <= 6; i++)
  {
    QCOMPARE(blocks_widget.PointCloudItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 9; i <= 12; i++)
  {
    QCOMPARE(blocks_widget.SurfaceModelItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 17; i <= 24; i++)
  {
    QCOMPARE(blocks_widget.DEMItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 17; i <= 24; i++)
  {
    QCOMPARE(blocks_widget.TextureItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 33; i <= 48; i++)
  {
    QCOMPARE(blocks_widget.DOMItem(i), (QTreeWidgetItem*)(0));
  }
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeletePointCloud(7);
  blocks[1].feature_matches[2].photo_orientations[4].point_clouds.erase(7);
  for (uint i = 13; i <= 14; i++)
  {
    QCOMPARE(blocks_widget.SurfaceModelItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 25; i <= 28; i++)
  {
    QCOMPARE(blocks_widget.DEMItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 25; i <= 28; i++)
  {
    QCOMPARE(blocks_widget.TextureItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 49; i <= 56; i++)
  {
    QCOMPARE(blocks_widget.DOMItem(i), (QTreeWidgetItem*)(0));
  }
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeleteSurfaceModel(15);
  blocks[1].feature_matches[2].photo_orientations[4].point_clouds[8].
    surface_models.erase(15);
  for (uint i = 29; i <= 30; i++)
  {
    QCOMPARE(blocks_widget.DEMItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 29; i <= 30; i++)
  {
    QCOMPARE(blocks_widget.TextureItem(i), (QTreeWidgetItem*)(0));
  }
  for (uint i = 57; i <= 60; i++)
  {
    QCOMPARE(blocks_widget.DOMItem(i), (QTreeWidgetItem*)(0));
  }
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeleteDEM(31);
  blocks[1].feature_matches[2].photo_orientations[4].point_clouds[8].
    surface_models[16].dems.erase(31);
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeleteTexture(31);
  blocks[1].feature_matches[2].photo_orientations[4].point_clouds[8].
    surface_models[16].textures.erase(31);
  for (uint i = 61; i <= 62; i++)
  {
    QCOMPARE(blocks_widget.DOMItem(i), (QTreeWidgetItem*)(0));
  }
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  blocks_widget.DeleteDOM(63);
  blocks[1].feature_matches[2].photo_orientations[4].point_clouds[8].
    surface_models[16].textures[32].doms.erase(63);
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

}

//void TestBlocksTreeWidget::TestChangingName()
//{
//  BlockNodeContainer blocks;
//  BlocksTreeWidget blocks_widget;
//  uint number_of_blocks = 2;
//  uint number_of_feature_matches = 2;
//  uint number_of_photo_orientations = 2;
//  uint number_of_point_clouds = 2;
//  uint number_of_surface_models = 2;
//  uint number_of_dems = 2;
//  uint number_of_textures = 2;
//  uint number_of_doms = 2;
//
//  GenerateBlocksTreeWidget(number_of_blocks,
//                           number_of_feature_matches,
//                           number_of_photo_orientations,
//                           number_of_point_clouds,
//                           number_of_surface_models,
//                           number_of_dems,
//                           number_of_textures,
//                           number_of_doms,
//                           blocks,
//                           &blocks_widget);
//
//  QString change_block_name = tr("test_change_block_0");
//  blocks_widget.ChangeBlockName(0, change_block_name);
//  blocks[0].name = change_block_name;
//
//  QString change_feature_match_name = tr("test_change_feature_match_1");
//  blocks_widget.ChangeFeatureMatchName(1, change_feature_match_name);
//  blocks[0].feature_matches[1].name = change_feature_match_name;
//
//  QString change_photo_orientation_name = tr("test_change_photo_orientation_3");
//  blocks_widget.ChangePhotoOrientationName(3, change_photo_orientation_name);
//  blocks[0].feature_matches[1].photo_orientations[3].name =
//    change_photo_orientation_name;
//
//  QString change_point_cloud_name = tr("test_change_point_cloud_7");
//  blocks_widget.ChangePointCloudName(7, change_point_cloud_name);
//  blocks[0].feature_matches[1].photo_orientations[3].point_clouds[7].name =
//    change_point_cloud_name;
//
//  QString change_surface_model_name = tr("test_change_surface_model_15");
//  blocks_widget.ChangeSurfaceModelName(15, change_surface_model_name);
//  blocks[0].feature_matches[1].photo_orientations[3].point_clouds[7].
//    surface_models[15].name = change_surface_model_name;
//
//  QString change_dem_name = tr("test_change_dem_31");
//  blocks_widget.ChangeDEMName(31, change_dem_name);
//  blocks[0].feature_matches[1].photo_orientations[3].point_clouds[7].
//    surface_models[15].dems[31].name = change_dem_name;
//
//  QString change_texture_name = tr("test_change_texture_31");
//  blocks_widget.ChangeTextureName(31, change_texture_name);
//  blocks[0].feature_matches[1].photo_orientations[3].point_clouds[7].
//    surface_models[15].textures[31].name = change_texture_name;
//
//  QString change_dom_name = tr("test_change_dom_62");
//  blocks_widget.ChangeDOMName(62, change_dom_name);
//  blocks[0].feature_matches[1].photo_orientations[3].point_clouds[7].
//    surface_models[15].textures[31].doms[62].name = change_dom_name;
//
//  blocks_widget.show();
//  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));
//}

//void TestBlocksTreeWidget::TestSelecting()
//{
//  BlockNodeContainer blocks;
//  BlocksTreeWidget blocks_widget;
//  uint number_of_blocks = 2;
//  uint number_of_feature_matches = 2;
//  uint number_of_photo_orientations = 2;
//  uint number_of_point_clouds = 2;
//  uint number_of_surface_models = 2;
//  uint number_of_dems = 2;
//  uint number_of_textures = 2;
//  uint number_of_doms = 2;
//
//  GenerateBlocksTreeWidget(number_of_blocks,
//                           number_of_feature_matches,
//                           number_of_photo_orientations,
//                           number_of_point_clouds,
//                           number_of_surface_models,
//                           number_of_dems,
//                           number_of_textures,
//                           number_of_doms,
//                           blocks,
//                           &blocks_widget);
//
//  TestBlocksTreeWidgetSignal signal_tester;
//
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::BlockSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnBlockSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::PhotosSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnPhotosSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::FeatureMatchSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnFeatureMatchSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::PhotoOrientationSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnPhotoOrientationSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::PointCloudSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnPointCloudSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::SurfaceModelSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnSurfaceModelSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::DEMSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnDEMSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::TextureSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnTextureSelected);
//  QObject::connect(&blocks_widget,
//                   &BlocksTreeWidget::DOMSelected,
//                   &signal_tester,
//                   &TestBlocksTreeWidgetSignal::OnDOMSelected);
//
//  int top_level_item_count = blocks_widget.topLevelItemCount();
//  for (int i = 0; i < top_level_item_count; i++)
//  {
//    QCOMPARE(true, TestTreeWidgetItemSignal(blocks_widget.topLevelItem(i),
//                                            &signal_tester));
//  }
//}


}

QTEST_MAIN(test::TestBlocksTreeWidget)
#include "test_blocks_tree_widget.moc"
