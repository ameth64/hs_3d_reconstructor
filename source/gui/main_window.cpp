#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>

#include "gui/manager_pane.hpp"
#include "gui/new_project_config_dialog.hpp"
#include "gui/settings_dialog.hpp"

#include "gui/main_window.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

MainWindow::MainWindow()
  : QMainWindow()
{
  menu_bar_ = new QMenuBar(this);

  menu_file_ = new QMenu(tr("File"), menu_bar_);
  menu_bar_->addAction(menu_file_->menuAction());

  menu_tools_ = new QMenu(tr("Tools"), menu_bar_);
  menu_bar_->addAction(menu_tools_->menuAction());

  setMenuBar(menu_bar_);

  status_bar_ = new QStatusBar(this);
  setStatusBar(status_bar_);

  action_new_project_ = new QAction(tr("New Project"), this);
  action_open_project_ = new QAction(tr("Open Project"), this);
  action_close_project_ = new QAction(tr("Close Project"), this);
  menu_file_->addAction(action_new_project_);
  menu_file_->addAction(action_open_project_);
  menu_file_->addAction(action_close_project_);

  action_preferences_ = new QAction(tr("Preferences"), this);
  menu_tools_->addAction(action_preferences_);

  QObject::connect(action_new_project_, &QAction::triggered,
                   this, &MainWindow::OnActionNewProjectTriggered);
  QObject::connect(action_open_project_, &QAction::triggered,
                   this, &MainWindow::OnActionOpenProjectTriggered);
  QObject::connect(action_close_project_, &QAction::triggered,
    this, &MainWindow::OnActionCloseProjectTriggered);

  QObject::connect(action_preferences_, &QAction::triggered,
                   this, &MainWindow::OnActionPreferencesTriggered);

  DefaultSetting();

  QWidget* tmp_central_widget = new QWidget(this);
  setCentralWidget(tmp_central_widget);

  photos_pane_ = new PhotosPane(this);
  blocks_pane_ = new BlocksPane(this);
  gcps_pane_ = new GCPsPane(this);

  scene_window_ = new SceneWindow(0, database_mediator_);
  scene_window_->SetBackgroundColor(0.2f, 0.1f, 0.2f, 1.0f);
  QWidget* container = QWidget::createWindowContainer(scene_window_, this);
  setCentralWidget(container);
  resize(800,600);
  setDockNestingEnabled(true);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  addDockWidget(Qt::LeftDockWidgetArea, blocks_pane_);
  tabifyDockWidget(blocks_pane_, photos_pane_);
  addDockWidget(Qt::BottomDockWidgetArea, gcps_pane_);
  showMaximized();
  //加了下面两行则无论在windows还是osx平台上都最大化显示正常，不知道为什么……
  hide();
  show();

  database_mediator_.RegisterObserver(photos_pane_);
  database_mediator_.RegisterObserver(blocks_pane_);
  database_mediator_.RegisterObserver(gcps_pane_);
  database_mediator_.RegisterObserver(scene_window_);

  QObject::connect(blocks_pane_, &BlocksPane::PhotoOrientationActivated,
                   scene_window_, &SceneWindow::SetPhotoOrientation);
  QObject::connect(blocks_pane_, &BlocksPane::PointCloudActivated,
                    scene_window_, &SceneWindow::SetPointCloud);
  QObject::connect(blocks_pane_, &BlocksPane::PhotoOrientationActivated,
                   gcps_pane_, &GCPsPane::UpdatePhotoOrientation);
  QObject::connect(gcps_pane_, &GCPsPane::GCPRelateLocationState,
                   scene_window_->action_filter_photos_by_selected_points(),
                   &QAction::setDisabled);
  QObject::connect(scene_window_, &SceneWindow::FilterPhotosBySelectedPoints,
                   gcps_pane_, &GCPsPane::FilterPhotosByPoints);
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
  QSettings settings;
  QString default_projects_directory =
    settings.value(tr("default_projects_directory")).toString();
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

void MainWindow::DefaultSetting()
{
  QSettings settings;
  QString default_projects_directory_key =
    tr("default_projects_directory");
  if (!settings.contains(default_projects_directory_key))
  {
    settings.setValue(default_projects_directory_key,
                      QCoreApplication::applicationDirPath());
  }

  QString intermediate_directory_key = tr("intermediate_directory");
  if (!settings.contains(intermediate_directory_key))
  {
    QString intermediate_path =
      QCoreApplication::applicationDirPath() + tr("/intermediate");
    settings.setValue(intermediate_directory_key, intermediate_path);
  }

  QString number_of_threads_key = tr("number_of_threads");
  if (!settings.contains(number_of_threads_key))
  {
    settings.setValue(number_of_threads_key, uint(1));
  }
}

void MainWindow::OnActionPreferencesTriggered()
{
  SettingsDialog dialog;
  dialog.exec();
}

}
}
}
