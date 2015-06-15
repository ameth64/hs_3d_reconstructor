#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTO_IMPORT_CHECK_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTO_IMPORT_CHECK_DIALOG_HPP_

#include "gui/photo_import_check_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{
  class PhotoImportCheckDialog: public QDialog
  {
    Q_OBJECT
  public:
    PhotoImportCheckDialog(std::map<int, std::string>& photo_id_path);
    ~PhotoImportCheckDialog();

    const std::map<int, std::string>& PhotoIDPaths() const;

  private:
    QVBoxLayout* main_v_layout_;
    QLabel* label_inform_;
    QPushButton* push_button_ok_;
    QPushButton* push_button_cancel_;
    PhotoImportCheckWidget* photo_import_check_widget_;

  };

}
}
}

#endif //_HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_