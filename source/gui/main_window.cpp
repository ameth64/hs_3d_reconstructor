#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

#include "gui/manager_pane.hpp"
#include "gui/new_project_config_dialog.hpp"
#include "gui/settings_dialog.hpp"

#include "gui/main_window.hpp"

#include "gui/start_up_dialog.hpp"
#include "gui/photo_import_check_dialog.hpp"

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

  menu_view_ = new QMenu(tr("View"), menu_bar_);
  menu_bar_->addAction(menu_view_->menuAction());

  menu_tools_ = new QMenu(tr("Tools"), menu_bar_);
  menu_bar_->addAction(menu_tools_->menuAction());

  setMenuBar(menu_bar_);

  status_bar_ = new QStatusBar(this);

  start_up_dialog_ = new StartUpDialog(this);

  setStatusBar(status_bar_);

  action_new_project_ = new QAction(tr("New Project"), this);
  action_open_project_ = new QAction(tr("Open Project"), this);
  action_close_project_ = new QAction(tr("Close Project"), this);
  menu_file_->addAction(action_new_project_);
  menu_file_->addAction(action_open_project_);
  menu_file_->addAction(action_close_project_);

	action_photos_pane_ = new QAction(tr("Photos Pane"),this);
	action_blocks_pane_ = new QAction(tr("Blocks Pane"),this);
	action_gcps_pane_ = new QAction(tr("GCPs Pane"),this);
	menu_view_->addAction(action_photos_pane_);
	menu_view_->addAction(action_blocks_pane_);
	menu_view_->addAction(action_gcps_pane_);

  action_preferences_ = new QAction(tr("Preferences"), this);
  menu_tools_->addAction(action_preferences_);

  QObject::connect(action_new_project_, &QAction::triggered,
                   this, &MainWindow::OnActionNewProjectTriggered);
  QObject::connect(action_open_project_, &QAction::triggered,
                   this, &MainWindow::OnActionOpenProjectTriggered);
  QObject::connect(action_close_project_, &QAction::triggered,
                   this, &MainWindow::OnActionCloseProjectTriggered);

  QObject::connect(action_photos_pane_, &QAction::triggered,
                   this, &MainWindow::OnActionPhotosPaneTriggered);

  QObject::connect(action_blocks_pane_, &QAction::triggered,
                   this, &MainWindow::OnActionBlocksPaneTriggered);

  QObject::connect(action_gcps_pane_, &QAction::triggered,
                   this, &MainWindow::OnActionGCPsPaneTriggered);

  QObject::connect(action_preferences_, &QAction::triggered,
                   this, &MainWindow::OnActionPreferencesTriggered);

  DefaultSetting();

  //QWidget* tmp_central_widget = new QWidget(this);
  //setCentralWidget(tmp_central_widget);

  QSize wh = size();
  photos_pane_ = new PhotosPane(this);
  blocks_pane_ = new BlocksPane(this);
  gcps_pane_ = new GCPsPane(this);

  scene_window_ = new SceneWindow(0, database_mediator_);
  scene_window_->SetBackgroundColor(0.2f, 0.1f, 0.2f, 1.0f);
  QWidget* container = QWidget::createWindowContainer(scene_window_, this);
  setCentralWidget(container);
  //resize(800,600);
  setDockNestingEnabled(true);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  addDockWidget(Qt::LeftDockWidgetArea, blocks_pane_);
  tabifyDockWidget(blocks_pane_, photos_pane_);
  addDockWidget(Qt::BottomDockWidgetArea, gcps_pane_);
  //showMaximized();
  //加了下面两行则无论在windows还是osx平台上都最大化显示正常，不知道为什么……
  //hide();
  //show();

  database_mediator_.RegisterObserver(photos_pane_);
  database_mediator_.RegisterObserver(blocks_pane_);
  database_mediator_.RegisterObserver(gcps_pane_);
  database_mediator_.RegisterObserver(scene_window_);

  QObject::connect(blocks_pane_, &BlocksPane::PhotoOrientationActivated,
                   scene_window_, &SceneWindow::SetPhotoOrientation);
  QObject::connect(blocks_pane_, &BlocksPane::PointCloudActivated,
                    scene_window_, &SceneWindow::SetPointCloud);
  QObject::connect(blocks_pane_, &BlocksPane::SurfaceModelActivated,
                    scene_window_, &SceneWindow::SetSurfaceModel);
  QObject::connect(blocks_pane_, &BlocksPane::PhotoOrientationActivated,
                   gcps_pane_, &GCPsPane::UpdatePhotoOrientation);
  QObject::connect(gcps_pane_, &GCPsPane::GCPRelateLocationState,
                   scene_window_->action_filter_photos_by_selected_points(),
                   &QAction::setDisabled);
  QObject::connect(scene_window_, &SceneWindow::FilterPhotosBySelectedPoints,
                   gcps_pane_, &GCPsPane::FilterPhotosByPoints);

  QObject::connect(start_up_dialog_, &StartUpDialog::NewProjcet,
    this, &MainWindow::OnActionNewProjectTriggered);
  QObject::connect(start_up_dialog_, &StartUpDialog::OpenProject,
    this, &MainWindow::OnActionOpenProjectTriggered);
  QObject::connect(start_up_dialog_, &StartUpDialog::OpenProjectWithFile,
    this, &MainWindow::OpenProject);
  QObject::connect(start_up_dialog_, &StartUpDialog::OpenTools,
    this, &MainWindow::OnActionPreferencesTriggered);

  QDesktopWidget* desk = QApplication::desktop();
  resize(desk->availableGeometry().size());
  showMaximized();

  start_up_dialog_->resize(800,200);
  start_up_dialog_->exec();
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
    settings.value("default_projects_directory").toString();
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
        msg_box.setText(tr("Project    exist!"));
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
    case hs::recon::db::Database::DATABASE_NO_ERROR:
     {
      QString db_file_path
        = QString::fromStdString(std_new_project_directory) + tr("/")
        + QFileInfo(QString::fromStdString(std_new_project_directory)).fileName()
        + tr(".3db");
      start_up_dialog_->SetCurrentFile(db_file_path);
      start_up_dialog_->close();
     }
    }
  }
}

void MainWindow::OnActionOpenProjectTriggered()
{
  QSettings settings;
  QString default_projects_directory =
    settings.value("default_projects_directory").toString();
  QFileDialog dialog;
  dialog.setDirectory(default_projects_directory);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("3D Reconstructor Database File (*.3db)"));
  if (dialog.exec())
  {
    QString database_file = dialog.selectedFiles()[0];
    OpenProject(database_file);
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
  start_up_dialog_->resize(800,200);
  start_up_dialog_->exec();
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

  QString intermediate_directory_key = QString("intermediate_directory");
  if (!settings.contains(intermediate_directory_key))
  {
    QString intermediate_path =
      QCoreApplication::applicationDirPath() + QString("/intermediate");
    settings.setValue(intermediate_directory_key, intermediate_path);
  }

  QString number_of_threads_key = QString("number_of_threads");
  if (!settings.contains(number_of_threads_key))
  {
    settings.setValue(number_of_threads_key, uint(1));
  }
}

void MainWindow::OnActionPhotosPaneTriggered()
{
	if(photos_pane_->isHidden())
		photos_pane_->show();
	photos_pane_->raise();
}
void MainWindow::OnActionBlocksPaneTriggered()
{
	if(blocks_pane_->isHidden())
		blocks_pane_->show();
	blocks_pane_->raise();
}
void MainWindow::OnActionGCPsPaneTriggered()
{
	if(gcps_pane_->isHidden())
		gcps_pane_->show();
	gcps_pane_->raise();
}

void MainWindow::OnActionPreferencesTriggered()
{
  SettingsDialog dialog;
  dialog.exec();
}

void MainWindow::OpenProject(const QString& database_file)
{
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
    case hs::recon::db::Database::ERROR_PHOTO_PATH_NEEDED_MODIFY:
    {

      PhotoImportCheckDialog* photo_import_check_dialog = 
        new PhotoImportCheckDialog(response.error_photo_id_paths);

      if(photo_import_check_dialog->exec() == QDialog::Accepted)
      {
        hs::recon::db::RequestUpdatePhotoPaths request;
        hs::recon::db::ResponseUpdatePhotoPaths response;
        request.photo_id_paths = photo_import_check_dialog->PhotoIDPaths();
        database_mediator_.Request(
          nullptr, hs::recon::db::DatabaseMediator::REQUEST_UPDATE_PHOTO_PATH,
          request, response, false);
      }
      start_up_dialog_->SetCurrentFile(database_file);
      start_up_dialog_->close();
      break;
    }
    case hs::recon::db::Database::DATABASE_NO_ERROR:
    {
      start_up_dialog_->SetCurrentFile(database_file);
      start_up_dialog_->close();
    }
  }
}

}//namespace gui
}
}
