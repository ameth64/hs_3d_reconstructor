#include "photo_orientation_info_widget.hpp"

namespace hs
{
namespace recon
{

QString PhotoOrientationInfoWidget::position_unit_texts_[
  NUMBER_OF_POSITION_UNIT_FLAGS] =
{
  QString(tr("")),
  QString(tr("(degree)")),
  QString(tr("(meter)"))
};

QString PhotoOrientationInfoWidget::rotation_unit_texts_[
  NUMBER_OF_ROTATION_UNIT_FLAGS] =
{
  QString(tr("(degree)")),
  QString(tr("(radian)"))
};

QString PhotoOrientationInfoWidget::column_names_[number_of_columns_] =
{
  QString(tr("Photo Name")),
  QString(tr("X")),
  QString(tr("Y")),
  QString(tr("Z")),
  QString(tr("Pitch")),
  QString(tr("Roll")),
  QString(tr("Heading")),
  QString(tr("Error(Pixel)"))
};

PhotoOrientationInfoWidget::PhotoOrientationInfoWidget(QWidget* parent)
  : QTableWidget(parent),
    photo_icon_(":/images/icon_photo.png")
{
  setSortingEnabled(true);
  setColumnCount(number_of_columns_);
  QStringList header_labels;
  for (int i = 0; i < number_of_columns_; i++)
  {
    header_labels<<column_names_[i];
  }
  setHorizontalHeaderLabels(header_labels);
}

void PhotoOrientationInfoWidget::SetPlanarUnit(PositionUnitFlag unit)
{
  QTableWidgetItem* x_item = horizontalHeaderItem(1);
  QTableWidgetItem* y_item = horizontalHeaderItem(2);
  QString x_text = column_names_[1] + " " + position_unit_texts_[unit];
  QString y_text = column_names_[2] + " " + position_unit_texts_[unit];
  x_item->setText(x_text);
  y_item->setText(y_text);
}

void PhotoOrientationInfoWidget::SetHeightUnit(PositionUnitFlag unit)
{
  QTableWidgetItem* z_item = horizontalHeaderItem(3);
  QString z_text = column_names_[3] + " " + position_unit_texts_[unit];
  z_item->setText(z_text);
}

void PhotoOrientationInfoWidget::SetRotationUnit(RotationUnitFlag unit)
{
  QTableWidgetItem* pitch_item = horizontalHeaderItem(4);
  QTableWidgetItem* roll_item = horizontalHeaderItem(5);
  QTableWidgetItem* heading_item = horizontalHeaderItem(6);
  QString pitch_text = column_names_[4] + " " + rotation_unit_texts_[unit];
  QString roll_text = column_names_[5] + " " + rotation_unit_texts_[unit];
  QString heading_text = column_names_[6] + " " + rotation_unit_texts_[unit];
  pitch_item->setText(pitch_text);
  roll_item->setText(roll_text);
  heading_item->setText(heading_text);
}

void PhotoOrientationInfoWidget::SetEntries(
  const PhotoOrientationEntryContainer& entries)
{
  Clear();
  setRowCount(int(entries.size()));
  auto itr_entry = entries.begin();
  auto itr_entry_end = entries.end();
  for (int row = 0; itr_entry != itr_entry_end; ++itr_entry, ++row)
  {
    QTableWidgetItem* name_item =
      new QTableWidgetItem(photo_icon_, itr_entry->second.photo_name);
    name_item->setFlags(name_item->flags() ^ Qt::ItemIsEditable);
    name_item->setData(Qt::UserRole, QVariant(itr_entry->first));
    setItem(row, 0, name_item);

    QTableWidgetItem* x_item =
      new QTableWidgetItem(QString::number(itr_entry->second.pos_x));
    x_item->setFlags(x_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 1, x_item);

    QTableWidgetItem* y_item =
      new QTableWidgetItem(QString::number(itr_entry->second.pos_y));
    y_item->setFlags(y_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 2, y_item);

    QTableWidgetItem* z_item =
      new QTableWidgetItem(QString::number(itr_entry->second.pos_z));
    z_item->setFlags(z_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 3, z_item);

    QTableWidgetItem* pitch_item =
      new QTableWidgetItem(QString::number(itr_entry->second.pitch));
    pitch_item->setFlags(pitch_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 4, pitch_item);

    QTableWidgetItem* roll_item =
      new QTableWidgetItem(QString::number(itr_entry->second.roll));
    roll_item->setFlags(roll_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 5, roll_item);

    QTableWidgetItem* heading_item =
      new QTableWidgetItem(QString::number(itr_entry->second.heading));
    heading_item->setFlags(heading_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 6, heading_item);

    QTableWidgetItem* error_item =
      new QTableWidgetItem(QString::number(
                             itr_entry->second.reprojection_error));
    error_item->setFlags(error_item->flags() ^ Qt::ItemIsEditable);
    setItem(row, 7, error_item);
  }
}

void PhotoOrientationInfoWidget::Clear()
{
  int row_count = rowCount();
  for (int i = 0; i < row_count; i++)
  {
    removeRow(0);
  }
}

}
}
