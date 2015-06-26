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
namespace gui
{

/**
 *  该Widget显示与像控点相关联的照片。
 */
struct TiepointPhoto
{
  typedef float Float;
  TiepointPhoto();

  uint photo_id;
  QString photo_path;
  QString thumbnail_path;
  int width;
  int height;
  Float predicated_pos[2];
  Float measured_pos[2];

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
  typedef hs::imgio::whole::ImageData ImageData;
  typedef std::map<uint, TiepointPhoto> TiepointPhotoContainer;

  typedef ImageContainerWidget Displayer;
  typedef std::map<uint, Displayer*> DisplayerContainer;
  typedef std::vector<Displayer*> DisplayerPool;
  typedef std::map<uint, QString> LoadingTask;
  typedef std::map<uint, ImageData> LoadingResult;

public:
  TiepointMeasureWidget(QWidget* parent = 0);
  virtual ~TiepointMeasureWidget();
  int SetTiepointPhotos(const TiepointPhotoContainer& tiepoint_photos);

protected:
  void resizeEvent(QResizeEvent* event);

private:
  int UpdateAlignment();
  int StartLoadingThread();
  int StopLoadingThread();
  void LoadingThreadWork();
  void Lock();
  void Unlock();
private slots:
  int UpdateImageWindows();
  void OnTimeout();
  void OnMeasured(uint photo_id, const EIGEN_VECTOR(Float, 2)& position);
  void OnMeasureDeleted(uint photo_id);
  void DisplayerSizeChanged(int scale);

signals:
  void TransmissionMeasured(uint photo_id,
                            const EIGEN_VECTOR(Float, 2)& position);
  void TransmissionMeasureDeleted(uint photo_id);

private:
  TiepointPhotoContainer tiepoint_photos_;
  QWidget* canvas_;
  QScrollBar* slider_;
  QHBoxLayout* layout_;
  QSlider* slider_resize_image_displayer_;

  TiepointPhotoAlignment alignment_;
  int displayer_width_;
  int displayer_height_;
  int vertical_gap_;
  int horizontal_gap_;

  DisplayerContainer used_displayers_;
  DisplayerPool displayer_pool_;

  std::thread working_thread_;
  std::mutex mutex_;
  int keep_working_;
  int keep_loading_;
  LoadingTask loading_task_;
  LoadingResult loading_result_;

  QTimer* update_origin_timer_;
};

}
}
}

#endif
