#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_IMPORT_CONFIGURE_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_IMPORT_CONFIGURE_DIALOG_HPP_

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "gui/photo_files_select_widget.hpp"
#include "gui/photogroup_info_pos_wrap_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotosImportConfigureDialog : public QDialog
{
  Q_OBJECT
public:
  enum ConfigureState
  {
    STATE_SELECT_PHOTO_FILES = 1,
    STATE_CONFIG_INFO_POS
  };
public:
  PhotosImportConfigureDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

private slots:
  void OnPushButtonPreviousClicked();
  void OnPushButtonNextClicked();
  void OnPhotoFileEntriesChanged();

private:
  QPushButton* push_button_previous_;
  QPushButton* push_button_next_;
  QVBoxLayout* layout_all_;
  QWidget* container_;
  QVBoxLayout* layout_container_;
  QHBoxLayout* layout_buttons_;

  int configure_state_;

  PhotoFilesSelectWidget* file_select_widget_;
  PhotogroupInfoPOSWrapWidget* photogroup_widget_;
};

}
}
}

#endif
