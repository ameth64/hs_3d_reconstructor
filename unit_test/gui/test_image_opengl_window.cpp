#include <QtTest/QtTest>
#include <QApplication>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/image_opengl_window.hpp"

namespace test
{

class TestImageOpenGLWindow : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::ImageOpenGLWindow::ImageData ImageData;
private slots:
  void Test();
};

void TestImageOpenGLWindow::Test()
{
  hs::recon::ImageOpenGLWindow* image_window1 =
    new hs::recon::ImageOpenGLWindow;
  image_window1->resize(500, 375);
  image_window1->show();
  //hs::recon::ImageOpenGLWindow* image_window2 =
  //  new hs::recon::ImageOpenGLWindow;
  //image_window2->resize(400, 300);
  //image_window2->show();
  //hs::recon::ImageOpenGLWindow* image_window3 =
  //  new hs::recon::ImageOpenGLWindow;
  //image_window3->resize(300, 225);
  //image_window3->show();
  //hs::recon::ImageOpenGLWindow* image_window4 =
  //  new hs::recon::ImageOpenGLWindow;
  //image_window4->resize(200, 150);
  //image_window4->show();
  //hs::recon::ImageOpenGLWindow* image_window5 =
  //  new hs::recon::ImageOpenGLWindow;
  //image_window5->resize(100, 75);
  //image_window5->show();
  ImageData thumbnail_image_data1;
  ImageData thumbnail_image_data2;
  ImageData origin_image_data1;
  ImageData origin_image_data2;
  size_t width1 = 0;
  size_t height1 = 0;
  size_t width2 = 0;
  size_t height2 = 0;
  std::string thumbnail_path1 = "test_data/images/DSC00036_thumbnail_1.JPG";
  std::string origin_path1 = "test_data/images/DSC00036.JPG";
  std::string thumbnail_path2 = "test_data/images/DSC00037_thumbnail.JPG";
  std::string origin_path2 = "test_data/images/DSC00037.JPG";

  hs::imgio::whole::ImageIO image_io;

  //QCOMPARE(image_io.LoadImage(thumbnail_path1, thumbnail_image_data1), 0);
  QCOMPARE(image_io.GetImageDimension(origin_path1, width1, height1), 0);
  typedef ImageData::Byte Byte;
  thumbnail_image_data1.CreateImage(514, 514, 3);
  for (int row = 0; row < 514; row++)
  {
    for (int col = 0; col < 514; col++)
    {
      thumbnail_image_data1.GetByte(row, col, 0) = Byte(255);
      thumbnail_image_data1.GetByte(row, col, 1) = Byte(255);
      thumbnail_image_data1.GetByte(row, col, 2) = Byte(0);
    }
  }
  image_io.SaveImage("test_data/images/test.JPG", thumbnail_image_data1);

  image_window1->DisplayThumbnailImage(int(width1), int(height1),
                                       thumbnail_image_data1);
  //image_window2->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  //image_window3->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  //image_window4->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  //image_window5->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  qApp->exec();

  //QCOMPARE(image_io.LoadImage(origin_path1, origin_image_data1), 0);
  origin_image_data1.CreateImage(width1, height1, 3);
  for (int row = 0; row < height1; row++)
  {
    for (int col = 0; col < width1; col++)
    {
      origin_image_data1.GetByte(row, col, 0) = Byte(0);
      origin_image_data1.GetByte(row, col, 1) = Byte(0);
      origin_image_data1.GetByte(row, col, 2) = Byte(255);
    }
  }
  image_window1->show();
  //image_window2->show();
  //image_window3->show();
  //image_window4->show();
  //image_window5->show();
  image_window1->SetOriginImage(origin_image_data1);
  //image_window2->SetOriginImage(origin_image_data1);
  //image_window3->SetOriginImage(origin_image_data1);
  //image_window4->SetOriginImage(origin_image_data1);
  //image_window5->SetOriginImage(origin_image_data1);
  qApp->exec();

  //QCOMPARE(image_io.LoadImage(thumbnail_path2, thumbnail_image_data2), 0);
  //QCOMPARE(image_io.GetImageDimension(origin_path2, width2, height2), 0);
  //image_window1->show();
  //image_window2->show();
  //image_window3->show();
  //image_window4->show();
  //image_window5->show();
  //image_window1->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window2->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window3->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window4->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window5->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //qApp->exec();

  //QCOMPARE(image_io.LoadImage(origin_path2, origin_image_data2), 0);
  //image_window1->show();
  //image_window2->show();
  //image_window3->show();
  //image_window4->show();
  //image_window5->show();
  //image_window1->SetOriginImage(origin_image_data2);
  //image_window2->SetOriginImage(origin_image_data2);
  //image_window3->SetOriginImage(origin_image_data2);
  //image_window4->SetOriginImage(origin_image_data2);
  //image_window5->SetOriginImage(origin_image_data2);
  //qApp->exec();

}

}

QTEST_MAIN(test::TestImageOpenGLWindow)
#include "test_image_opengl_window.moc"
