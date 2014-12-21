#include <QtTest/QtTest>
#include <QApplication>

#include "gui/photogroup_info_pos_wrap_widget.hpp"

namespace test
{

class TestPhotogroupInfoPOSWrapWidget : public QObject
{
  Q_OBJECT
private slots:
  void Test();
};

void TestPhotogroupInfoPOSWrapWidget::Test()
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

  QStringList photo_paths;
  photo_paths << "./test_data/images/DSC00036.JPG"
              << "./test_data/images/DSC00037.JPG"
              << "./test_data/images/DSC00123.JPG"
              << "./test_data/images/DSC00125.JPG"
              << "./test_data/images/DSC00291.JPG"
              << "./test_data/images/DSC09887.JPG"
              << "./test_data/images/DSC09889.JPG"
              << "./test_data/images/DSC09891.JPG"
              << "./test_data/images/DSC09893.JPG"
              << "./test_data/images/DSC09981.JPG"
              << "./test_data/images/DSC09985.JPG";

  hs::recon::gui::PhotogroupInfoPOSWrapWidget widget(info, photo_paths);
  widget.show();
  qApp->exec();
}

}

QTEST_MAIN(test::TestPhotogroupInfoPOSWrapWidget);
#include "test_photogroup_info_pos_wrap_widget.moc"
