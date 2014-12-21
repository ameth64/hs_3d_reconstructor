#include <QtTest/QtTest>
#include <QApplication>

#include "gui/photogroup_info_widget.hpp"

namespace test
{

class TestPhotogroupInfoWidget : public QObject
{
  Q_OBJECT
private slots:
  void Test();
};

void TestPhotogroupInfoWidget::Test()
{
  hs::recon::gui::PhotogroupInfo info;
  info.name = tr("Test");
  info.description = tr("Test Photogroup Info\nTest!\n");
  info.width = 5472;
  info.height = 3648;
  info.focal_length = 10.4;
  info.pixel_size_x = 0.00244361;
  info.pixel_size_y = 0.00244361;
  info.principal_x = 2680.71;
  info.principal_y = 1874.35;
  info.skew = 0.0;
  info.radial1 = -0.0270415;
  info.radial2 = -0.00278797;
  info.radial3 = -0.00285158;
  info.decentering1 = 0.00096984;
  info.decentering2 = -0.00132662;

  hs::recon::gui::PhotogroupInfoWidget widget(info);
  widget.show();
  qApp->exec();
}

}

QTEST_MAIN(test::TestPhotogroupInfoWidget);
#include "test_photogroup_info_widget.moc"
