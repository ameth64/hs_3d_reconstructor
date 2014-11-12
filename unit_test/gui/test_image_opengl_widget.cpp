#include <QtTest/QtTest>
#include <QApplication>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/image_opengl_widget.hpp"

namespace test
{

class TestImageOpenGLWidget : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::ImageOpenGLWidget::ImageData ImageData;
private slots:
  void Test();
};

void TestImageOpenGLWidget::Test()
{
  hs::recon::ImageOpenGLWidget* image_widget1 =
    hs::recon::ImageOpenGLWidget::Create();
  image_widget1->resize(500, 375);
  image_widget1->show();
  hs::recon::ImageOpenGLWidget* image_widget2 =
    hs::recon::ImageOpenGLWidget::Create();
  image_widget2->resize(400, 300);
  image_widget2->show();
  hs::recon::ImageOpenGLWidget* image_widget3 =
    hs::recon::ImageOpenGLWidget::Create();
  image_widget3->resize(300, 225);
  image_widget3->show();
  hs::recon::ImageOpenGLWidget* image_widget4 =
    hs::recon::ImageOpenGLWidget::Create();
  image_widget4->resize(200, 150);
  image_widget4->show();
  hs::recon::ImageOpenGLWidget* image_widget5 =
    hs::recon::ImageOpenGLWidget::Create();
  image_widget5->resize(100, 75);
  image_widget5->show();

  ImageData image_data1;
  ImageData image_data2;

  hs::imgio::whole::ImageIO image_io;
  QCOMPARE(0, image_io.LoadImage("test_image_window_1.jpg", image_data1));

  image_widget1->DisplayImage(image_data1);
  image_widget2->DisplayImage(image_data1);
  image_widget3->DisplayImage(image_data1);
  image_widget4->DisplayImage(image_data1);
  image_widget5->DisplayImage(image_data1);

  qApp->exec();

  image_widget1->show();
  image_widget2->show();
  image_widget3->show();
  image_widget4->show();
  image_widget5->show();
  QCOMPARE(0, image_io.LoadImage("test_image_window_2.jpg", image_data2));
  image_widget1->DisplayImage(image_data2);
  image_widget2->DisplayImage(image_data2);
  image_widget3->DisplayImage(image_data2);
  image_widget4->DisplayImage(image_data2);
  image_widget5->DisplayImage(image_data2);
  image_widget3->DisplayImage(image_data2);
  qApp->exec();
}

}

QTEST_MAIN(test::TestImageOpenGLWidget)
#include "test_image_opengl_widget.moc"
