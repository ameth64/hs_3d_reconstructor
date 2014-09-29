#ifndef _HS_3D_RECONSTRUCTOR_MAIN_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_MAIN_WINDOW_HPP_

#include <QMainWindow>

#include "main_window.hpp"

namespace Ui
{
class MainWindow;
}

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
  Ui::MainWindow* ui_;
};

}
}

#endif
