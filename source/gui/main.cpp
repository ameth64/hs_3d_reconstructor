#include <QApplication>
#include <QDesktopWidget>
#include "gui/main_window.hpp"

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  QDesktopWidget* desk = QApplication::desktop();
  //TODO:此处应加入国际化支持
  QCoreApplication::setOrganizationName("Huishi Technology");
  QCoreApplication::setApplicationName("Huishi 3D Reconstructor");
  hs::recon::gui::MainWindow main_window;
  main_window.resize(desk->availableGeometry().size());
  main_window.showMaximized();
  return application.exec();
}
