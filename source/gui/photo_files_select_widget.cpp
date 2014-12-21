#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

#include <exiv2/exiv2.hpp>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/photo_files_select_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotoFileEntry::PhotoFileEntry()
  : width(0)
  , height(0)
  , focal_length(0.0f)
  , pixel_x_size(0.0f)
  , pixel_y_size(0.0f)
{
}

bool PhotoFileEntry::operator == (const PhotoFileEntry& other) const
{
  return (width == other.width &&
          height == other.height &&
          focal_length == other.focal_length &&
          pixel_x_size == other.pixel_x_size &&
          pixel_y_size == other.pixel_y_size);
}

bool PhotoFileEntry::operator != (const PhotoFileEntry& other) const
{
  return !(*this == other);
}

PhotoFilesSelectWidget::PhotoFilesSelectWidget(QWidget* parent,
                                               Qt::WindowFlags f)
  : QWidget(parent, f)
  , layout_buttons_(new QHBoxLayout)
  , layout_all_(new QVBoxLayout(this))
  , table_photos_(new QTableWidget(this))
  , push_button_add_photos_(new QPushButton(tr("Add Photos"), this))
  , push_button_add_directory_(new QPushButton(tr("Add Directory"), this))
  , push_button_remove_photos_(new QPushButton(tr("Remove Photos"), this))
  , photo_icon_(":/images/icon_photo.png")
{
  layout_all_->addLayout(layout_buttons_);
  layout_all_->addWidget(table_photos_);

  layout_buttons_->addWidget(push_button_add_photos_);
  layout_buttons_->addWidget(push_button_add_directory_);
  layout_buttons_->addWidget(push_button_remove_photos_);

  setLayout(layout_all_);

  push_button_remove_photos_->setEnabled(false);

  QStringList header_labels;
  header_labels << tr("Label")
                << tr("Width")
                << tr("Height")
                << tr("Focal length (mm)")
                << tr("Pixel x size (mm)")
                << tr("Pixel y size (mm)")
                << tr("Date & time")
                << tr("Make")
                << tr("Model")
                << tr("F-stop")
                << tr("ISO")
                << tr("Shutter");
  table_photos_->setColumnCount(int(header_labels.size()));
  table_photos_->setHorizontalHeaderLabels(header_labels);
  table_photos_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_photos_->resizeColumnsToContents();

  QObject::connect(push_button_add_photos_, &QPushButton::clicked,
                   this, &PhotoFilesSelectWidget::OnAddPhotosClicked);
  QObject::connect(
    table_photos_, &QTableWidget::itemSelectionChanged,
    this, &PhotoFilesSelectWidget::OnTablePhotoItemSelectionChanged);
  QObject::connect(push_button_remove_photos_, &QPushButton::clicked,
                   this, &PhotoFilesSelectWidget::OnRemovePhotosClicked);
  QObject::connect(push_button_add_directory_, &QPushButton::clicked,
                   this, &PhotoFilesSelectWidget::OnAddDirectoryClicked);
}

PhotoFilesSelectWidget::PhotoFileEntryContainer
PhotoFilesSelectWidget::GetPhotoFileEnties()
{
  PhotoFileEntryContainer photo_file_entries;
  int row_count = table_photos_->rowCount();
  for (int row = 0; row < row_count; ++row)
  {
    PhotoFileEntry photo_file_entry;
    photo_file_entry.width =
      table_photos_->item(row, 1)->text().toInt();
    photo_file_entry.height =
      table_photos_->item(row, 2)->text().toInt();
    photo_file_entry.focal_length =
      table_photos_->item(row, 3)->text().toFloat();
    photo_file_entry.pixel_x_size =
      table_photos_->item(row, 4)->text().toFloat();
    photo_file_entry.pixel_y_size =
      table_photos_->item(row, 5)->text().toFloat();
    photo_file_entries.push_back(photo_file_entry);
  }
  return photo_file_entries;
}

QStringList PhotoFilesSelectWidget::GetPhotoFilePaths()
{
  QStringList photo_file_paths;
  int row_count = table_photos_->rowCount();
  for (int row = 0; row < row_count; ++row)
  {
    QString photo_file_path =
      table_photos_->item(row, 0)->data(Qt::UserRole).toString();
    photo_file_paths.push_back(photo_file_path);
  }
  return photo_file_paths;
}

void PhotoFilesSelectWidget::OnAddPhotosClicked()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilter(tr("JPEG Files (*.jpg)"));
  if (dialog.exec())
  {
    QStringList file_names = dialog.selectedFiles();
    AddPhotos(file_names);
  }
}

void PhotoFilesSelectWidget::OnAddDirectoryClicked()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::Directory);
  if (dialog.exec())
  {
    QString directory_path = dialog.selectedFiles()[0];
    QDir dir(directory_path);
    dir.setNameFilters(QStringList()<<"*.jpg");
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QStringList photo_files = dir.entryList();
    QStringList absolute_photo_files;
    auto itr_photo_file = photo_files.begin();
    auto itr_photo_file_end = photo_files.end();
    for (; itr_photo_file != itr_photo_file_end; ++itr_photo_file)
    {
      absolute_photo_files.push_back(dir.absoluteFilePath(*itr_photo_file));
    }
    AddPhotos(absolute_photo_files);
  }
}

void PhotoFilesSelectWidget::OnRemovePhotosClicked()
{
  QList<QTableWidgetItem*> selected_items = table_photos_->selectedItems();
  QList<QTableWidgetItem*> selected_first_items;
  auto itr_selected_item = selected_items.begin();
  auto itr_selected_item_end = selected_items.end();
  for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
  {
    if ((*itr_selected_item)->column() == 0)
    {
      selected_first_items.push_back(*itr_selected_item);
    }
  }
  itr_selected_item = selected_first_items.begin();
  itr_selected_item_end = selected_first_items.end();
  for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
  {
    table_photos_->removeRow((*itr_selected_item)->row());
  }
  table_photos_->resizeColumnsToContents();
  emit PhotoFileEntriesChanged();
}

void PhotoFilesSelectWidget::OnTablePhotoItemSelectionChanged()
{
  if (table_photos_->selectedItems().empty())
  {
    push_button_remove_photos_->setEnabled(false);
  }
  else
  {
    push_button_remove_photos_->setEnabled(true);
  }
}

QList<QTableWidgetItem*> PhotoFilesSelectWidget::GetRowFromPhoto(
  const QString& photo_path)
{
  QList<QTableWidgetItem*> row;
  std::string std_photo_path = photo_path.toLocal8Bit().data();

  QTableWidgetItem* label_item = new QTableWidgetItem;
  row.push_back(label_item);
  QTableWidgetItem* width_item = new QTableWidgetItem;
  row.push_back(width_item);
  QTableWidgetItem* height_item = new QTableWidgetItem;
  row.push_back(height_item);
  QTableWidgetItem* focal_length_item = new QTableWidgetItem;
  row.push_back(focal_length_item);
  QTableWidgetItem* pixel_x_size_item = new QTableWidgetItem;
  row.push_back(pixel_x_size_item);
  QTableWidgetItem* pixel_y_size_item = new QTableWidgetItem;
  row.push_back(pixel_y_size_item);
  QTableWidgetItem* date_time_item = new QTableWidgetItem;
  row.push_back(date_time_item);
  QTableWidgetItem* make_item = new QTableWidgetItem;
  row.push_back(make_item);
  QTableWidgetItem* model_item = new QTableWidgetItem;
  row.push_back(model_item);
  QTableWidgetItem* f_stop_item = new QTableWidgetItem;
  row.push_back(f_stop_item);
  QTableWidgetItem* iso_item = new QTableWidgetItem;
  row.push_back(iso_item);
  QTableWidgetItem* shutter_item = new QTableWidgetItem;
  row.push_back(shutter_item);

  QFileInfo file_info(photo_path);
  label_item->setText(file_info.fileName());
  label_item->setIcon(photo_icon_);
  label_item->setData(Qt::UserRole, photo_path);

  while (1)
  {
    hs::imgio::whole::ImageIO image_io;
    size_t width, height;
    if (image_io.GetImageDimension(std_photo_path, width, height) != 0)
    {
      break;
    }
    width_item->setText(QString::number(width));
    height_item->setText(QString::number(height));

    Exiv2::Image::AutoPtr image =
      Exiv2::ImageFactory::open(std_photo_path.c_str());
    if (image.get() == 0)
    {
      break;
    }
    image->readMetadata();
    Exiv2::ExifData& exif_data = image->exifData();
    if (exif_data.empty())
    {
      break;
    }

    Exiv2::ExifData::const_iterator itr_focal_length = exif_data.findKey(
      Exiv2::ExifKey("Exif.Image.FocalLength"));
    if (itr_focal_length == exif_data.end())
    {
      itr_focal_length = exif_data.findKey(
        Exiv2::ExifKey("Exif.Photo.FocalLength"));
    }
    float focal_length = 0.0f;
    if (itr_focal_length != exif_data.end())
    {
      focal_length = itr_focal_length->value().toFloat();
      focal_length_item->setText(QString::number(focal_length));
    }

    float pixel_x_size = 0.0f;
    float pixel_y_size = 0.0f;
    Exiv2::ExifData::const_iterator itr_focal_length_in_35mm_film =
      exif_data.findKey(Exiv2::ExifKey("Exif.Photo.FocalLengthIn35mmFilm"));
    if (itr_focal_length_in_35mm_film != exif_data.end())
    {
      float focal_length_in_35mm_film =
        itr_focal_length_in_35mm_film->value().toFloat();
      float sensor_width =
        36.0f * focal_length / focal_length_in_35mm_film;
      float sensor_height =
        24.0f * focal_length / focal_length_in_35mm_film;
      pixel_x_size = sensor_width / float(width);
      //pixel_y_size = sensor_height / float(height);
      pixel_y_size = pixel_x_size;
    }
    else
    {
      float resolution_unit = 0.0f;
      Exiv2::ExifData::const_iterator itr_focal_plane_resolution_unit =
        exif_data.findKey(Exiv2::ExifKey(
            "Exif.Image.FocalPlaneResolutionUnit"));
      Exiv2::ExifData::const_iterator itr_focal_plane_x_resolution =
        exif_data.findKey(Exiv2::ExifKey(
            "Exif.Image.FocalPlaneXResolution"));
      Exiv2::ExifData::const_iterator itr_focal_plane_y_resolution =
        exif_data.findKey(Exiv2::ExifKey(
            "Exif.Image.FocalPlaneYResolution"));
      if (itr_focal_plane_resolution_unit == exif_data.end() ||
          itr_focal_plane_x_resolution == exif_data.end() ||
          itr_focal_plane_y_resolution == exif_data.end())
      {
        itr_focal_plane_resolution_unit =
          exif_data.findKey(Exiv2::ExifKey(
              "Exif.Photo.FocalPlaneResolutionUnit"));
        itr_focal_plane_x_resolution =
          exif_data.findKey(Exiv2::ExifKey(
              "Exif.Photo.FocalPlaneXResolution"));
        itr_focal_plane_y_resolution =
          exif_data.findKey(Exiv2::ExifKey(
              "Exif.Photo.FocalPlaneYResolution"));
      }
      if (itr_focal_plane_resolution_unit != exif_data.end() &&
          itr_focal_plane_x_resolution != exif_data.end() &&
          itr_focal_plane_y_resolution != exif_data.end())
      {
        short focal_plane_resolution_unit =
          itr_focal_plane_resolution_unit->value().toLong();
        float focal_plane_resolution_unit_mm = 0.0f;
        switch (focal_plane_resolution_unit)
        {
        case 2:
          {
            focal_plane_resolution_unit_mm = 25.4f;
            break;
          }
        case 3:
          {
            focal_plane_resolution_unit_mm = 10.0f;
            break;
          }
        }
        float focal_plane_x_resolution =
          itr_focal_plane_x_resolution->value().toFloat();
        if (focal_plane_x_resolution != 0.0f)
        {
          pixel_x_size = focal_plane_resolution_unit_mm /
                         focal_plane_x_resolution;
        }
        float focal_plane_y_resolution =
          itr_focal_plane_y_resolution->value().toFloat();
        if (focal_plane_y_resolution != 0.0f)
        {
          pixel_y_size = focal_plane_resolution_unit_mm /
                         focal_plane_y_resolution;
        }
      }
    }

    if (pixel_x_size > 0.0f)
    {
      pixel_x_size_item->setText(QString::number(pixel_x_size));
    }
    if (pixel_y_size > 0.0f)
    {
      pixel_y_size_item->setText(QString::number(pixel_y_size));
    }

    Exiv2::ExifData::const_iterator itr_date_time =
      exif_data.findKey(Exiv2::ExifKey("Exif.Image.DateTime"));
    if (itr_date_time != exif_data.end())
    {
      std::string date_time = itr_date_time->value().toString();
      date_time_item->setText(QString::fromLocal8Bit(date_time.c_str()));
    }

    Exiv2::ExifData::const_iterator itr_make =
      exif_data.findKey(Exiv2::ExifKey("Exif.Image.Make"));
    if (itr_make != exif_data.end())
    {
      std::string make = itr_make->value().toString();
      make_item->setText(QString::fromLocal8Bit(make.c_str()));
    }

    Exiv2::ExifData::const_iterator itr_model =
      exif_data.findKey(Exiv2::ExifKey("Exif.Image.Model"));
    if (itr_model != exif_data.end())
    {
      std::string model = itr_model->value().toString();
      model_item->setText(QString::fromLocal8Bit(model.c_str()));
    }

    Exiv2::ExifData::const_iterator itr_f_stop =
      exif_data.findKey(Exiv2::ExifKey("Exif.Photo.FNumber"));
    if (itr_f_stop != exif_data.end())
    {
      float f_stop = itr_f_stop->value().toFloat();
      f_stop_item->setText(QString::number(f_stop));
    }

    Exiv2::ExifData::const_iterator itr_iso =
      exif_data.findKey(Exiv2::ExifKey("Exif.Photo.ISOSpeedRatings"));
    if (itr_iso != exif_data.end())
    {
      std::string iso = itr_iso->value().toString();
      iso_item->setText(QString::fromLocal8Bit(iso.c_str()));
    }

    Exiv2::ExifData::const_iterator itr_shutter =
      exif_data.findKey(Exiv2::ExifKey("Exif.Photo.ExposureTime"));
    if (itr_shutter != exif_data.end())
    {
      std::string shutter = itr_shutter->value().toString();
      shutter_item->setText(QString::fromLocal8Bit(shutter.c_str()));
    }

    break;
  }

  return row;
}

void PhotoFilesSelectWidget::AddPhotos(const QStringList& file_names)
{
  auto itr_file_name = file_names.begin();
  auto itr_file_name_end = file_names.end();
  bool added = false;
  for (; itr_file_name != itr_file_name_end; ++itr_file_name)
  {
    //查找照片是否已存在
    bool is_exist = false;
    for (int i = 0; i < table_photos_->rowCount(); i++)
    {
      QTableWidgetItem* label_item = table_photos_->item(i, 0);
      QString file_name = label_item->data(Qt::UserRole).toString();
      if (file_name == *itr_file_name)
      {
        is_exist = true;
        break;
      }
    }
    if (is_exist) continue;

    QList<QTableWidgetItem*> row = GetRowFromPhoto(*itr_file_name);
    if (int(row.size()) != table_photos_->columnCount()) continue;

    int row_id = table_photos_->rowCount();
    table_photos_->insertRow(row_id);

    auto itr_item = row.begin();
    auto itr_item_end = row.end();
    for (int col_id = 0; itr_item != itr_item_end; ++itr_item, col_id++)
    {
      (*itr_item)->setFlags((*itr_item)->flags() ^ Qt::ItemIsEditable);
      table_photos_->setItem(row_id, col_id, *itr_item);
    }

    added = true;
  }
  if (added)
  {
    table_photos_->resizeColumnsToContents();
    emit PhotoFileEntriesChanged();
  }
}

}
}
}
