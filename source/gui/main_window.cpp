#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>

#include "gui/main_window.hpp"

#include "gui/manager_pane.hpp"
#include "gui/new_project_config_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

MainWindow::MainWindow()
  : QMainWindow()
  , settings_()
{
  menu_bar_ = new QMenuBar(this);
  menu_file_ = new QMenu(menu_bar_);
  menu_file_->setTitle(tr("File"));
  menu_bar_->addAction(menu_file_->menuAction());
  setMenuBar(menu_bar_);
  status_bar_ = new QStatusBar(this);
  setStatusBar(status_bar_);

  action_new_project_ = new QAction(this);
  action_new_project_->setText(tr("New Project"));
  action_open_project_ = new QAction(this);
  action_open_project_->setText(tr("Open Project"));
  action_close_project_ = new QAction(this);
  action_close_project_->setText(tr("Close Project"));

  menu_file_->addAction(action_new_project_);
  menu_file_->addAction(action_open_project_);
  menu_file_->addAction(action_close_project_);

  QObject::connect(action_new_project_, &QAction::triggered,
                   this, &MainWindow::OnActionNewProjectTriggered);
  QObject::connect(action_open_project_, &QAction::triggered,
                   this, &MainWindow::OnActionOpenProjectTriggered);
  QObject::connect(action_close_project_, &QAction::triggered,
                   this, &MainWindow::OnActionCloseProjectTriggered);

  settings_.setValue(tr("default_projects_directory"),
                     QCoreApplication::applicationDirPath());

  QWidget* tmp_central_widget = new QWidget(this);
  setCentralWidget(tmp_central_widget);

  photos_pane_ = new PhotosPane(this);
  //blocks_pane_ = new BlocksPane(this);

  addDockWidget(Qt::LeftDockWidgetArea, photos_pane_);
  //addDockWidget(Qt::LeftDockWidgetArea, blocks_pane_);


  database_mediator_.RegisterObserver(photos_pane_);

  showMaximized();
}

MainWindow::~MainWindow()
{
}

hs::recon::db::DatabaseMediator& MainWindow::database_mediator()
{
  return database_mediator_;
}

void MainWindow::OnActionNewProjectTriggered()
{
  QString default_projects_directory =
    settings_.value(tr("default_projects_directory")).toString();
  NewProjectConfigDialog dialog(default_projects_directory, this);
  if (dialog.exec())
  {
    std::string std_new_project_directory =
      dialog.new_project_directory().toLocal8Bit().data();
    QMessageBox msg_box;
    hs::recon::db::RequestCreateDatabase request;
    hs::recon::db::ResponseCreateDatabase response;
    request.database_directory = std_new_project_directory;
    switch(database_mediator_.Request(
        nullptr, hs::recon::db::DatabaseMediator::REQUEST_CREATE_DATABASE,
        request, response, true))
    {
    case hs::recon::db::Database::ERROR_DATABASE_DIRECTORY_EXIST:
      {
        msg_box.setText(tr("Project directory exist!"));
        msg_box.exec();
        break;
      }
    case hs::recon::db::Database::ERROR_CANT_CREATE_DATABASE_DIRECTORY:
      {
        msg_box.setText(tr("Project directory can't be created!"));
        msg_box.exec();
        break;
      }
    case hs::recon::db::Database::ERROR_FAIL_TO_OPEN_SQLITE_DATABASE:
      {
        msg_box.setText(tr("Fail to open database!"));
        msg_box.exec();
        break;
      }
    }
  }
}

void MainWindow::OnActionOpenProjectTriggered()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("3D Reconstructor Database File (*.3db)"));
  if (dialog.exec())
  {
    QString database_file = dialog.selectedFiles()[0];
    std::string std_database_file = database_file.toLocal8Bit().data();
    QMessageBox msg_box;
    hs::recon::db::RequestOpenDatabase request;
    hs::recon::db::ResponseOpenDatabase response;
    request.database_file = std_database_file;
    switch (database_mediator_.Request(
        nullptr, hs::recon::db::DatabaseMediator::REQUEST_OPEN_DATABASE,
        request, response, true))
    {
    case hs::recon::db::Database::ERROR_FAIL_TO_OPEN_SQLITE_DATABASE:
      {
        msg_box.setText(tr("Fail to open database!"));
        msg_box.exec();
        break;
      }
    case hs::recon::db::Database::ERROR_DATABASE_FILE_NOT_EXIST:
      {
        msg_box.setText(tr("Database file not exist!"));
        msg_box.exec();
        break;
      }
    }
  }
}

void MainWindow::OnActionCloseProjectTriggered()
{
  hs::recon::db::RequestCloseDatabase request;
  hs::recon::db::ResponseCloseDatabase response;
  if (database_mediator_.Request(
      nullptr, hs::recon::db::DatabaseMediator::REQUEST_CLOSE_DATABASE,
      request, response, true) ==
      hs::recon::db::Database::ERROR_FAIL_TO_CLOSE_SQLITE_DATABASE)
  {
    QMessageBox msg_box;
    msg_box.setText(tr("Fail to close database!"));
    msg_box.exec();
  }
}

}
}
}
