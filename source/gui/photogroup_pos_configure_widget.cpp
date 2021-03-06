﻿#include <string>
#include <streambuf>
#include <fstream>
#include <cmath>

#include <QFileDialog>
#include <QMessageBox>

#include "hs_cartographics/cartographics_qt/coordinate_system_config_dialog.hpp"
#include "hs_cartographics/cartographics_format/formatter_proj4.hpp"

#include "gui/photogroup_pos_configure_widget.hpp"
#include "gui/property_field_asignment_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

QStringList PhotogroupPOSConfigureWidget::HeaderLabels()
{
  return QStringList() << tr("Photo Name")
                       << tr("Coordinate X")
                       << tr("Coordinate Y")
                       << tr("Coordinate Z")
                       << tr("Pitch")
                       << tr("Roll")
                       << tr("Heading");
}

PhotogroupPOSConfigureWidget::PhotogroupPOSConfigureWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  Initialize();
}

PhotogroupPOSConfigureWidget::PhotogroupPOSConfigureWidget(
  const QStringList& photo_paths, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  Initialize();
  SetPhotoPaths(photo_paths);
}

void PhotogroupPOSConfigureWidget::SetPhotoPaths(const QStringList& photo_paths)
{
  while (table_pos_->rowCount() > 0)
  {
    table_pos_->removeRow(0);
  }

  table_pos_->setRowCount(int(photo_paths.size()));
  for (int i = 0; i < int(photo_paths.size()); i++)
  {
    QFileInfo file_info(photo_paths[i]);
    QString photo_label = file_info.fileName();
    QTableWidgetItem* label_item = new QTableWidgetItem(photo_label);
    label_item->setFlags(label_item->flags() ^ Qt::ItemIsEditable);
    label_item->setIcon(photo_icon_);
    label_item->setData(Qt::UserRole, QVariant(photo_paths[i]));
    table_pos_->setItem(i, 0, label_item);
    label_row_map_[photo_label] = i;
    for (int j = 1; j < table_pos_->columnCount(); j++)
    {
      QTableWidgetItem* item = new QTableWidgetItem;
      table_pos_->setItem(i, j, item);
    }
  }
  //table_pos_->resizeColumnsToContents();
}

PhotogroupPOSConfigureWidget::POSEntryContainer
PhotogroupPOSConfigureWidget::GetPOSEntries()
{
  POSEntryContainer pos_entries;
  for (int row = 0; row < table_pos_->rowCount(); row++)
  {
    POSEntry pos_entry;

    QTableWidgetItem* item_name = table_pos_->item(row, 0);
    pos_entry.photo_path = item_name->data(Qt::UserRole).toString();

    bool ok = false;

    QTableWidgetItem* item_x = table_pos_->item(row, 1);
    double x = item_x->text().toDouble(&ok);
    if (!ok) pos_entry.x = -std::numeric_limits<double>::max();
    else pos_entry.x = x;

    QTableWidgetItem* item_y = table_pos_->item(row, 2);
    double y = item_y->text().toDouble(&ok);
    if (!ok) pos_entry.y = -std::numeric_limits<double>::max();
    else pos_entry.y = y;

    QTableWidgetItem* item_z = table_pos_->item(row, 3);
    double z = item_z->text().toDouble(&ok);
    if (!ok) pos_entry.z = -std::numeric_limits<double>::max();
    else pos_entry.z = z;

    QTableWidgetItem* item_pitch = table_pos_->item(row, 4);
    double pitch = item_pitch->text().toDouble(&ok);
    if (!ok) pos_entry.pitch = -std::numeric_limits<double>::max();
    else pos_entry.pitch = pitch;

    QTableWidgetItem* item_roll = table_pos_->item(row, 5);
    double roll = item_roll->text().toDouble(&ok);
    if (!ok) pos_entry.roll = -std::numeric_limits<double>::max();
    else pos_entry.roll = roll;

    QTableWidgetItem* item_heading = table_pos_->item(row, 6);
    double heading = item_heading->text().toDouble(&ok);
    if (!ok) pos_entry.heading = -std::numeric_limits<double>::max();
    else pos_entry.heading = heading;

    std::string proj4_code;
    hs::cartographics::format::HS_FormatterProj4<double> formatter;
    formatter.CoordinateSystemToString(coordinate_system_, proj4_code);
    pos_entry.coordinate_system = QString::fromLocal8Bit(proj4_code.c_str());

    pos_entries.push_back(pos_entry);
  }

  return pos_entries;
}

void PhotogroupPOSConfigureWidget::Initialize()
{
  layout_all_ = new QVBoxLayout(this);
  layout_header_ = new QHBoxLayout;
  push_button_config_coordinate_system_ =
    new QPushButton(tr("Configure Coordinate System"), this);
  push_button_import_ = new QPushButton(tr("Import"), this);
  push_button_clear_ = new QPushButton(tr("Clear"), this);
  table_pos_ = new QTableWidget(this);

  photo_icon_.addFile(":/images/icon_photo.png");

  layout_all_->addLayout(layout_header_);
  layout_all_->addWidget(table_pos_);

  push_button_config_coordinate_system_->setSizePolicy(
    QSizePolicy(QSizePolicy::Preferred,
                QSizePolicy::Preferred));
  push_button_clear_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                                QSizePolicy::Preferred));
  push_button_import_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                                 QSizePolicy::Preferred));

  layout_header_->addWidget(push_button_config_coordinate_system_);
  layout_header_->addStretch();
  layout_header_->addWidget(push_button_clear_);
  layout_header_->addWidget(push_button_import_);

  table_pos_->setColumnCount(int(HeaderLabels().size()));
  table_pos_->setHorizontalHeaderLabels(HeaderLabels());
  table_pos_->resizeColumnsToContents();

  QObject::connect(
    push_button_config_coordinate_system_, &QPushButton::clicked,
    this,
    &PhotogroupPOSConfigureWidget::OnPushButtonConfigCoordinateSystemClicked);
  QObject::connect(
    push_button_import_, &QPushButton::clicked,
    this, &PhotogroupPOSConfigureWidget::OnPushButtonImportClicked);
  QObject::connect(
    push_button_clear_, &QPushButton::clicked,
    this, &PhotogroupPOSConfigureWidget::OnPushButtonClearClicked);
}

void PhotogroupPOSConfigureWidget::DoPosStatistic(
  double& x_mean, double& y_mean, double& x_stddev, double& y_stddev)
{
  x_mean = 0.0;
  y_mean = 0.0;
  x_stddev = 0.0;
  x_stddev = 0.0;
  size_t valid_x = 0;
  size_t valid_y = 0;
  for (int row = 0; row < table_pos_->rowCount(); row++)
  {
    QTableWidgetItem* item_x = table_pos_->item(row, 1);
    QTableWidgetItem* item_y = table_pos_->item(row, 2);
    bool ok = false;
    double x = item_x->text().toDouble(&ok);
    if (ok)
    {
      x_mean += x;
      valid_x++;
    }
    double y = item_y->text().toDouble(&ok);
    if (ok)
    {
      y_mean += y;
      valid_y++;
    }
  }
  x_mean /= double(valid_x);
  y_mean /= double(valid_y);
  for (int row = 0; row < table_pos_->rowCount(); row++)
  {
    QTableWidgetItem* item_x = table_pos_->item(row, 1);
    QTableWidgetItem* item_y = table_pos_->item(row, 2);
    bool ok = false;
    double x = item_x->text().toDouble(&ok);
    if (ok)
    {
      x_stddev += (x - x_mean) * (x - x_mean);
      valid_x++;
    }
    double y = item_y->text().toDouble(&ok);
    if (ok)
    {
      y_stddev += (y - y_mean) * (y - y_mean);
      valid_y++;
    }
  }
  x_stddev /= double(valid_x);
  y_stddev /= double(valid_y);
  x_stddev = std::sqrt(x_stddev);
  y_stddev = std::sqrt(y_stddev);
}

void PhotogroupPOSConfigureWidget::OnPushButtonConfigCoordinateSystemClicked()
{
  hs::cartographics::qt::CoordinateSystemConfigDialog dialog;
  dialog.SetCoordinateSystem(coordinate_system_);
  if (dialog.exec())
  {
    coordinate_system_ = dialog.GetCoordinateSystem();
  }
}

void PhotogroupPOSConfigureWidget::OnPushButtonImportClicked()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Character-separated values (*.csv *.txt)"));
  if (dialog.exec())
  {
    QString file_path = dialog.selectedFiles()[0];
    std::string std_file_path = file_path.toLocal8Bit().data();
    std::ifstream file(std_file_path);
    if (file)
    {
      std::string file_content =
        std::string(std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>());
      PropertyFieldAsignmentDialog assignment_dialog(
        file_content, HeaderLabels(), this);
      if (assignment_dialog.exec())
      {
        PropertyFieldAsignmentDialog::AssignedFieldValues assigned_field_values;
        assignment_dialog.GetAssignedFieldValues(assigned_field_values);
        auto itr_first_values =
          assigned_field_values.find(HeaderLabels()[0]);
        if (itr_first_values == assigned_field_values.end())
        {
          //Not aligned
          for (int column = 1; column < int(HeaderLabels().size()); column++)
          {
            auto itr_values =
              assigned_field_values.find(HeaderLabels()[column]);
            if (itr_values != assigned_field_values.end())
            {
              auto itr_value = itr_values->second.begin();
              auto itr_value_end = itr_values->second.end();
              for (int row = 0;
                   itr_value != itr_value_end &&
                   row < table_pos_->rowCount(); ++row, ++itr_value)
              {
                table_pos_->item(row, column)->setText(*itr_value);
              }
            }
          }
          //SetAligned(false);
        }
        else
        {
          //Aligned
          for (int column = 1; column < int(HeaderLabels().size()); column++)
          {
            auto itr_values =
              assigned_field_values.find(HeaderLabels()[column]);
            if (itr_values != assigned_field_values.end())
            {
              auto itr_value = itr_values->second.begin();
              auto itr_value_end = itr_values->second.end();
              for (int id = 0; itr_value != itr_value_end; ++itr_value, ++id)
              {
                QString first_value = itr_first_values->second[id];
                auto itr_label_row = label_row_map_.find(first_value);
                if (itr_label_row != label_row_map_.end())
                {
                  int row = itr_label_row->second;
                  QTableWidgetItem* item = table_pos_->item(row, column);
                  item->setText(*itr_value);
                }
              }
            }
          }
          //SetAligned(true);
        }
        double x_mean, y_mean, x_stddev, y_stddev;
        DoPosStatistic(x_mean, y_mean, x_stddev, y_stddev);
        if ((x_mean > -180 && x_mean < 180) &&
            (y_mean > -180 && y_mean < 180) &&
            (x_stddev < 10) &&
            (y_stddev < 10))
        {
          std::string prj4_code = "+proj=longlat +datum=WGS84";
          hs::cartographics::format::HS_FormatterProj4<double> formatter;
          formatter.StringToCoordinateSystem(prj4_code,
                                             coordinate_system_);
        }
        else
        {
          coordinate_system_ = CoordinateSystem();
        }
      }
    }
    else
    {
      QMessageBox message_box;
      message_box.setText(tr("Loading file failed!"));
      message_box.exec();
    }
  }
}

void PhotogroupPOSConfigureWidget::OnPushButtonClearClicked()
{
  for (int row = 0; row < table_pos_->rowCount(); row++)
  {
    for (int column = 1; column < table_pos_->columnCount(); column++)
    {
      QTableWidgetItem* item = table_pos_->item(row, column);
      item->setText("");
    }
  }
}

}
}
}
