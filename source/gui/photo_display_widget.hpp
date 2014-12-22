#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_DISPLAY_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_DISPLAY_WIDGET_HPP_

#include <memory>
#include <thread>
#include <mutex>

#include <QTimer>

#include "gui/image_container_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotoDisplayWidget : public ImageContainerWidget
{
  Q_OBJECT
public:
  typedef ImageOpenGLWindow::ImageData ImageData;
public:
  PhotoDisplayWidget(ImageOpenGLWindow* image_window, QWidget* parent = 0);
  virtual ~PhotoDisplayWidget();
  void ResetImage(const ImageData& thumbnail_image_data,
                  const QString& origin_image_path);

private:
  int StartLoadingThread();
  int StopLoadingThread();
  void LoadingThreadWork();
  void Lock();
  void Unlock();
private slots:
  void OnTimeout();

private:
  QTimer* update_origin_timer_;

  std::thread working_thread_;
  std::mutex mutex_;
  int keep_working_;
  int keep_loading_;
  bool origin_image_loaded_;
  bool origin_image_rendered_;
  std::string origin_image_path_;
  ImageData origin_image_data_;
};

}
}
}

#endif
