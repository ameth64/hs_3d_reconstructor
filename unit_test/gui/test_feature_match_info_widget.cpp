#include <QtTest/QtTest>
#include <QApplication>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "feature_match_info_widget.hpp"

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

  static void GeneratePhotoFeatureEntrys(uint number_of_photos,
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
  void TestConstructor();
};

void TestFeatureMatchInfoWidget::GeneratePhotoFeatureEntrys(
  uint number_of_photos,
  uint min_number_of_keypoints,
  uint max_number_of_keypoints,
  uint min_number_of_matches,
  uint max_number_of_matches,
  uint photo_id_offset,
  PhotoFeatureEntryContainer& entries)
{
  boost::random::mt19937 random_generator;
  boost::random::uniform_int_distribution<uint> keypoints_distribution(
    min_number_of_keypoints, max_number_of_keypoints);
  boost::random::uniform_int_distribution<uint> matches_distribution(
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
  return true;
}

void TestFeatureMatchInfoWidget::TestConstructor()
{
  uint number_of_photos = 2000;
  uint min_number_of_keypoints = 38000;
  uint max_number_of_keypoints = 40000;
  uint min_number_of_matches = 8000;
  uint max_number_of_matches = 10000;
  uint photo_id_offset = 0;
  PhotoFeatureEntryContainer entries;
  GeneratePhotoFeatureEntrys(number_of_photos,
                             min_number_of_keypoints,
                             max_number_of_keypoints,
                             min_number_of_matches,
                             max_number_of_matches,
                             photo_id_offset,
                             entries);

  FeatureMatchInfoWidget feature_widget(entries);
  feature_widget.show();

  qApp->exec();
}

}

QTEST_MAIN(test::TestFeatureMatchInfoWidget);
#include "test_feature_match_info_widget.moc"
