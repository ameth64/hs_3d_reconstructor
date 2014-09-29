#include "main_window.hpp"
#include "ui_main_window.h"

#include <QListWidget>

#include "manager_pane.hpp"
#include "blocks_displayer.hpp"

namespace hs
{
namespace recon
{

MainWindow::MainWindow()
  : QMainWindow(),
    ui_(new Ui::MainWindow)
{
  ui_->setupUi(this);

  QToolBar* tool_bar = new QToolBar(NULL);
  tool_bar->addAction(tr("TestToolBar"));
  ManagerPane* test_pane = new ManagerPane(tr("Test Manager Pane"), this);
  QListWidget* list_widget_0 = new QListWidget(test_pane);
  list_widget_0->addItems(QStringList()<<tr("Test Manager Pane")
                                       <<tr("This is list widget 0"));
  test_pane->AddWidget(list_widget_0);

  test_pane->SetToolBar(tool_bar);

  addDockWidget(Qt::RightDockWidgetArea, test_pane);
}

MainWindow::~MainWindow()
{

}

}
}
