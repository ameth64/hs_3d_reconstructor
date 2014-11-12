#include <QtTest/QtTest>
#include <QApplication>

#include <random>

#include "gui/feature_match_info_widget.hpp"

#include "test_utility.hpp"

namespace test
{

class TestFeatureMatchInfoWidget : public QObject
{
  Q_OBJECT
public:
  typedef hs::recon::FeatureMatchInfoWidget FeatureMatchInfoWidget;
  typedef FeatureMatchInfoWidget::PhotoFeatureEntry PhotoFeatureEntry;
  typedef FeatureMatchInfoWidget::PhotoFeatureEntryContainer
          PhotoFeatureEntryContainer;

  static void GeneratePhotoFeatureEntries(uint number_of_photos,
                                          uint min_number_of_keypoints,
                                          uint max_number_of_keypoints,
                                          uint min_number_of_matches,
                                          uint max_number_of_matches,
                                          uint photo_id_offset,
                                          PhotoFeatureEntryContainer& entries);

  static bool ValidateFeatureMatchInfoWidget(
    const PhotoFeatureEntryContainer& entries,
    FeatureMatchInfoWidget* feature_widget);

private slots:
  void TestSet();
};

void TestFeatureMatchInfoWidget::GeneratePhotoFeatureEntries(
  uint number_of_photos,
  uint min_number_of_keypoints,
  uint max_number_of_keypoints,
  uint min_number_of_matches,
  uint max_number_of_matches,
  uint photo_id_offset,
  PhotoFeatureEntryContainer& entries)
{
  std::mt19937 random_generator;
  std::uniform_int_distribution<uint> keypoints_distribution(
    min_number_of_keypoints, max_number_of_keypoints);
  std::uniform_int_distribution<uint> matches_distribution(
    min_number_of_matches, max_number_of_matches);
  entries.clear();
  for (uint photo_id = photo_id_offset;
       photo_id < photo_id_offset + number_of_photos; ++photo_id)
  {
    PhotoFeatureEntry entry;
    entry.photo_id = photo_id;
    entry.photo_name = QString(tr("%1.JPG")).arg(photo_id, 5, 10,
                                                 QLatin1Char('0'));
    entry.number_of_keypoints = keypoints_distribution(random_generator);
    entry.number_of_matches = matches_distribution(random_generator);
    entries[photo_id] = entry;
  }
}

bool TestFeatureMatchInfoWidget::ValidateFeatureMatchInfoWidget(
  const PhotoFeatureEntryContainer& entries,
  FeatureMatchInfoWidget* feature_widget)
{
  QIcon photo_icon(QIcon(":/images/icon_photo.png"));

  size_t number_of_entries = entries.size();
  if (number_of_entries != size_t(feature_widget->rowCount()))
    return false;
  auto itr_entry = entries.begin();
  auto itr_entry_end = entries.end();
  for (int entry_item_id = 0; itr_entry != itr_entry_end;
       ++itr_entry, ++entry_item_id)
  {
    QTableWidgetItem* photo_item = feature_widget->item(entry_item_id, 0);
    QTableWidgetItem* keypoints_item = feature_widget->item(entry_item_id, 1);
    QTableWidgetItem* matches_item = feature_widget->item(entry_item_id, 2);
    if (itr_entry->first != photo_item->data(Qt::UserRole).toUInt())
      return false;
    if (!IconCompare(photo_item->icon(), photo_icon))
      return false;
    if (itr_entry->second.photo_name != photo_item->text())
      return false;
    if (itr_entry->second.number_of_keypoints  !=
        keypoints_item->text().toUInt())
      return false;
    if (itr_entry->second.number_of_matches != matches_item->text().toUInt())
      return false;
  }

  return true;
}

void TestFeatureMatchInfoWidget::TestSet()
{
  uint number_of_photos = 2000;
  uint min_number_of_keypoints = 38000;
  uint max_number_of_keypoints = 40000;
  uint min_number_of_matches = 8000;
  uint max_number_of_matches = 10000;
  uint photo_id_offset = 0;
  PhotoFeatureEntryContainer entries;
  GeneratePhotoFeatureEntries(number_of_photos,
                             min_number_of_keypoints,
                             max_number_of_keypoints,
                             min_number_of_matches,
                             max_number_of_matches,
                             photo_id_offset,
                             entries);

  FeatureMatchInfoWidget feature_widget;
  feature_widget.Set(entries);
  QCOMPARE(true, ValidateFeatureMatchInfoWidget(entries, &feature_widget));
  feature_widget.Clear();
  entries.clear();
  ValidateFeatureMatchInfoWidget(entries, &feature_widget);

  feature_widget.show();

  qApp->exec();
}

}

QTEST_MAIN(test::TestFeatureMatchInfoWidget);
#include "test_feature_match_info_widget.moc"
