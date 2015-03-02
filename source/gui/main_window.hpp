#ifndef _HS_3D_RECONSTRUCTOR_MAIN_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_MAIN_WINDOW_HPP_

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QSettings>

#include "database/database_mediator.hpp"

#include "gui/scene_window.hpp"
#include "gui/photos_pane.hpp"
#include "gui/blocks_pane.hpp"
#include "gui/gcps_pane.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow();
  ~MainWindow();

public:
  hs::recon::db::DatabaseMediator& database_mediator();

private slots:
  void OnActionNewProjectTriggered();
  void OnActionOpenProjectTriggered();
  void OnActionCloseProjectTriggered();

private:
  QMenuBar* menu_bar_;
  QMenu* menu_file_;
  QStatusBar* status_bar_;

  QAction* action_new_project_;
  QAction* action_open_project_;
  QAction* action_close_project_;

  hs::recon::db::DatabaseMediator database_mediator_;

  QSettings settings_;

  PhotosPane* photos_pane_;
  BlocksPane* blocks_pane_;
  GCPsPane* gcps_pane_;
  SceneWindow* scene_window_;
};

}
}
}

#endif
