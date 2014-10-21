#include <QApplication>
#include "main_window.hpp"

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  hs::recon::MainWindow main_window;
  main_window.show();
  return application.exec();
}
