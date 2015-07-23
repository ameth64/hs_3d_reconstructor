#include <string>
#include <algorithm>

#if 1
#include <iostream>
#endif

#include <QScrollBar>
#include <QFileInfo>

#include "hs_image_io/whole_io/image_io.hpp"

#include "gui/image_measure_opengl_window.hpp"
#include "gui/tiepoint_measure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

TiepointPhoto::TiepointPhoto()
  : photo_id(std::numeric_limits<uint>::max())
  , photo_path()
  , width(0)
  , height(0)
{
  predicated_pos[0] = std::numeric_limits<Float>::max();
  predicated_pos[1] = std::numeric_limits<Float>::max();
  measured_pos[0] = std::numeric_limits<Float>::max();
  measured_pos[1] = std::numeric_limits<Float>::max();
}

TiepointPhotoAlignment::TiepointPhotoAlignment()
  : rows_in_page(0)
  , cols_in_page(0)
  , number_of_pages(0)
{
}

TiepointMeasureWidget::TiepointMeasureWidget(QWidget* parent)
  : QWidget(parent)
  , layout_(new QHBoxLayout(this))
  , canvas_(new QWidget(this))
  , slider_(new QScrollBar(this))
  , slider_resize_image_displayer_(new QSlider(this))
  , displayer_width_(240)
  , displayer_height_(180)
  , vertical_gap_(10)
  , horizontal_gap_(10)
  , update_origin_timer_(new QTimer(this))
{
  canvas_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding));
  slider_->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                     QSizePolicy::Minimum));
  slider_resize_image_displayer_->setRange(0,500);
  //slider_->setTickPosition(QSlider::TicksBothSides);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->addWidget(canvas_);
  layout_->addWidget(slider_resize_image_displayer_);
  layout_->addWidget(slider_);

  setMinimumSize(horizontal_gap_ + displayer_width_ + slider_->width() + 20,
                 vertical_gap_ + displayer_height_ + 20);

  StartLoadingThread();
  update_origin_timer_->start(100);
  QObject::connect(slider_, &QAbstractSlider::valueChanged,
                   this, &TiepointMeasureWidget::UpdateImageWindows);
  QObject::connect(update_origin_timer_, &QTimer::timeout,
                   this, &TiepointMeasureWidget::OnTimeout);
  QObject::connect(slider_resize_image_displayer_,&QSlider::valueChanged,
                   this,&TiepointMeasureWidget::DisplayerSizeChanged);
}

TiepointMeasureWidget::~TiepointMeasureWidget()
{
  StopLoadingThread();
}

int TiepointMeasureWidget::SetTiepointPhotos(
  const TiepointPhotoContainer& tiepoint_photos)
{
  tiepoint_photos_ = tiepoint_photos;
  return UpdateAlignment();
}

void TiepointMeasureWidget::resizeEvent(QResizeEvent* event)
{
  UpdateAlignment();
  QWidget::resizeEvent(event);
}

int TiepointMeasureWidget::UpdateAlignment()
{
  QRect rect = canvas_->geometry();
  alignment_.cols_in_page =
    (rect.width() - horizontal_gap_) / (displayer_width_ + horizontal_gap_);
  if (alignment_.cols_in_page < 1) return -1;
  alignment_.rows_in_page =
    (rect.height() - vertical_gap_) / (displayer_height_ + vertical_gap_);
  if (alignment_.rows_in_page < 1) return -1;
  int displayers_in_page = alignment_.cols_in_page * alignment_.rows_in_page;
  alignment_.number_of_pages =
    (int(tiepoint_photos_.size()) + displayers_in_page - 1) /
    displayers_in_page;

  if (alignment_.number_of_pages <= 0) alignment_.number_of_pages = 1;

  auto itr_tiepoint_photo = tiepoint_photos_.begin();
  auto itr_tiepoint_photo_end = tiepoint_photos_.end();
  int row = 0;
  int col = 0;
  alignment_.photo_id_map.clear();
  for (; itr_tiepoint_photo != itr_tiepoint_photo_end; ++itr_tiepoint_photo)
  {
    TiepointPhotoAlignment::AlignmentKey key(row, col);
    alignment_.photo_id_map[key] = itr_tiepoint_photo->first;
    col++;
    if (col >= alignment_.cols_in_page)
    {
      col = 0;
      row++;
    }
  }

  slider_->setRange(0, alignment_.number_of_pages - 1);
  slider_->setPageStep(1);
  slider_->setSingleStep(1);

  slider_->setValue(std::min(slider_->value(), alignment_.number_of_pages - 1));

  UpdateImageWindows();

  return 0;
}

int TiepointMeasureWidget::StartLoadingThread()
{
  keep_working_ = 1;
  working_thread_ =
    std::thread(&TiepointMeasureWidget::LoadingThreadWork, this);
  return 0;
}

int TiepointMeasureWidget::StopLoadingThread()
{
  keep_loading_ = 0;
  keep_working_ = 0;
  if (working_thread_.joinable())
  {
    working_thread_.join();
  }

  return 0;
}

void TiepointMeasureWidget::LoadingThreadWork()
{
  while (keep_working_)
  {
    uint photo_id;
    std::string photo_path;
    Lock();
    if (!loading_task_.empty())
    {
      auto itr_task = loading_task_.begin();
      auto itr_result = loading_result_.find(itr_task->first);
      if (itr_result == loading_result_.end())
      {
        photo_id = itr_task->first;
        photo_path = itr_task->second.toLocal8Bit().data();
      }
      loading_task_.erase(itr_task);
    }
    Unlock();

    if (!photo_path.empty())
    {
      hs::imgio::whole::ImageIO image_io;
      ImageData image_data;
      keep_loading_ = 1;
      image_io.LoadImage(photo_path, image_data, &keep_loading_);

      Lock();
      loading_result_[photo_id] = image_data;
      Unlock();
    }
  }
}

void TiepointMeasureWidget::Lock()
{
  mutex_.lock();
}

void TiepointMeasureWidget::Unlock()
{
  mutex_.unlock();
}

int TiepointMeasureWidget::UpdateImageWindows()
{
  int number_of_displayers = alignment_.cols_in_page * alignment_.rows_in_page;
  int size_of_pool = number_of_displayers - int(used_displayers_.size());
  if (size_of_pool < int(displayer_pool_.size()))
  {
    auto itr_displayer = displayer_pool_.begin();
    int number_of_eliminating = int(displayer_pool_.size()) - size_of_pool;
    for (int i = 0;
         itr_displayer != displayer_pool_.end() &&
         i < number_of_eliminating; ++i)
    {
      delete *itr_displayer;
      itr_displayer = displayer_pool_.erase(itr_displayer);
    }
  }
  else
  {
    int number_of_adding = size_of_pool - int(displayer_pool_.size());
    for (int i = 0; i < number_of_adding; i++)
    {
      ImageMeasureOpenGLWindow* image_opengl_window =
        new ImageMeasureOpenGLWindow();
      Displayer* displayer =
        new Displayer(image_opengl_window, QString(), this);
      ImageMeasureOpenGLWindow* image_measure_opengl_window =
        static_cast<ImageMeasureOpenGLWindow*>(image_opengl_window);
      QObject::connect(image_measure_opengl_window,
                       &ImageMeasureOpenGLWindow::Measured,
                       this, &TiepointMeasureWidget::OnMeasured);
      QObject::connect(image_measure_opengl_window,
                       &ImageMeasureOpenGLWindow::MeasureDeleted,
                       this, &TiepointMeasureWidget::OnMeasureDeleted);
      displayer->resize(displayer_width_, displayer_height_);
      displayer->hide();
      displayer_pool_.push_back(displayer);
    }
  }

  //计算需要显示的照片id
  int slider_value = slider_->value();
  auto itr_photo_id = alignment_.photo_id_map.begin();
  auto itr_photo_id_end = alignment_.photo_id_map.end();
  TiepointPhotoAlignment::PhotoIDMap photo_id_map_need_reset;
  DisplayerContainer kept_displayers;
  for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
  {
    int row = itr_photo_id->first.first;
    int col = itr_photo_id->first.second;
    int offset = slider_value * alignment_.rows_in_page;
    if (row >= offset && row < (offset + alignment_.rows_in_page))
    {
      auto itr_displayer = used_displayers_.find(itr_photo_id->second);
      if (itr_displayer != used_displayers_.end())
      {
        kept_displayers[itr_displayer->first] = itr_displayer->second;
        //移动到合适位置
        itr_displayer->second->move(
          horizontal_gap_ +
          col * (displayer_width_ + horizontal_gap_),
          vertical_gap_ +
          (row - slider_value * alignment_.rows_in_page) *
          (displayer_height_ + vertical_gap_));
        //重新设置大小
        itr_displayer->second->resize(displayer_width_,displayer_height_);
        //重新设置座标
        auto itr_tiepoint_photo = tiepoint_photos_.find(itr_displayer->first);
        ImageOpenGLWindow* image_opengl_window =
          itr_displayer->second->image_window();
        ImageMeasureOpenGLWindow* image_measure_opengl_window =
          static_cast<ImageMeasureOpenGLWindow*>(image_opengl_window);
        if (itr_tiepoint_photo->second.predicated_pos[0] !=
            std::numeric_limits<Float>::max() &&
            itr_tiepoint_photo->second.predicated_pos[1] !=
            std::numeric_limits<Float>::max())
        {
          ImageMeasureOpenGLWindow::Position pos_predicated;
          pos_predicated << itr_tiepoint_photo->second.predicated_pos[0],
                            itr_tiepoint_photo->second.predicated_pos[1];
          image_measure_opengl_window->set_pos_predicated(pos_predicated);
        }
        else
        {
          image_measure_opengl_window->ClearPredicated();
        }
        if (itr_tiepoint_photo->second.measured_pos[0] !=
            std::numeric_limits<Float>::max() &&
            itr_tiepoint_photo->second.measured_pos[1] !=
            std::numeric_limits<Float>::max())
        {
          ImageMeasureOpenGLWindow::Position pos_measure;
          pos_measure << itr_tiepoint_photo->second.measured_pos[0],
                         itr_tiepoint_photo->second.measured_pos[1];
          image_measure_opengl_window->set_pos_measure(pos_measure);
        }
        else
        {
          image_measure_opengl_window->ClearMeasure();
        }
        used_displayers_.erase(itr_displayer);
      }
      else
      {
        photo_id_map_need_reset[itr_photo_id->first] = itr_photo_id->second;
      }
    }
  }
  //将需要重新设置的影像显示窗口放入影像显示窗口池中
  auto itr_displayer = used_displayers_.begin();
  for (; itr_displayer != used_displayers_.end();)
  {
    itr_displayer->second->hide();
    ImageOpenGLWindow* image_opengl_window =
      itr_displayer->second->image_window();
    image_opengl_window->ClearImage();
    ImageMeasureOpenGLWindow* image_measure_opengl_window =
      static_cast<ImageMeasureOpenGLWindow*>(image_opengl_window);
    image_measure_opengl_window->ClearPosition();
    displayer_pool_.push_back(itr_displayer->second);
    Lock();
    auto itr_task = loading_task_.find(itr_displayer->first);
    if (itr_task != loading_task_.end())
    {
      loading_task_.erase(itr_task);
    }
    Unlock();
    itr_displayer = used_displayers_.erase(itr_displayer);
  }
  used_displayers_.swap(kept_displayers);

  //重新读取并设置影像
  itr_photo_id = photo_id_map_need_reset.begin();
  itr_photo_id_end = photo_id_map_need_reset.end();
  hs::imgio::whole::ImageIO image_io;
  for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
  {
    auto itr_tiepoint_photo = tiepoint_photos_.find(itr_photo_id->second);
    if (itr_tiepoint_photo == tiepoint_photos_.end()) continue;
    QString photo_path = itr_tiepoint_photo->second.photo_path;
    QFileInfo photo_file_info(photo_path);
    std::string std_photo_path = photo_path.toLocal8Bit().data();
    hs::imgio::whole::ImageData image_data;
    QString thumbnail_path = itr_tiepoint_photo->second.thumbnail_path;
    std::string std_thumbnail_path = thumbnail_path.toLocal8Bit().data();

    image_io.LoadImage(std_thumbnail_path, image_data);
    Displayer* displayer = displayer_pool_.back();
    int row = itr_photo_id->first.first;
    int col = itr_photo_id->first.second;
    displayer->SetComment(photo_file_info.baseName());
    displayer->move(horizontal_gap_ +
                    col * (displayer_width_ + horizontal_gap_),
                    vertical_gap_ +
                    (row - slider_value * alignment_.rows_in_page) *
                    (displayer_height_ + vertical_gap_));
    ImageOpenGLWindow* image_opengl_window = displayer->image_window();
    image_opengl_window->SetThumbnailImage(
      itr_tiepoint_photo->second.width, itr_tiepoint_photo->second.height,
      image_data);
    ImageMeasureOpenGLWindow* image_measure_opengl_window =
      static_cast<ImageMeasureOpenGLWindow*>(image_opengl_window);
    if (itr_tiepoint_photo->second.predicated_pos[0] !=
        std::numeric_limits<Float>::max() &&
        itr_tiepoint_photo->second.predicated_pos[1] !=
        std::numeric_limits<Float>::max())
    {
      ImageMeasureOpenGLWindow::Position pos_predicated;
      pos_predicated << itr_tiepoint_photo->second.predicated_pos[0],
                        itr_tiepoint_photo->second.predicated_pos[1];
      image_measure_opengl_window->set_pos_predicated(pos_predicated);
    }
    if (itr_tiepoint_photo->second.measured_pos[0] !=
        std::numeric_limits<Float>::max() &&
        itr_tiepoint_photo->second.measured_pos[1] !=
        std::numeric_limits<Float>::max())
    {
      ImageMeasureOpenGLWindow::Position pos_measure;
      pos_measure << itr_tiepoint_photo->second.measured_pos[0],
                     itr_tiepoint_photo->second.measured_pos[1];
      image_measure_opengl_window->set_pos_measure(pos_measure);
    }
    image_measure_opengl_window->set_photo_id(itr_tiepoint_photo->first);
    displayer_pool_.pop_back();
    displayer->resize(displayer_width_,displayer_height_);
    displayer->show();

    used_displayers_[itr_photo_id->second] = displayer;

    Lock();
    auto itr_task = loading_task_.find(itr_photo_id->second);
    if (itr_task == loading_task_.end())
    {
      loading_task_[itr_photo_id->second] = photo_path;
    }
    Unlock();
  }

  //销毁无用的影像显示窗口
  size_of_pool = number_of_displayers - int(used_displayers_.size());
  if (size_of_pool < int(displayer_pool_.size()))
  {
    auto itr_displayer = displayer_pool_.begin();
    int number_of_eliminating = int(displayer_pool_.size()) - size_of_pool;
    for (int i = 0;
         itr_displayer != displayer_pool_.end() &&
         i < number_of_eliminating; ++i)
    {
      delete *itr_displayer;
      itr_displayer = displayer_pool_.erase(itr_displayer);
    }
  }

  return 0;
}

void TiepointMeasureWidget::OnTimeout()
{
  auto itr_result = loading_result_.begin();
  for (; itr_result != loading_result_.end();)
  {
    auto itr_displayer = used_displayers_.find(itr_result->first);
    if (itr_displayer != used_displayers_.end())
    {
      ImageOpenGLWindow* image_opengl_window =
        itr_displayer->second->image_window();
      image_opengl_window->SetOriginImage(itr_result->second);
    }
    itr_result = loading_result_.erase(itr_result);
  }
}

void TiepointMeasureWidget::OnMeasured(uint photo_id,
                                       const EIGEN_VECTOR(Float, 2)& position)
{
  emit TransmissionMeasured(photo_id, position);
}

void TiepointMeasureWidget::OnMeasureDeleted(uint photo_id)
{
  emit TransmissionMeasureDeleted(photo_id);
}
void TiepointMeasureWidget::DisplayerSizeChanged(int scale)
{
  displayer_width_ = 240 + scale;
  displayer_height_ = 180 + scale;
  UpdateAlignment();
}

}
}
}

