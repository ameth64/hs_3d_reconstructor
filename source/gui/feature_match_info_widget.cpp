#include "feature_match_info_widget.hpp"

namespace hs
{
namespace recon
{

FeatureMatchInfoWidget::FeatureMatchInfoWidget(QWidget* parent)
  : QTableWidget(parent),
    photo_icon_(":/images/icon_photo.png")
{
  setSortingEnabled(true);
  setColumnCount(3);
  setHorizontalHeaderLabels(QStringList()<<tr("Photo Name")
                                         <<tr("Keypoints")
                                         <<tr("Matches"));
}

void FeatureMatchInfoWidget::Set(
  const PhotoFeatureEntryContainer& photo_feature_entries)
{
  Clear();
  setRowCount(int(photo_feature_entries.size()));
  auto itr_entry = photo_feature_entries.begin();
  auto itr_entry_end = photo_feature_entries.end();
  for (int row = 0; itr_entry != itr_entry_end; ++itr_entry, row++)
  {
    QTableWidgetItem* name_item =
      new QTableWidgetItem(photo_icon_, itr_entry->second.photo_name);
    name_item->setFlags(name_item->flags() ^ Qt::ItemIsEditable);
    name_item->setData(Qt::UserRole, QVariant(itr_entry->first));
    setItem(row, 0, name_item);

    QTableWidgetItem* keypoints_item =
      new QTableWidgetItem(
        QString::number(itr_entry->second.number_of_keypoints));
    keypoints_item->setFlags(keypoints_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 1, keypoints_item);

    QTableWidgetItem* matches_item =
      new QTableWidgetItem(
        QString::number(itr_entry->second.number_of_matches));
    matches_item->setFlags(matches_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 2, matches_item);

  }
}

void FeatureMatchInfoWidget::Clear()
{
  int row_count = rowCount();
  for (int i = 0; i < row_count; i++)
  {
    removeRow(0);
  }
}

}
}
