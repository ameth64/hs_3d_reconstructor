#include <QtTest/QtTest>
#include <QApplication>

#include <random>

#include "gui/photo_orientation_info_widget.hpp"

#include "test_utility.hpp"

namespace test
{

class TestPhotoOrientationInfoWidget : public QObject
{
  Q_OBJECT
public:
  typedef hs::recon::PhotoOrientationInfoWidget PhotoOrientationInfoWidget;
  typedef PhotoOrientationInfoWidget::PhotoOrientationEntry
          PhotoOrientationEntry;
  typedef PhotoOrientationInfoWidget::PhotoOrientationEntryContainer
          PhotoOrientationEntryContainer;
  typedef PhotoOrientationInfoWidget::Float Float;

  static bool ValidatePhotoOrientationInfoWidget(
    const PhotoOrientationEntryContainer& entries,
    PhotoOrientationInfoWidget::PositionUnitFlag planar_unit,
    PhotoOrientationInfoWidget::PositionUnitFlag height_unit,
    PhotoOrientationInfoWidget::RotationUnitFlag rotation_unit,
    PhotoOrientationInfoWidget* widget);

private slots:
  void Test();
};

bool TestPhotoOrientationInfoWidget::ValidatePhotoOrientationInfoWidget(
  const PhotoOrientationEntryContainer& entries,
  PhotoOrientationInfoWidget::PositionUnitFlag planar_unit,
  PhotoOrientationInfoWidget::PositionUnitFlag height_unit,
  PhotoOrientationInfoWidget::RotationUnitFlag rotation_unit,
  PhotoOrientationInfoWidget* widget)
{
  QIcon photo_icon(QIcon(":/images/icon_photo.png"));
  size_t number_of_entries = entries.size();
  if (number_of_entries != size_t(widget->rowCount()))
    return false;
  auto itr_entry = entries.begin();
  auto itr_entry_end = entries.end();
  for (int entry_item_id = 0; itr_entry != itr_entry_end;
       ++itr_entry, ++entry_item_id)
  {
    QTableWidgetItem* photo_item = widget->item(entry_item_id, 0);
    QTableWidgetItem* x_item = widget->item(entry_item_id, 1);
    QTableWidgetItem* y_item = widget->item(entry_item_id, 2);
    QTableWidgetItem* z_item = widget->item(entry_item_id, 3);
    QTableWidgetItem* pitch_item = widget->item(entry_item_id, 4);
    QTableWidgetItem* roll_item = widget->item(entry_item_id, 5);
    QTableWidgetItem* heading_item = widget->item(entry_item_id, 6);
    QTableWidgetItem* error_item = widget->item(entry_item_id, 7);
    if (itr_entry->first != photo_item->data(Qt::UserRole).toUInt())
      return false;
    if (!IconCompare(photo_item->icon(), photo_icon))
      return false;
    if (itr_entry->second.photo_name != photo_item->text())
      return false;
    if (QString::number(itr_entry->second.pos_x) != x_item->text())
      return false;
    if (QString::number(itr_entry->second.pos_y) != y_item->text())
      return false;
    if (QString::number(itr_entry->second.pos_z) != z_item->text())
      return false;
    if (QString::number(itr_entry->second.pitch) != pitch_item->text())
      return false;
    if (QString::number(itr_entry->second.roll) != roll_item->text())
      return false;
    if (QString::number(itr_entry->second.heading) != heading_item->text())
      return false;
    if (QString::number(itr_entry->second.reprojection_error) !=
        error_item->text())
      return false;
  }

  QString x_header_text = widget->horizontalHeaderItem(1)->text();
  QString y_header_text = widget->horizontalHeaderItem(2)->text();
  QString z_header_text = widget->horizontalHeaderItem(3)->text();
  QString pitch_header_text = widget->horizontalHeaderItem(4)->text();
  QString roll_header_text = widget->horizontalHeaderItem(5)->text();
  QString heading_header_text = widget->horizontalHeaderItem(6)->text();

  QString expected_x_header_text =
    PhotoOrientationInfoWidget::column_names_[1] + " " +
    PhotoOrientationInfoWidget::position_unit_texts_[planar_unit];
  QString expected_y_header_text =
    PhotoOrientationInfoWidget::column_names_[2] + " " +
    PhotoOrientationInfoWidget::position_unit_texts_[planar_unit];
  QString expected_z_header_text =
    PhotoOrientationInfoWidget::column_names_[3] + " " +
    PhotoOrientationInfoWidget::position_unit_texts_[height_unit];
  QString expected_pitch_header_text =
    PhotoOrientationInfoWidget::column_names_[4] + " " +
    PhotoOrientationInfoWidget::rotation_unit_texts_[rotation_unit];
  QString expected_roll_header_text =
    PhotoOrientationInfoWidget::column_names_[5] + " " +
    PhotoOrientationInfoWidget::rotation_unit_texts_[rotation_unit];
  QString expected_heading_header_text =
    PhotoOrientationInfoWidget::column_names_[6] + " " +
    PhotoOrientationInfoWidget::rotation_unit_texts_[rotation_unit];

  if (x_header_text != expected_x_header_text)
    return false;
  if (y_header_text != expected_y_header_text)
    return false;
  if (z_header_text != expected_z_header_text)
    return false;
  if (pitch_header_text != expected_pitch_header_text)
    return false;
  if (roll_header_text != expected_roll_header_text)
    return false;
  if (heading_header_text != expected_heading_header_text)
    return false;

  return true;
}

void TestPhotoOrientationInfoWidget::Test()
{
  PhotoOrientationEntry entry1 =
  {
    1,
    QString(tr("DSC00432.JPG")),
    Float(108.281637),
    Float(22.848552),
    Float(151.665707),
    Float(78.983),
    Float(-106.660),
    Float(307.951),
    Float(0.5)
  };
  PhotoOrientationEntry entry2 =
  {
    2,
    QString(tr("DSC00433.JPG")),
    Float(108.247397),
    Float(22.886737),
    Float(151.880472),
    Float(81.832),
    Float(-76.571),
    Float(339.042),
    Float(0.45)
  };
  PhotoOrientationEntry entry3 =
  {
    3,
    QString(tr("DSC00434.JPG")),
    Float(108.187657),
    Float(22.980626),
    Float(151.539856),
    Float(-55.464),
    Float(-96.998),
    Float(329.913),
    Float(0.58)
  };
  PhotoOrientationEntry entry4 =
  {
    4,
    QString(tr("DSC00435.JPG")),
    Float(108.179570),
    Float(22.008955),
    Float(151.275192),
    Float(-61.312),
    Float(-95.387),
    Float(329.820),
    Float(0.48)
  };
  PhotoOrientationInfoWidget widget;
  widget.show();
  PhotoOrientationEntryContainer entries;
  entries[entry1.photo_id] = entry1;
  entries[entry2.photo_id] = entry2;
  entries[entry3.photo_id] = entry3;
  entries[entry4.photo_id] = entry4;

  widget.SetEntries(entries);
  widget.SetPlanarUnit(PhotoOrientationInfoWidget::POSITION_UNIT_DEGREE);
  widget.SetHeightUnit(PhotoOrientationInfoWidget::POSITION_UNIT_METER);
  widget.SetRotationUnit(PhotoOrientationInfoWidget::ROTATION_UNIT_DEGREE);
  QCOMPARE(true, ValidatePhotoOrientationInfoWidget(
                   entries,
                   PhotoOrientationInfoWidget::POSITION_UNIT_DEGREE,
                   PhotoOrientationInfoWidget::POSITION_UNIT_METER,
                   PhotoOrientationInfoWidget::ROTATION_UNIT_DEGREE,
                   &widget));
  widget.SetEntries(entries);
  widget.SetPlanarUnit(PhotoOrientationInfoWidget::POSITION_UNIT_DEGREE);
  widget.SetHeightUnit(PhotoOrientationInfoWidget::POSITION_UNIT_METER);
  widget.SetRotationUnit(PhotoOrientationInfoWidget::ROTATION_UNIT_DEGREE);
  QCOMPARE(true, ValidatePhotoOrientationInfoWidget(
                   entries,
                   PhotoOrientationInfoWidget::POSITION_UNIT_DEGREE,
                   PhotoOrientationInfoWidget::POSITION_UNIT_METER,
                   PhotoOrientationInfoWidget::ROTATION_UNIT_DEGREE,
                   &widget));

  //widget.show();
  //qApp->exec();

  widget.SetPlanarUnit(PhotoOrientationInfoWidget::POSITION_UNIT_METER);
  widget.Clear();
  entries.clear();
  QCOMPARE(true, ValidatePhotoOrientationInfoWidget(
                   entries,
                   PhotoOrientationInfoWidget::POSITION_UNIT_METER,
                   PhotoOrientationInfoWidget::POSITION_UNIT_METER,
                   PhotoOrientationInfoWidget::ROTATION_UNIT_DEGREE,
                   &widget));

  //widget.show();
  //qApp->exec();
}

}

QTEST_MAIN(test::TestPhotoOrientationInfoWidget);
#include "test_photo_orientation_info_widget.moc"
