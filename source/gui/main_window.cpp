#include "main_window.hpp"
#include "ui_main_window.h"

#include <QListWidget>

#include "manager_pane.hpp"

namespace hs
{
namespace recon
{

MainWindow::MainWindow()
  : QMainWindow(),
    ui_(new Ui::MainWindow)
{
  ui_->setupUi(this);

}

MainWindow::~MainWindow()
{

}

}
}
