#include <QtTest/QtTest>
#include <QApplication>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/tiepoint_measure_widget.hpp"

namespace test
{

class TestTiepointMeasureWidget : public QObject
{
  Q_OBJECT
private:
  typedef hs::recon::TiepointMeasureWidget TiepointMeasureWidget;
  typedef hs::recon::TiepointPhoto TiepointPhoto;
  typedef TiepointPhoto::Float Float;
  typedef TiepointMeasureWidget::TiepointPhotoContainer TiepointPhotoContainer;

private slots:
  void Test();
};

void TestTiepointMeasureWidget::Test()
{
  TiepointMeasureWidget widget;
  widget.resize(1000, 500);
  widget.show();

  TiepointPhotoContainer tiepoint_photos;
  TiepointPhoto tiepoint_photo;

  tiepoint_photo.photo_id = 1;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC00036.JPG");
  tiepoint_photo.measured_pos[0] = Float(3.7515994e+03);
  tiepoint_photo.measured_pos[1] = Float(2.6030869e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 2;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC00037.JPG");
  tiepoint_photo.measured_pos[0] = Float(3.9375232e+03);
  tiepoint_photo.measured_pos[1] = Float(2.2473549e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 3;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC00123.JPG");
  tiepoint_photo.measured_pos[0] = Float(4.6977822e+03);
  tiepoint_photo.measured_pos[1] = Float(1.0951683e+03);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 4;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC00125.JPG");
  tiepoint_photo.measured_pos[0] = Float(2.5812996e+03);
  tiepoint_photo.measured_pos[1] = Float(6.5750354e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 5;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC00291.JPG");
  tiepoint_photo.measured_pos[0] = Float(5.3914232e+00);
  tiepoint_photo.measured_pos[1] = Float(3.2354810e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 6;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC09887.JPG");
  tiepoint_photo.measured_pos[0] = Float(1.5532335e+03);
  tiepoint_photo.measured_pos[1] = Float(5.8003125e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 7;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC09889.JPG");
  tiepoint_photo.measured_pos[0] = Float(9.7950128e+02);
  tiepoint_photo.measured_pos[1] = Float(7.9357324e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 8;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC09891.JPG");
  tiepoint_photo.measured_pos[0] = Float(7.0302057e+02);
  tiepoint_photo.measured_pos[1] = Float(1.0066604e+03);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 9;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC09893.JPG");
  tiepoint_photo.measured_pos[0] = Float(3.7815771e+02);
  tiepoint_photo.measured_pos[1] = Float(1.2753890e+03);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 10;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC09981.JPG");
  tiepoint_photo.measured_pos[0] = Float(4.3240625e+03);
  tiepoint_photo.measured_pos[1] = Float(1.5975845e+03);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  tiepoint_photo.photo_id = 11;
  tiepoint_photo.photo_path =
    QString::fromLocal8Bit("test_data/images/DSC09985.JPG");
  tiepoint_photo.measured_pos[0] = Float(1.6740935e+03);
  tiepoint_photo.measured_pos[1] = Float(7.2484967e+02);
  tiepoint_photo.predicated_pos[0] = 0;
  tiepoint_photo.predicated_pos[1] = 0;
  tiepoint_photos[tiepoint_photo.photo_id] = tiepoint_photo;

  //auto itr_tiepoint_photo = tiepoint_photos.begin();
  //auto itr_tiepoint_photo_end = tiepoint_photos.end();
  //hs::imgio::whole::ImageIO image_io;
  //for (; itr_tiepoint_photo != itr_tiepoint_photo_end; ++itr_tiepoint_photo)
  //{
  //  std::string std_path =
  //    itr_tiepoint_photo->second.photo_path.toLocal8Bit().data();
  //  image_io.LoadImage(std_path, itr_tiepoint_photo->second.image_data);
  //}

  widget.SetTiepointPhotos(tiepoint_photos);

  qApp->exec();
}

}

QTEST_MAIN(test::TestTiepointMeasureWidget)
#include "test_tiepoint_measure_widget.moc"
