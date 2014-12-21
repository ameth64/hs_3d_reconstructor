#ifndef _HS_3D_RECONSTRUCTOR_GUI_NEW_PROJECT_CONFIG_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_NEW_PROJECT_CONFIG_DIALOG_HPP_

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

namespace hs
{
namespace recon
{
namespace gui
{

class NewProjectConfigDialog : public QDialog
{
  Q_OBJECT
public:
  enum InvalidCode
  {
    IC_VALID = 0,
    IC_INVALID_NAME,
    IC_PROJECT_DIRECTORY_EXIST
  };
public:
  NewProjectConfigDialog(const QString& default_project_directory,
                         QWidget* parent = 0, Qt::WindowFlags f = 0);

  QString new_project_directory() const;

private slots:
  void OnBrowseButtonClicked();
  void OnAccept();

private:
  int ValidateProjectDirectory();

private:
  QLabel* label_project_name_;
  QLineEdit* line_edit_project_name_;
  QHBoxLayout* layout_project_name_;
  QLabel* label_directory_;
  QLineEdit* line_edit_directory_;
  QPushButton* push_button_browse_;
  QHBoxLayout* layout_directory_;
  QVBoxLayout* layout_all_;
  QDialogButtonBox* dialog_button_box_ok_cancel_;

  QString default_project_directory_;
};

}
}
}

#endif
