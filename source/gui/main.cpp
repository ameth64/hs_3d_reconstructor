#include <QApplication>
#include <QTranslator>
#include "gui/main_window.hpp"

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  QTranslator* translator = new QTranslator;
  translator->load("hs_3d_reconstructor_zh_cn.qm");
  application.installTranslator(translator);

  QTranslator* translator_car = new QTranslator;
  translator_car->load("cartographics_qt_zh_cn.qm");
  application.installTranslator(translator_car);
  //TODO:此处应加入国际化支持
  QCoreApplication::setOrganizationName("Huishi Technology");
  QCoreApplication::setApplicationName("Huishi 3D Reconstructor");
  hs::recon::gui::MainWindow main_window;
  return application.exec();
}
