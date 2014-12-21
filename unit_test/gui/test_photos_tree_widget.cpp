#include <QtTest/QtTest>

#include "hs_math/random/uniform_random_var.hpp"

#include "test_utility.hpp"
#include "gui/photos_tree_widget.hpp"

namespace test
{

class TestPhotosTreeWidget : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::PhotosTreeWidget PhotosTreeWidget;
  typedef PhotosTreeWidget::Float Float;
  typedef PhotosTreeWidget::PhotoEntry PhotoEntry;
  typedef PhotosTreeWidget::PhotoContainer PhotoContainer;
  typedef PhotosTreeWidget::GroupEntry GroupEntry;
  typedef std::map<uint, GroupEntry> GroupContainer;

  static void GeneratePhotos(int number_of_photos,
                             uint id_offset,
                             PhotoContainer& photos);

  static bool ValidatePhotosTreeWidget(const GroupContainer& groups,
                                       PhotosTreeWidget* photos_widget);
private slots:
  void TestAddGroup();
  void TestAddPhotos();
  void TestDeleteGroupsByIds();
  void TestDeleteGroupsBySelectedItems();
  void TestRemovePhotosByIds();
  void TestRemovePhotosBySelectedItems();
};

void TestPhotosTreeWidget::GeneratePhotos(int number_of_photos,
                                          uint id_offset,
                                          PhotoContainer& photos)
{
  photos.clear();
  for (int i = 0; i < number_of_photos; i++)
  {
    PhotoEntry photo;
    photo.id = id_offset + i;
    photo.file_name = QString("DSC000%1.jpg").arg(i);
    hs::math::random::UniformRandomVar<Float, 1>::Generate(
      Float(108), Float(109), photo.x);
    hs::math::random::UniformRandomVar<Float, 1>::Generate(
      Float(24), Float(25), photo.y);
    hs::math::random::UniformRandomVar<Float, 1>::Generate(
      Float(0), Float(100), photo.z);
    hs::math::random::UniformRandomVar<Float, 1>::Generate(
      Float(0), Float(15), photo.pitch);
    hs::math::random::UniformRandomVar<Float, 1>::Generate(
      Float(0), Float(15), photo.roll);
    hs::math::random::UniformRandomVar<Float, 1>::Generate(
      Float(0), Float(360), photo.heading);
    photo.projection = tr("WGS84");
    photos[photo.id] = photo;
  }
}

bool TestPhotosTreeWidget::ValidatePhotosTreeWidget(
  const GroupContainer& groups, PhotosTreeWidget* photos_widget)
{
  QIcon group_icon(":/images/icon_group.png");
  QIcon photo_icon(":/images/icon_photo.png");

  int group_item_count = photos_widget->topLevelItemCount();
  if (group_item_count != int(groups.size())) return false;
  for (int i = 0; i < group_item_count; i++)
  {
    QTreeWidgetItem* group_item = photos_widget->topLevelItem(i);
    QIcon item_group_icon = group_item->icon(0);
    if (!IconCompare(group_icon, item_group_icon)) return false;
    uint group_id = group_item->data(0, Qt::UserRole).toUInt();
    auto itr_group = groups.find(group_id);
    if (itr_group == groups.end()) return false;
    int photo_item_count = group_item->childCount();
    if (photo_item_count != int(itr_group->second.photos.size())) return false;
    for (int j = 0; j < photo_item_count; j++)
    {
      QTreeWidgetItem* photo_item = group_item->child(j);
      QIcon item_photo_icon = photo_item->icon(0);
      if (!IconCompare(photo_icon, item_photo_icon)) return false;
      uint photo_id = photo_item->data(0, Qt::UserRole).toUInt();
      auto itr_photo = itr_group->second.photos.find(photo_id);
      if (itr_photo == itr_group->second.photos.end()) return false;
      if (itr_photo->second.file_name != photo_item->text(0)) return false;

      QString x_text = QString::number(Float(itr_photo->second.x));
      if (x_text != photo_item->text(1)) return false;
      QString y_text = QString::number(Float(itr_photo->second.y));
      if (y_text != photo_item->text(2)) return false;
      QString z_text = QString::number(Float(itr_photo->second.z));
      if (z_text != photo_item->text(3)) return false;
      QString pitch_text = QString::number(Float(itr_photo->second.pitch));
      if (pitch_text != photo_item->text(4)) return false;
      QString roll_text = QString::number(Float(itr_photo->second.roll));
      if (roll_text != photo_item->text(5)) return false;
      QString heading_text = QString::number(Float(itr_photo->second.heading));
      if (heading_text != photo_item->text(6)) return false;
      if (itr_photo->second.projection != photo_item->text(7)) return false;
    }
  }

  return true;
}

void TestPhotosTreeWidget::TestAddGroup()
{
  int number_of_photos = 1000;
  GroupEntry group;
  group.id = 0;
  group.name = tr("test_photo_0");
  GeneratePhotos(number_of_photos, 0, group.photos);

  PhotosTreeWidget photo_widget;
  photo_widget.AddGroup(group.id, group);
  photo_widget.show();

  GroupContainer groups;
  groups[group.id] = group;

  QCOMPARE(true, ValidatePhotosTreeWidget(groups, &photo_widget));

  //qApp->exec();
}

void TestPhotosTreeWidget::TestAddPhotos()
{
  uint group_id;
  GroupEntry group;
  int number_of_photos;
  GroupContainer groups;

  group_id = 0;
  group.id = group_id;
  group.name = tr("test_photo_0");
  number_of_photos = 10;
  GeneratePhotos(number_of_photos, 0, group.photos);
  groups[group_id] = group;

  PhotosTreeWidget photos_widget;
  photos_widget.AddGroup(group_id, group);

  PhotoContainer photos_to_add;
  GeneratePhotos(5, number_of_photos, photos_to_add);
  photos_widget.AddPhotos(group_id, photos_to_add);

  groups[group_id].photos.insert(photos_to_add.begin(), photos_to_add.end());

  QCOMPARE(true, ValidatePhotosTreeWidget(groups, &photos_widget));
}

void TestPhotosTreeWidget::TestDeleteGroupsByIds()
{
  uint group_id;
  GroupEntry group;
  int number_of_photos;
  GroupContainer groups;
  uint photo_id_offset = 0;

  group_id = 0;
  group.id = group_id;
  group.name = tr("test_photo_0");
  number_of_photos = 3;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  group_id = 1;
  group.id = group_id;
  group.name = tr("test_photo_1");
  number_of_photos = 4;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  group_id = 2;
  group.id = group_id;
  group.name = tr("test_photo_2");
  number_of_photos = 5;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  PhotosTreeWidget photos_widget;
  photos_widget.AddGroup(0, groups[0]);
  photos_widget.AddGroup(1, groups[1]);
  photos_widget.AddGroup(2, groups[2]);

  photos_widget.show();
  //qApp->exec();

  std::vector<uint> group_ids;
  group_ids.push_back(1);
  group_ids.push_back(2);
  photos_widget.DeleteGroupsByIds(group_ids);

  groups.erase(1);
  groups.erase(2);

  QCOMPARE(true, ValidatePhotosTreeWidget(groups, &photos_widget));
}

void TestPhotosTreeWidget::TestDeleteGroupsBySelectedItems()
{
  uint group_id;
  GroupEntry group;
  int number_of_photos;
  GroupContainer groups;
  uint photo_id_offset = 0;

  group_id = 0;
  group.id = group_id;
  group.name = tr("test_photo_0");
  number_of_photos = 3;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  group_id = 1;
  group.id = group_id;
  group.name = tr("test_photo_1");
  number_of_photos = 4;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  group_id = 2;
  group.id = group_id;
  group.name = tr("test_photo_2");
  number_of_photos = 5;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  PhotosTreeWidget photos_widget;
  photos_widget.show();
  photos_widget.resize(200, 400);
  photos_widget.AddGroup(0, groups[0]);
  photos_widget.AddGroup(1, groups[1]);
  photos_widget.AddGroup(2, groups[2]);

  QTest::mouseClick(photos_widget.viewport(), Qt::LeftButton, 0,
                    QPoint(50, 80));
  QTest::mouseClick(photos_widget.viewport(), Qt::LeftButton,
                    Qt::ControlModifier, QPoint(50, 170));
  photos_widget.DeleteGroupsBySelectedItems();

  groups.erase(1);
  groups.erase(2);
  QCOMPARE(true, ValidatePhotosTreeWidget(groups, &photos_widget));
  //qApp->exec();
}

void TestPhotosTreeWidget::TestRemovePhotosByIds()
{
  uint group_id;
  GroupEntry group;
  int number_of_photos;
  GroupContainer groups;
  uint photo_id_offset = 0;

  group_id = 0;
  group.id = group_id;
  group.name = tr("test_photo_0");
  number_of_photos = 3;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  group_id = 1;
  group.id = group_id;
  group.name = tr("test_photo_1");
  number_of_photos = 4;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  PhotosTreeWidget photos_widget;
  photos_widget.show();
  photos_widget.resize(200, 400);
  photos_widget.AddGroup(0, groups[0]);
  photos_widget.AddGroup(1, groups[1]);

  std::vector<uint> photo_ids;
  photo_ids.push_back(1);
  photo_ids.push_back(2);
  photo_ids.push_back(4);
  photo_ids.push_back(5);
  photos_widget.RemovePhotosByIds(photo_ids);

  groups[0].photos.erase(1);
  groups[0].photos.erase(2);
  groups[1].photos.erase(4);
  groups[1].photos.erase(5);

  QCOMPARE(true, ValidatePhotosTreeWidget(groups, &photos_widget));
}

void TestPhotosTreeWidget::TestRemovePhotosBySelectedItems()
{
  uint group_id;
  GroupEntry group;
  int number_of_photos;
  GroupContainer groups;
  uint photo_id_offset = 0;

  group_id = 0;
  group.id = group_id;
  group.name = tr("test_photo_0");
  number_of_photos = 3;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  group_id = 1;
  group.id = group_id;
  group.name = tr("test_photo_1");
  number_of_photos = 4;
  GeneratePhotos(number_of_photos, photo_id_offset, group.photos);
  groups[group_id] = group;
  photo_id_offset += number_of_photos;

  PhotosTreeWidget photos_widget;
  photos_widget.show();
  photos_widget.resize(200, 400);
  photos_widget.AddGroup(0, groups[0]);
  photos_widget.AddGroup(1, groups[1]);

  QTest::mouseClick(photos_widget.viewport(), Qt::LeftButton,
                    0, QPoint(50, 100));
  QTest::mouseClick(photos_widget.viewport(), Qt::LeftButton,
                    Qt::ShiftModifier, QPoint(50, 140));
  QTest::mouseClick(photos_widget.viewport(), Qt::LeftButton,
                    Qt::ControlModifier, QPoint(50, 70));
  QTest::mouseClick(photos_widget.viewport(), Qt::LeftButton,
                    Qt::ControlModifier, QPoint(50, 50));

  photos_widget.RemovePhotosBySelectedItems();

  groups[0].photos.erase(1);
  groups[0].photos.erase(2);
  groups[1].photos.erase(3);
  groups[1].photos.erase(4);
  groups[1].photos.erase(5);

  QCOMPARE(true, ValidatePhotosTreeWidget(groups, &photos_widget));
  //qApp->exec();
}

}

QTEST_MAIN(test::TestPhotosTreeWidget)
#include "test_photos_tree_widget.moc"
