#include "main_window.hpp"

#include "manager_pane.hpp"

namespace hs
{
namespace recon
{

MainWindow::MainWindow()
  : QMainWindow()
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

  widget_ = new QWidget(this);
  setCentralWidget(widget_);

  showMaximized();
}

MainWindow::~MainWindow()
{

}

}
}
