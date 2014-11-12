#ifndef _HS_3D_RECONSTRUCTOR_MAIN_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_MAIN_WINDOW_HPP_

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>

namespace hs
{
namespace recon
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow();
  ~MainWindow();

private:
  QMenuBar* menu_bar_;
  QMenu* menu_file_;
  QStatusBar* status_bar_;

  QAction* action_new_project_;
  QAction* action_open_project_;
  QAction* action_close_project_;

  QWidget* widget_;
};

}
}

#endif
