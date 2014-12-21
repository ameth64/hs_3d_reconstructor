#include <QtTest/QtTest>
#include <QApplication>

#include "gui/photogroup_pos_configure_widget.hpp"

namespace test
{

class TestPhotogroupPOSConfigureWidget : public QObject
{
  Q_OBJECT
private slots:
  void Test();
};

void TestPhotogroupPOSConfigureWidget::Test()
{
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
  hs::recon::gui::PhotogroupPOSConfigureWidget widget(photo_paths);
  widget.show();
  qApp->exec();
}

}

QTEST_MAIN(test::TestPhotogroupPOSConfigureWidget);
#include "test_photogroup_pos_configure_widget.moc"
