#ifndef _HS_3D_RECONSTRUCTOR_TIEPOINT_MEASURE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_TIEPOINT_MEASURE_WIDGET_HPP_

#include <map>
#include <vector>
#include <thread>
#include <mutex>

#include <QWidget>
#include <QSlider>
#include <QScrollBar>

#include "gui/image_container_widget.hpp"

namespace hs
{
namespace recon
{

/**
 *  该Widget显示与像控点相关联的照片。
 *  有三种显示方式：
 *  1.缩略图列表模式，此时只显示照片的缩略图，不可对照片作放大缩小平移等操作；
 *  2.快速标记模式，此时显示照片一定范围内的准确像素，可放大缩小至一定程度；
 *  3.误差列表模式，此时显示各张照片测量的重投影误差。
 */
//class TiepointMeasureWidget : public QAbstractScrollArea
//{
//  Q_OBJECT
//public:
//  typedef float Float;
//  struct TiepointPhoto
//  {
//    TiepointPhoto()
//      : photo_id(std::numeric_limits<uint>::max())
//      , photo_path()
//    {
//      predicated_pos[0] = std::numeric_limits<Float>::quiet_NaN();
//      predicated_pos[1] = std::numeric_limits<Float>::quiet_NaN();
//      measured_pos[0] = std::numeric_limits<Float>::quiet_NaN();
//      measured_pos[1] = std::numeric_limits<Float>::quiet_NaN();
//    }
//    uint photo_id;
//    QString photo_path;
//    Float predicated_pos[2];
//    Float measured_pos[2];
//  };
//  typedef std::map<uint, TiepointPhoto> TiepointPhotoContainer;
//  typedef std::pair<int, int> LayoutKey;
//  typedef std::map<LayoutKey, uint> TiepointPhotoLayout;
//  typedef std::map<LayoutKey, ImageWindow> LayoutedImageWindowContainer;
//  typedef std::pair<QWidget*, ImageWindow*> ImageDisplayer;
//  typedef std::map<uint, ImageDisplayer> IdentifiedImageDisplayerContainer;
//  typedef std::vector<ImageDisplayer> ImageDisplayerPool;
//
//public:
//  TiepointMeasureWidget(QWidget* parent = 0);
//  int SetTiepointPhotos(const TiepointPhotoContainer& tiepoint_photos);
//private:
//  int UpdateTiepointPhotoLayout();
//  QPoint LayoutKeyToCanvasPos(const LayoutKey& layout_key) const;
//  LayoutKey CanvasPosToLayoutKey(const QPoint& canvas_pos) const;
//private slots:
//  int UpdateImageWindow();
//
//private:
//  int vertical_gap_;
//  int horizontal_gap_;
//  int image_window_width_;
//  int image_window_height_;
//  int cached_count_;
//
//  QWidget* canvas_;
//
//  TiepointPhotoContainer tiepoint_photos_;
//  TiepointPhotoLayout tiepoint_photo_layout_;
//  IdentifiedImageDisplayerContainer identified_image_displayers_;
//  ImageDisplayerPool image_displayer_pool_;
//};

struct TiepointPhoto
{
  typedef float Float;
  TiepointPhoto();

  uint photo_id;
  QString photo_path;
  Float predicated_pos[2];
  Float measured_pos[2];
  //hs::imgio::whole::ImageData image_data;
};

struct TiepointPhotoAlignment
{
  typedef std::pair<int, int> AlignmentKey;
  typedef std::map<AlignmentKey, uint> PhotoIDMap;
  TiepointPhotoAlignment();
  int rows_in_page;
  int cols_in_page;
  int number_of_pages;
  PhotoIDMap photo_id_map;
};

class TiepointMeasureWidget : public QWidget
{
  Q_OBJECT
public:
  typedef TiepointPhoto::Float Float;
  typedef std::map<uint, TiepointPhoto> TiepointPhotoContainer;

  typedef ImageContainerWidget Displayer;
  typedef std::map<uint, Displayer*> DisplayerContainer;
  typedef std::vector<Displayer*> DisplayerPool;

public:
  TiepointMeasureWidget(QWidget* parent = 0);
  int SetTiepointPhotos(const TiepointPhotoContainer& tiepoint_photos);

protected:
  void resizeEvent(QResizeEvent* event);

private:
  int UpdateAlignment();
private slots:
  int UpdateImageWindows();

private:
  TiepointPhotoContainer tiepoint_photos_;
  QWidget* canvas_;
  QScrollBar* slider_;
  QHBoxLayout* layout_;

  TiepointPhotoAlignment alignment_;
  int displayer_width_;
  int displayer_height_;
  int vertical_gap_;
  int horizontal_gap_;

  DisplayerContainer used_displayers_;
  DisplayerPool displayer_pool_;
};

}
}

#endif
