#include <QtTest/QtTest>
#include <QApplication>

#include "blocks_tree_widget.hpp"

#include "test_utility.hpp"

namespace test
{

class TestBlocksTreeWidget : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::BlocksTreeWidget BlocksTreeWidget;
  typedef BlocksTreeWidget::PhotoEntry PhotoEntry;
  typedef BlocksTreeWidget::BlockEntry BlockEntry;
  typedef BlocksTreeWidget::PhotoContainer PhotoContainer;
  typedef std::map<uint, BlockEntry> BlockContainer;

  static void GeneratePhotos(int number_of_not_at_all_photos,
                             int number_of_oriented_photos,
                             int number_of_position_valid_photos,
                             int number_of_rotation_valid_photos,
                             int number_of_valid_photos,
                             uint id_offset,
                             PhotoContainer& photos);

  static bool ValidateBlocksTreeWidget(const BlockContainer& blocks,
                                       BlocksTreeWidget* blocks_widget);
private slots:
  void TestAddBlock();
  void TestAddPhotos();
  void TestDeleteBlocksByIds();
  void TestDeleteBlocksBySelectedItems();
  void TestActivateBlockById();
  void TestActivateBlockBySelectedItem();
  void TestRemovePhotosByIds();
  void TestRemovePhotosBySelectedItems();
};

void TestBlocksTreeWidget::GeneratePhotos(int number_of_not_at_all_photos,
                                         int number_of_oriented_photos,
                                         int number_of_position_valid_photos,
                                         int number_of_rotation_valid_photos,
                                         int number_of_valid_photos,
                                         uint id_offset,
                                         PhotoContainer& photos)
{
  photos.clear();
  for (int i = 0; i < number_of_not_at_all_photos; i++)
  {
    PhotoEntry photo;
    photo.id = id_offset + i;
    photo.file_name = QString("na000%1.jpg").arg(i);
    photo.mask.reset();
    photos[photo.id] = photo;
  }
  for (int i = 0; i < number_of_oriented_photos; i++)
  {
    PhotoEntry photo;
    photo.id = id_offset + number_of_not_at_all_photos + i;
    photo.file_name = QString("ca000%1.jpg").arg(i);
    photo.mask.set();
    photos[photo.id] = photo;
  }
  for (int i = 0; i < number_of_position_valid_photos; i++)
  {
    PhotoEntry photo;
    photo.id = id_offset +
               number_of_not_at_all_photos + number_of_oriented_photos + i;
    photo.file_name = QString("pi000%1.jpg").arg(i);
    photo.mask.reset();
    photo.mask.set(PhotoEntry::POSITION);
    photos[photo.id] = photo;
  }
  for (int i = 0; i < number_of_rotation_valid_photos; i++)
  {
    PhotoEntry photo;
    photo.id = id_offset +
               number_of_not_at_all_photos +
               number_of_oriented_photos +
               number_of_position_valid_photos + i;
    photo.file_name = QString("ri000%1.jpg").arg(i);
    photo.mask.reset();
    photo.mask.set(PhotoEntry::ROTATION);
    photos[photo.id] = photo;
  }
  for (int i = 0; i < number_of_valid_photos; i++)
  {
    PhotoEntry photo;
    photo.id = id_offset +
               number_of_not_at_all_photos +
               number_of_oriented_photos +
               number_of_position_valid_photos +
               number_of_rotation_valid_photos + i;
    photo.file_name = QString("ii000%1.jpg").arg(i);
    photo.mask.reset();
    photo.mask.set(PhotoEntry::POSITION);
    photo.mask.set(PhotoEntry::ROTATION);
    photos[photo.id] = photo;
  }
}

bool TestBlocksTreeWidget::ValidateBlocksTreeWidget(
  const BlockContainer& blocks, BlocksTreeWidget* blocks_widget)
{
  QIcon photo_icon(":/images/icon_photo.png");
  QIcon valid_icon(":/images/icon_valid.png");
  QIcon invalid_icon(":/images/icon_invalid.png");
  QIcon workspace_icon(":/images/icon_workspace.png");
  QIcon block_icon(":/images/icon_block.png");

  QTreeWidgetItem* workspace_item =
    blocks_widget->findItems(tr("Workspace"), Qt::MatchExactly)[0];
  QIcon item_workspace_icon = workspace_item->icon(0);
  if (!IconCompare(workspace_icon, item_workspace_icon)) return false;

  int block_item_count = workspace_item->childCount();
  if (block_item_count != int(blocks.size())) return false;
  for (int i = 0; i < block_item_count; i++)
  {
    QTreeWidgetItem* block_item = workspace_item->child(i);
    uint block_id = block_item->data(0, Qt::UserRole).toUInt();
    auto itr_block = blocks.find(block_id);
    if (itr_block == blocks.end()) return false;
    QIcon item_block_icon = block_item->icon(0);
    if (!IconCompare(block_icon, item_block_icon)) return false;
    if (itr_block->second.name != block_item->text(0)) return false;

    int photo_item_count = block_item->childCount();
    if (photo_item_count != int(itr_block->second.photos.size())) return false;
    for (int j = 0; j < photo_item_count; j++)
    {
      QTreeWidgetItem* photo_item = block_item->child(j);
      QIcon item_photo_icon = photo_item->icon(0);
      if (!IconCompare(photo_icon, item_photo_icon)) return false;

      uint photo_id = photo_item->data(0, Qt::UserRole).toUInt();
      auto itr_photo = itr_block->second.photos.find(photo_id);
      if (itr_photo == itr_block->second.photos.end()) return false;
      if (itr_photo->second.file_name != photo_item->text(0)) return false;

      QIcon position_icon = photo_item->icon(1);
      QIcon rotation_icon = photo_item->icon(2);
      QIcon oriented_icon = photo_item->icon(3);
      if (itr_photo->second.mask[PhotoEntry::POSITION])
      {
        if (!IconCompare(valid_icon, position_icon)) return false;
      }
      else
      {
        if (!IconCompare(invalid_icon, position_icon)) return false;
      }
      if (itr_photo->second.mask[PhotoEntry::ROTATION])
      {
        if (!IconCompare(valid_icon, rotation_icon)) return false;
      }
      else
      {
        if (!IconCompare(invalid_icon, rotation_icon)) return false;
      }
      if (itr_photo->second.mask[PhotoEntry::ORIENTED])
      {
        if (!IconCompare(valid_icon, oriented_icon)) return false;
      }
      else
      {
        if (!IconCompare(invalid_icon, oriented_icon)) return false;
      }
    }
  }

  return true;
}

void TestBlocksTreeWidget::TestAddBlock()
{
  uint block_id = 0;

  BlockEntry block;
  block.id = block_id;
  block.name = tr("test_block");

  int number_of_not_at_all_photos = 3;
  int number_of_oriented_photos = 3;
  int number_of_position_valid_photos = 3;
  int number_of_rotation_valid_photos = 3;
  int number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 0,
                 block.photos);

  BlockContainer blocks;
  blocks[block_id] = block;

  hs::recon::BlocksTreeWidget blocks_widget(NULL);
  blocks_widget.AddBlock(block_id, block);
  blocks_widget.show();

  ValidateBlocksTreeWidget(blocks, &blocks_widget);

  //qApp->exec();
}

void TestBlocksTreeWidget::TestAddPhotos()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos, 0,
                 block.photos);
  blocks[block_id] = block;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(block_id, block);

  PhotoContainer photos_to_add;
  GeneratePhotos(2, 2, 2, 2, 2, 15, photos_to_add);
  blocks_widget.AddPhotos(block_id, photos_to_add);

  blocks[block_id].photos.insert(photos_to_add.begin(), photos_to_add.end());

  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));
}

void TestBlocksTreeWidget::TestDeleteBlocksByIds()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;
  uint photo_id_offset = 0;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 15;

  block_id = 1;
  block.id = block_id;
  block.name = tr("test_block_1");
  number_of_not_at_all_photos = 4;
  number_of_oriented_photos = 4;
  number_of_position_valid_photos = 4;
  number_of_rotation_valid_photos = 4;
  number_of_valid_photos = 4;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 20;

  block_id = 2;
  block.id = block_id;
  block.name = tr("test_block_2");
  number_of_not_at_all_photos = 5;
  number_of_oriented_photos = 5;
  number_of_position_valid_photos = 5;
  number_of_rotation_valid_photos = 5;
  number_of_valid_photos = 5;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 25;

  block_id = 3;
  block.id = block_id;
  block.name = tr("test_block_3");
  number_of_not_at_all_photos = 6;
  number_of_oriented_photos = 6;
  number_of_position_valid_photos = 6;
  number_of_rotation_valid_photos = 6;
  number_of_valid_photos = 6;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 30;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(0, blocks[0]);
  blocks_widget.AddBlock(1, blocks[1]);
  blocks_widget.AddBlock(2, blocks[2]);
  blocks_widget.AddBlock(3, blocks[3]);
  blocks_widget.show();

  std::vector<uint> block_ids;
  block_ids.push_back(1);
  block_ids.push_back(2);
  blocks_widget.DeleteBlocksByIds(block_ids);

  blocks.erase(1);
  blocks.erase(2);

  //QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));
  qApp->exec();
}

void TestBlocksTreeWidget::TestDeleteBlocksBySelectedItems()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;
  uint photo_id_offset = 0;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;

  block_id = 1;
  block.id = block_id;
  block.name = tr("test_block_1");
  number_of_not_at_all_photos = 4;
  number_of_oriented_photos = 4;
  number_of_position_valid_photos = 4;
  number_of_rotation_valid_photos = 4;
  number_of_valid_photos = 4;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 20;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(0, blocks[0]);
  blocks_widget.AddBlock(1, blocks[1]);
  blocks_widget.show();
  QTest::mouseClick(blocks_widget.viewport(), Qt::LeftButton, 0,
                    QPoint(80, 20));
  blocks_widget.DeleteBlocksBySelectedItems();
  blocks.erase(0);
  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  //qApp->exec();
}

void TestBlocksTreeWidget::TestActivateBlockById()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;
  uint photo_id_offset = 0;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 15;

  block_id = 1;
  block.id = block_id;
  block.name = tr("test_block_1");
  number_of_not_at_all_photos = 4;
  number_of_oriented_photos = 4;
  number_of_position_valid_photos = 4;
  number_of_rotation_valid_photos = 4;
  number_of_valid_photos = 4;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 20;

  block_id = 2;
  block.id = block_id;
  block.name = tr("test_block_2");
  number_of_not_at_all_photos = 5;
  number_of_oriented_photos = 5;
  number_of_position_valid_photos = 5;
  number_of_rotation_valid_photos = 5;
  number_of_valid_photos = 5;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 25;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(0, blocks[0]);
  blocks_widget.AddBlock(1, blocks[1]);
  blocks_widget.AddBlock(2, blocks[2]);

  blocks_widget.ActivateBlockById(1);

  QCOMPARE(uint(1), blocks_widget.activated_block_id());
}

void TestBlocksTreeWidget::TestActivateBlockBySelectedItem()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;
  uint photo_id_offset = 0;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 15;

  block_id = 1;
  block.id = block_id;
  block.name = tr("test_block_1");
  number_of_not_at_all_photos = 4;
  number_of_oriented_photos = 4;
  number_of_position_valid_photos = 4;
  number_of_rotation_valid_photos = 4;
  number_of_valid_photos = 4;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 20;

  block_id = 2;
  block.id = block_id;
  block.name = tr("test_block_2");
  number_of_not_at_all_photos = 5;
  number_of_oriented_photos = 5;
  number_of_position_valid_photos = 5;
  number_of_rotation_valid_photos = 5;
  number_of_valid_photos = 5;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 25;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(0, blocks[0]);
  blocks_widget.AddBlock(1, blocks[1]);
  blocks_widget.AddBlock(2, blocks[2]);

  blocks_widget.resize(800, 600);
  blocks_widget.show();

  QTest::mouseDClick(&blocks_widget, Qt::LeftButton, 0,
                     QPoint(50, 310));

  QCOMPARE(uint(1), blocks_widget.activated_block_id());
  //qApp->exec();
}

void TestBlocksTreeWidget::TestRemovePhotosByIds()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;
  uint photo_id_offset = 0;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 15;

  block_id = 1;
  block.id = block_id;
  block.name = tr("test_block_1");
  number_of_not_at_all_photos = 4;
  number_of_oriented_photos = 4;
  number_of_position_valid_photos = 4;
  number_of_rotation_valid_photos = 4;
  number_of_valid_photos = 4;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;
  photo_id_offset += 20;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(0, blocks[0]);
  blocks_widget.AddBlock(1, blocks[1]);
  blocks_widget.show();

  std::vector<uint> photo_ids;
  photo_ids.push_back(14);
  photo_ids.push_back(16);
  photo_ids.push_back(18);
  photo_ids.push_back(20);
  blocks_widget.RemovePhotosByIds(photo_ids);

  blocks[0].photos.erase(14);
  blocks[1].photos.erase(16);
  blocks[1].photos.erase(18);
  blocks[1].photos.erase(20);

  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));
}

void TestBlocksTreeWidget::TestRemovePhotosBySelectedItems()
{
  uint block_id;
  BlockEntry block;
  int number_of_not_at_all_photos;
  int number_of_oriented_photos;
  int number_of_position_valid_photos;
  int number_of_rotation_valid_photos;
  int number_of_valid_photos;
  BlockContainer blocks;
  uint photo_id_offset = 0;

  block_id = 0;
  block.id = block_id;
  block.name = tr("test_block_0");
  number_of_not_at_all_photos = 3;
  number_of_oriented_photos = 3;
  number_of_position_valid_photos = 3;
  number_of_rotation_valid_photos = 3;
  number_of_valid_photos = 3;
  GeneratePhotos(number_of_not_at_all_photos,
                 number_of_oriented_photos,
                 number_of_position_valid_photos,
                 number_of_rotation_valid_photos,
                 number_of_valid_photos,
                 photo_id_offset,
                 block.photos);
  blocks[block_id] = block;

  BlocksTreeWidget blocks_widget;
  blocks_widget.AddBlock(block_id, blocks[block_id]);

  blocks_widget.show();

  QTest::mouseClick(blocks_widget.viewport(), Qt::LeftButton, 0,
                    QPoint(80, 62));
  QTest::mouseClick(blocks_widget.viewport(), Qt::LeftButton,
                    Qt::ShiftModifier, QPoint(83, 104));
  QTest::mouseClick(blocks_widget.viewport(), Qt::LeftButton,
                    Qt::ControlModifier, QPoint(69, 132));
  QTest::mouseClick(blocks_widget.viewport(), Qt::LeftButton,
                    Qt::ControlModifier, QPoint(93, 158));

  blocks_widget.RemovePhotosBySelectedItems();

  blocks[block_id].photos.erase(1);
  blocks[block_id].photos.erase(2);
  blocks[block_id].photos.erase(3);
  blocks[block_id].photos.erase(5);
  blocks[block_id].photos.erase(6);

  QCOMPARE(true, ValidateBlocksTreeWidget(blocks, &blocks_widget));

  //qApp->exec();
}

}

QTEST_MAIN(test::TestBlocksTreeWidget)
#include "test_blocks_tree_widget.moc"
