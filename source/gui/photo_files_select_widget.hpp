#ifndef _HS_3D_RECONSTRUTOR_GUI_PHOTO_FILES_SELECT_WIDGET_HPP_
#define _HS_3D_RECONSTRUTOR_GUI_PHOTO_FILES_SELECT_WIDGET_HPP_

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QIcon>

namespace hs
{
namespace recon
{
namespace gui
{

struct PhotoFileEntry
{
  PhotoFileEntry();
  int width;
  int height;
  float focal_length;
  float pixel_x_size;
  float pixel_y_size;

  bool operator == (const PhotoFileEntry& other) const;
  bool operator != (const PhotoFileEntry& other) const;
};

class PhotoFilesSelectWidget : public QWidget
{
  Q_OBJECT
public:
  typedef std::vector<PhotoFileEntry> PhotoFileEntryContainer;
public:
  PhotoFilesSelectWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
  PhotoFileEntryContainer GetPhotoFileEnties();
  QStringList GetPhotoFilePaths();

signals:
  void PhotoFileEntriesChanged();

private slots:
  void OnAddPhotosClicked();
  void OnAddDirectoryClicked();
  void OnRemovePhotosClicked();
  void OnTablePhotoItemSelectionChanged();

private:
  QList<QTableWidgetItem*> GetRowFromPhoto(const QString& photo_path);
  void AddPhotos(const QStringList& file_names);

private:
  QHBoxLayout* layout_buttons_;
  QVBoxLayout* layout_all_;
  QTableWidget* table_photos_;
  QPushButton* push_button_add_photos_;
  QPushButton* push_button_add_directory_;
  QPushButton* push_button_remove_photos_;

  QIcon photo_icon_;
};

}
}
}

#endif
