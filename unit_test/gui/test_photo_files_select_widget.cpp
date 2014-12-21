#include <QtTest/QtTest>
#include <QApplication>

#include "gui/photo_files_select_widget.hpp"

namespace test
{

class TestPhotoFilesSelectWidget : public QObject
{
  Q_OBJECT
private slots:
  void Test();
};

void TestPhotoFilesSelectWidget::Test()
{
  hs::recon::gui::PhotoFilesSelectWidget widget;
  widget.show();
  qApp->exec();
}

}

QTEST_MAIN(test::TestPhotoFilesSelectWidget);
#include "test_photo_files_select_widget.moc"
