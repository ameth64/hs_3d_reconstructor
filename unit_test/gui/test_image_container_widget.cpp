#include <QtTest/QtTest>
#include <QApplication>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/image_container_widget.hpp"

namespace test
{

class TestImageContainerWidget : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::ImageOpenGLWindow ImageOpenGLWindow;
  typedef ImageOpenGLWindow::ImageData ImageData;
  typedef hs::recon::ImageContainerWidget ImageContainerWidget;
private slots:
  void Test();
};

void TestImageContainerWidget::Test()
{
  hs::imgio::whole::ImageIO image_io;

  ImageOpenGLWindow* image_window1 = new ImageOpenGLWindow;
  ImageContainerWidget* image_container_widget1 =
    new ImageContainerWidget(image_window1);
  image_container_widget1->show();
  image_container_widget1->resize(600, 500);
  QCOMPARE(image_window1, image_container_widget1->image_window());

  ImageData image_data1;
  QCOMPARE(0, image_io.LoadImage("test_image_window_1.jpg", image_data1));
  image_window1->DisplayImage(image_data1);
  qApp->exec();
  image_container_widget1->show();
  QString comment = "test!";
  image_container_widget1->SetComment(comment);
  QCOMPARE(comment, image_container_widget1->comment());

  qApp->exec();
}

}

QTEST_MAIN(test::TestImageContainerWidget)
#include "test_image_container_widget.moc"
