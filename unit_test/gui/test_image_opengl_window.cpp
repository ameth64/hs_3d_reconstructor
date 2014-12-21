#include <QtTest/QtTest>
#include <QApplication>
#include <QImage>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/image_opengl_window.hpp"

namespace test
{

class TestImageOpenGLWindow : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::ImageOpenGLWindow::Float Float;
  typedef hs::recon::ImageOpenGLWindow::Position Position;
  typedef hs::recon::ImageOpenGLWindow::PositionContainer PositionContainer;
  typedef hs::recon::ImageOpenGLWindow::ImageData ImageData;
  typedef ImageData::Byte Byte;
private:
  static ImageData LoadIcon(const QString& icon_path);
private slots:
  void Test();
};

TestImageOpenGLWindow::ImageData TestImageOpenGLWindow::LoadIcon(
  const QString& icon_path)
{
  QImage icon(icon_path);
  int icon_width = icon.width();
  int icon_height = icon.height();
  int icon_channel = icon.depth() / 8;
  ImageData icon_image_data;
  icon_image_data.CreateImage(icon_width, icon_height, icon_channel);
  for (int row = 0; row < icon_height; row++)
  {
    for (int col = 0; col < icon_width; col++)
    {
      QRgb pixel = icon.pixel(col, row);
      icon_image_data.GetByte(row, col, 0) = Byte(qRed(pixel));
      icon_image_data.GetByte(row, col, 1) = Byte(qGreen(pixel));
      icon_image_data.GetByte(row, col, 2) = Byte(qBlue(pixel));
      if (icon_channel == 4)
      {
        icon_image_data.GetByte(row, col, 3) = Byte(qAlpha(pixel));
      }
    }
  }

#if 1
  hs::imgio::whole::ImageIO image_io;
  //ImageData test_icon_image_data;
  //image_io.LoadImage("/Users/limbo/workspace/hs_dev/executable/hs_3d_reconstructor/resource/images/icon_tiepoint.png", test_icon_image_data);
  //image_io.SaveImage("test_icon_1.tif", test_icon_image_data);
  image_io.SaveImage("test_icon.tif", icon_image_data);
#endif

  return icon_image_data;
}

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
  std::string thumbnail_path1 = "test_data/images/DSC00036_thumbnail.JPG";
  std::string origin_path1 = "test_data/images/DSC00036.JPG";
  std::string thumbnail_path2 = "test_data/images/DSC00037_thumbnail.JPG";
  std::string origin_path2 = "test_data/images/DSC00037.JPG";

  hs::imgio::whole::ImageIO image_io;

  QCOMPARE(image_io.LoadImage(thumbnail_path1, thumbnail_image_data1), 0);
  QCOMPARE(image_io.GetImageDimension(origin_path1, width1, height1), 0);

  image_window1->DisplayThumbnailImage(int(width1), int(height1),
                                       thumbnail_image_data1);
  PositionContainer positions;
  //positions.push_back(Position(Float(width1) * Float(0.5),
  //                             Float(height1) * Float(0.5)));
  //positions.push_back(Position(Float(0), Float(0)));
  positions.push_back(Position(Float(0), Float(height1 / 2)));
  //positions.push_back(Position(Float(width1 - 1), Float(0)));
  //positions.push_back(Position(Float(width1 - 1), Float(height1 - 1)));
  ImageData icon_gcp = LoadIcon(QString(":/images/icon_gcp.png"));
  image_window1->SetIcon(icon_gcp, 0, icon_gcp.height() / 2);
  image_window1->SetPositions(positions);
  //image_window2->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  //image_window3->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  //image_window4->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  //image_window5->DisplayThumbnailImage(int(width1), int(height1),
  //                                     thumbnail_image_data1);
  qApp->exec();

  QCOMPARE(image_io.LoadImage(origin_path1, origin_image_data1), 0);
  //positions[3] = Position(Float(width1) * Float(0.5),
  //                        Float(height1 * Float(0.5)));
  ImageData icon_tiepoint = LoadIcon(QString(":/images/icon_tiepoint.png"));
  image_window1->SetIcon(icon_tiepoint, 0, icon_tiepoint.height() / 2);
  image_window1->SetPositions(positions);
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

  QCOMPARE(image_io.LoadImage(thumbnail_path2, thumbnail_image_data2), 0);
  QCOMPARE(image_io.GetImageDimension(origin_path2, width2, height2), 0);
  image_window1->show();
  //image_window2->show();
  //image_window3->show();
  //image_window4->show();
  //image_window5->show();
  image_window1->DisplayThumbnailImage(int(width2), int(height2),
                                       thumbnail_image_data2);
  //image_window2->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window3->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window4->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  //image_window5->DisplayThumbnailImage(int(width2), int(height2),
  //                                     thumbnail_image_data2);
  qApp->exec();

  QCOMPARE(image_io.LoadImage(origin_path2, origin_image_data2), 0);
  image_window1->show();
  //image_window2->show();
  //image_window3->show();
  //image_window4->show();
  //image_window5->show();
  image_window1->SetOriginImage(origin_image_data2);
  //image_window2->SetOriginImage(origin_image_data2);
  //image_window3->SetOriginImage(origin_image_data2);
  //image_window4->SetOriginImage(origin_image_data2);
  //image_window5->SetOriginImage(origin_image_data2);
  qApp->exec();

}

}

QTEST_MAIN(test::TestImageOpenGLWindow)
#include "test_image_opengl_window.moc"
