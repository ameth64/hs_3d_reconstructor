#include <QApplication>
#include <QTranslator>
#include "gui/main_window.hpp"

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  QTranslator* translator = new QTranslator;
  translator->load("zh_cn.qm");
  application.installTranslator(translator);
  //TODO:此处应加入国际化支持
  QCoreApplication::setOrganizationName("Huishi Technology");
  QCoreApplication::setApplicationName("Huishi 3D Reconstructor");
  hs::recon::gui::MainWindow main_window;
  return application.exec();
}
