#include <QtTest/QtTest>
#include <QApplication>

#include "gui/photos_import_configure_dialog.hpp"

namespace test
{

class TestPhotosImportConfigureDialog : public QObject
{
  Q_OBJECT
private slots:
  void Test();
};

void TestPhotosImportConfigureDialog::Test()
{
  hs::recon::gui::PhotosImportConfigureDialog dialog;
  dialog.show();
  qApp->exec();
}

}

QTEST_MAIN(test::TestPhotosImportConfigureDialog);
#include <test_photos_import_configure_dialog.moc>
