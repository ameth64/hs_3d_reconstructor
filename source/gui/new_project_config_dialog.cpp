#include "gui/new_project_config_dialog.hpp"

#include <string>

#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QHideEvent>

#include <boost/filesystem.hpp>

namespace hs
{
namespace recon
{
namespace gui
{

NewProjectConfigDialog::NewProjectConfigDialog(
  const QString& default_project_directory, QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , default_project_directory_()
  , label_project_name_(new QLabel(tr("Project Name:"), this))
  , line_edit_project_name_(new QLineEdit(this))
  , layout_project_name_(new QHBoxLayout)
  , label_directory_(new QLabel(tr("Create in:"), this))
  , line_edit_directory_(new QLineEdit(this))
  , push_button_browse_(new QPushButton(tr("Browse"), this))
  , layout_directory_(new QHBoxLayout)
  , layout_all_(new QVBoxLayout(this))
  , dialog_button_box_ok_cancel_(new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel,
                                                      this))
{
  layout_all_->addLayout(layout_project_name_);
  layout_all_->addLayout(layout_directory_);
  layout_all_->addWidget(dialog_button_box_ok_cancel_);

  layout_project_name_->addWidget(label_project_name_);
  layout_project_name_->addWidget(line_edit_project_name_);

  layout_directory_->addWidget(label_directory_);
  layout_directory_->addWidget(line_edit_directory_);
  layout_directory_->addWidget(push_button_browse_);

  setLayout(layout_all_);

  setWindowTitle(tr("New Project Configuration"));

  QObject::connect(dialog_button_box_ok_cancel_, &QDialogButtonBox::accepted,
                   this, &NewProjectConfigDialog::OnAccept);
  QObject::connect(dialog_button_box_ok_cancel_, &QDialogButtonBox::rejected,
                   this, &QDialog::reject);
  QObject::connect(push_button_browse_, &QAbstractButton::clicked,
                   this, &NewProjectConfigDialog::OnBrowseButtonClicked);

  resize(600, 200);

  line_edit_directory_->setText(default_project_directory);
  line_edit_directory_->setReadOnly(true);
}

QString NewProjectConfigDialog::new_project_directory() const
{
  return line_edit_directory_->text() + '/' + line_edit_project_name_->text();
}

int NewProjectConfigDialog::ValidateProjectDirectory()
{
  //验证Project Name是否有效
  std::string std_project_name =
    line_edit_project_name_->text().toLocal8Bit().data();
  if (!boost::filesystem::portable_file_name(std_project_name))
  {
    return IC_INVALID_NAME;
  }
  //验证Project Directory是否存在
  std::string std_project_directory =
    new_project_directory().toLocal8Bit().data();
  if (boost::filesystem::exists(boost::filesystem::path(std_project_name)))
  {
    return IC_PROJECT_DIRECTORY_EXIST;
  }

  return IC_VALID;
}

void NewProjectConfigDialog::OnBrowseButtonClicked()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::Directory);
  if (dialog.exec())
  {
    line_edit_directory_->setText(dialog.selectedFiles()[0]);
  }
}

void NewProjectConfigDialog::OnAccept()
{
  QMessageBox msg_box;
  switch(ValidateProjectDirectory())
  {
  case IC_INVALID_NAME:
    {
      msg_box.setText(tr("Invalid project name!"));
      msg_box.exec();
      break;
    }
  case IC_PROJECT_DIRECTORY_EXIST:
    {
      msg_box.setText(tr("New project directory exists!"));
      msg_box.exec();
      break;
    }
  case IC_VALID:
    {
      accept();
      break;
    }
  }
}

}
}
}
