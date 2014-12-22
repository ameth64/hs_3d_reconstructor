#include "gui/photo_display_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotoDisplayWidget::PhotoDisplayWidget(ImageOpenGLWindow* image_window,
                                       QWidget* parent)
  : ImageContainerWidget(image_window, QString(), parent)
  , update_origin_timer_(new QTimer(this))
  , origin_image_loaded_(false)
  , origin_image_rendered_(true)
{
  StartLoadingThread();
  update_origin_timer_->start(100);
  setMinimumSize(400, 300);

  QObject::connect(update_origin_timer_, &QTimer::timeout,
                   this, &PhotoDisplayWidget::OnTimeout);
}
PhotoDisplayWidget::~PhotoDisplayWidget()
{
  StopLoadingThread();
}

void PhotoDisplayWidget::ResetImage(const ImageData& thumbnail_image_data,
                                    const QString& origin_image_path)
{
  origin_image_path_ = origin_image_path.toLocal8Bit().data();
  hs::imgio::whole::ImageIO image_io;
  size_t width, height;
  if(image_io.GetImageDimension(origin_image_path_, width, height) == 0)
  {
    image_window()->SetThumbnailImage(int(width), int(height),
                                      thumbnail_image_data);
    Lock();
    origin_image_loaded_ = false;
    origin_image_rendered_ = false;
    origin_image_data_.Reset();
    Unlock();
  }
}

int PhotoDisplayWidget::StartLoadingThread()
{
  keep_working_ = 1;
  working_thread_ =
    std::thread(&PhotoDisplayWidget::LoadingThreadWork, this);
  return 0;
}

int PhotoDisplayWidget::StopLoadingThread()
{
  keep_loading_ = 0;
  keep_working_ = 0;
  if (working_thread_.joinable())
  {
    working_thread_.join();
  }

  return 0;
}

void PhotoDisplayWidget::LoadingThreadWork()
{
  while (keep_working_)
  {
    Lock();
    if (!origin_image_loaded_)
    {
      hs::imgio::whole::ImageIO image_io;
      if (image_io.LoadImage(origin_image_path_, origin_image_data_) == 0)
      {
        origin_image_loaded_ = true;
      }
    }
    Unlock();
  }
}

void PhotoDisplayWidget::Lock()
{
  mutex_.lock();
}

void PhotoDisplayWidget::Unlock()
{
  mutex_.unlock();
}

void PhotoDisplayWidget::OnTimeout()
{
  Lock();
  if (origin_image_loaded_ && !origin_image_rendered_)
  {
    image_window()->SetOriginImage(origin_image_data_);
    origin_image_rendered_ = true;
  }
  Unlock();
}

}
}
}
