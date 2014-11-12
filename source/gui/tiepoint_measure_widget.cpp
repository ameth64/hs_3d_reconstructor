#include "gui/tiepoint_measure_widget.hpp"

#include <string>
#include <algorithm>
#include <iostream>

#include <QScrollBar>
#include <QFileInfo>

#include "hs_image_io/whole_io/image_io.hpp"

namespace hs
{
namespace recon
{

//TiepointMeasureWidget::TiepointMeasureWidget(QWidget* parent)
//  : QAbstractScrollArea(parent)
//  , vertical_gap_(10)
//  , horizontal_gap_(10)
//  , image_window_width_(400)
//  , image_window_height_(300)
//  , cached_count_(1)
//  , canvas_(new QWidget(viewport()))
//{
//  setVerticalScrollBar(new QScrollBar(this));
//  setHorizontalScrollBar(new QScrollBar(this));
//  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//  //canvas_->setParent(viewport());
//
//  QObject::connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
//                   this, &TiepointMeasureWidget::UpdateImageWindow);
//  QObject::connect(horizontalScrollBar(), &QAbstractSlider::valueChanged,
//                   this, &TiepointMeasureWidget::UpdateImageWindow);
//}
//
//int TiepointMeasureWidget::SetTiepointPhotos(
//  const TiepointPhotoContainer& tiepoint_photos)
//{
//  tiepoint_photos_ = tiepoint_photos;
//  UpdateTiepointPhotoLayout();
//  return 0;
//}
//
//int TiepointMeasureWidget::UpdateTiepointPhotoLayout()
//{
//  if (tiepoint_photos_.empty()) return 0;
//  int number_of_columns = (viewport()->width()/* - horizontal_gap_*/) /
//                          (image_window_width_ + horizontal_gap_);
//  int number_of_rows = 0;
//  if (number_of_columns == 0)
//  {
//    number_of_rows = int(tiepoint_photos_.size());
//    number_of_columns = 1;
//  }
//  else
//  {
//    number_of_rows = int(tiepoint_photos_.size() + number_of_columns - 1) /
//                     number_of_columns;
//  }
//  int total_width =
//    //horizontal_gap_ +
//    number_of_columns * (image_window_width_ + horizontal_gap_);
//  int total_height =
//    //vertical_gap_ +
//    number_of_rows * (image_window_height_ + vertical_gap_);
//  QRect viewport_rect = viewport()->rect();
//  std::cout<<"viewport_rect:"
//           <<viewport_rect.x()<<" "<<viewport_rect.y()<<" "
//           <<viewport_rect.width()<<" "<<viewport_rect.height()<<"\n";
//  verticalScrollBar()->setRange(
//    0, total_height - viewport_rect.height());
//  verticalScrollBar()->setPageStep(viewport_rect.height());
//  verticalScrollBar()->setSingleStep(50);
//  horizontalScrollBar()->setRange(
//    0, total_width - viewport_rect.width());
//  horizontalScrollBar()->setPageStep(viewport_rect.width());
//  horizontalScrollBar()->setSingleStep(50);
//  //verticalScrollBar()->setValue(0);
//  //verticalScrollBar()->setValue(0);
//
//  canvas_->resize(total_width, total_height);
//
//  auto itr_tiepoint_photo = tiepoint_photos_.begin();
//  auto itr_tiepoint_photo_end = tiepoint_photos_.end();
//  int row = 0;
//  int col = 0;
//  tiepoint_photo_layout_.clear();
//  for (; itr_tiepoint_photo != itr_tiepoint_photo_end; ++itr_tiepoint_photo)
//  {
//    LayoutKey layout_key(row, col);
//    tiepoint_photo_layout_[layout_key] = itr_tiepoint_photo->first;
//    col++;
//    if (col >= number_of_columns)
//    {
//      col = 0;
//      row++;
//    }
//  }
//
//  UpdateImageWindow();
//  return 0;
//}
//
//int TiepointMeasureWidget::UpdateImageWindow()
//{
//  //移动画布
//  int vertical_value = verticalScrollBar()->value();
//  int horizontal_value = horizontalScrollBar()->value();
//  std::cout<<"vertical_value:"<<vertical_value<<"\n";
//  std::cout<<"horizontal_value:"<<horizontal_value<<"\n";
//  QRect viewport_rect = viewport()->rect();
//  QPoint top_left = viewport_rect.topLeft();
//  QPoint canvas_top_left(top_left.x() - horizontal_value,
//                         top_left.y() - vertical_value);
//  QRect canvas_rect = canvas_->geometry();
//  canvas_->move(canvas_top_left);
//  //canvas_->setGeometry(canvas_top_left.x(), canvas_top_left.y(),
//  //                     canvas_rect.width(), canvas_rect.height());
//  //std::cout<<"canvas_rect:"
//  //         <<canvas_rect.x()<<" "<<canvas_rect.y()<<" "
//  //         <<canvas_rect.width()<<" "<<canvas_rect.height()<<"\n";
//
//  //根据需要调整影像显示窗口池的大小
//  int rows = (viewport_rect.height() + image_window_height_) /
//             (image_window_height_ + vertical_gap_);
//  int cols = (viewport_rect.width() + image_window_width_) /
//             (image_window_width_ + horizontal_gap_);
//  if (((vertical_value - vertical_gap_) %
//       (image_window_height_ + vertical_gap_)) < vertical_gap_ &&
//      ((viewport_rect.height() + image_window_height_) %
//       (image_window_height_ + vertical_gap_)) == 0)
//  {
//    rows++;
//  }
//  if (((horizontal_value - horizontal_gap_) %
//       (image_window_width_ + horizontal_gap_)) < horizontal_gap_ &&
//      ((viewport_rect.width() + image_window_width_) %
//       (image_window_width_ + horizontal_gap_)) == 0)
//  {
//    cols++;
//  }
//  int size_of_pool = rows * cols - int(identified_image_displayers_.size());
//  if (size_of_pool < int(image_displayer_pool_.size()))
//  {
//    auto itr_image_displayer = image_displayer_pool_.begin();
//    auto itr_image_displayer_end = image_displayer_pool_.end();
//    int number_of_eliminating =
//      int(image_displayer_pool_.size()) - size_of_pool;
//    for (int i = 0;
//         itr_image_displayer != itr_image_displayer_end &&
//         i < number_of_eliminating;
//         ++i)
//    {
//      delete itr_image_displayer->first;
//      itr_image_displayer = image_displayer_pool_.erase(itr_image_displayer);
//    }
//  }
//  else
//  {
//    int number_of_adding = size_of_pool - int(image_displayer_pool_.size());
//    for (int i = 0; i < number_of_adding; i++)
//    {
//      ImageWindow* image_window = new ImageWindow();
//      QWidget* container = QWidget::createWindowContainer(image_window,
//                                                          canvas_);
//      container->resize(image_window_width_, image_window_height_);
//      image_displayer_pool_.push_back(ImageDisplayer(container, image_window));
//    }
//  }
//
//  //计算与viewport相交的照片布局
//  auto itr_layout = tiepoint_photo_layout_.begin();
//  auto itr_layout_end = tiepoint_photo_layout_.end();
//  TiepointPhotoLayout layout_need_reset;
//  IdentifiedImageDisplayerContainer kept_displayers;
//  for (; itr_layout != itr_layout_end; ++itr_layout)
//  {
//    QPoint canvas_pos = LayoutKeyToCanvasPos(itr_layout->first);
//    //判断是否与viewport相交
//    QRect photo_rect(canvas_pos, QSize(image_window_width_,
//                                       image_window_height_));
//    if (photo_rect.intersects(viewport_rect))
//    {
//      //找出需要重新设置的影像显示窗口
//      auto itr_identified_image_displayer =
//        identified_image_displayers_.find(itr_layout->second);
//      if (itr_identified_image_displayer != identified_image_displayers_.end())
//      {
//        kept_displayers[itr_identified_image_displayer->first] =
//          itr_identified_image_displayer->second;
//      }
//      else
//      {
//        layout_need_reset[itr_layout->first] = itr_layout->second;
//      }
//    }
//  }
//  //将需要重新设置的影像显示窗口放入影像显示窗口池中
//  auto itr_image_displayer = identified_image_displayers_.begin();
//  auto itr_image_displayer_end = identified_image_displayers_.end();
//  for (; itr_image_displayer != itr_image_displayer;)
//  {
//    QWidget* container = itr_image_displayer->second.first;
//    ImageWindow* image_window = itr_image_displayer->second.second;
//    image_window->ClearImage();
//    image_window->hide();
//    container->hide();
//    image_displayer_pool_.push_back(ImageDisplayer(container, image_window));
//    itr_image_displayer =
//      identified_image_displayers_.erase(itr_image_displayer);
//  }
//  identified_image_displayers_.swap(kept_displayers);
//
//  //重新读取并设置影像
//  auto itr_layout_need_reset = layout_need_reset.begin();
//  auto itr_layout_need_reset_end = layout_need_reset.end();
//  hs::imgio::whole::ImageIO image_io;
//  for (; itr_layout_need_reset != itr_layout_need_reset_end;
//       ++itr_layout_need_reset)
//  {
//    auto itr_tiepoint_photo =
//      tiepoint_photos_.find(itr_layout_need_reset->second);
//    if (itr_tiepoint_photo == tiepoint_photos_.end()) continue;
//    std::string std_photo_path =
//      itr_tiepoint_photo->second.photo_path.toLocal8Bit().data();
//    hs::imgio::whole::ImageData image_data;
//    image_io.LoadImage(std_photo_path, image_data);
//    QWidget* container = image_displayer_pool_.back().first;
//    QPoint image_window_pos =
//      LayoutKeyToCanvasPos(itr_layout_need_reset->first);
//    container->move(image_window_pos);
//    ImageWindow* image_window = image_displayer_pool_.back().second;
//    image_window->DisplayImage(image_data);
//    image_displayer_pool_.pop_back();
//    container->show();
//    ImageDisplayer image_displayer(container, image_window);
//    identified_image_displayers_[itr_layout_need_reset->second] =
//      image_displayer;
//  }
//
//  //销毁无用的影像显示窗口
//  size_of_pool = rows * cols - int(identified_image_displayers_.size());
//  if (size_of_pool < int(image_displayer_pool_.size()))
//  {
//    auto itr_image_displayer = image_displayer_pool_.begin();
//    auto itr_image_displayer_end = image_displayer_pool_.end();
//    int number_of_eliminating = int(image_displayer_pool_.size()) -
//                                size_of_pool;
//    for (int i = 0;
//         itr_image_displayer != itr_image_displayer_end &&
//         i < number_of_eliminating;
//         ++i)
//    {
//      delete itr_image_displayer->first;
//      itr_image_displayer = image_displayer_pool_.erase(itr_image_displayer);
//    }
//  }
//
//  return 0;
//}
//
//QPoint TiepointMeasureWidget::LayoutKeyToCanvasPos(
//  const LayoutKey& layout_key) const
//{
//  return QPoint(horizontal_gap_ +
//                layout_key.second * (image_window_width_ + horizontal_gap_),
//                vertical_gap_ +
//                layout_key.first * (image_window_height_ + vertical_gap_));
//}
//
//TiepointMeasureWidget::LayoutKey TiepointMeasureWidget::CanvasPosToLayoutKey(
//  const QPoint& canvas_pos) const
//{
//  return LayoutKey((canvas_->width() - horizontal_gap_) /
//                   (image_window_width_ + horizontal_gap_),
//                   (canvas_->height() - vertical_gap_) /
//                   (image_window_height_ + vertical_gap_));
//}

TiepointPhoto::TiepointPhoto()
  : photo_id(std::numeric_limits<uint>::max())
  , photo_path()
{
  predicated_pos[0] = std::numeric_limits<Float>::quiet_NaN();
  predicated_pos[1] = std::numeric_limits<Float>::quiet_NaN();
  measured_pos[0] = std::numeric_limits<Float>::quiet_NaN();
  measured_pos[1] = std::numeric_limits<Float>::quiet_NaN();
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
  , displayer_width_(300)
  , displayer_height_(225)
  , vertical_gap_(10)
  , horizontal_gap_(10)
{
  canvas_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding));
  slider_->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                     QSizePolicy::Minimum));
  //slider_->setTickPosition(QSlider::TicksBothSides);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->addWidget(canvas_);
  layout_->addWidget(slider_);

  setMinimumSize(horizontal_gap_ + displayer_width_ + slider_->width() + 20,
                 vertical_gap_ + displayer_height_ + 20);

  QObject::connect(slider_, &QAbstractSlider::valueChanged,
                   this, &TiepointMeasureWidget::UpdateImageWindows);
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
      ImageOpenGLWindow* image_opengl_window = new ImageOpenGLWindow();
      Displayer* displayer =
        new Displayer(image_opengl_window, QString(), this);
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
  for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
  {
    int row = itr_photo_id->first.first;
    int col = itr_photo_id->first.second;
    int offset = slider_value * alignment_.rows_in_page;
    if (row >= offset && row < (offset + alignment_.rows_in_page))
    {
      auto itr_displayer = used_displayers_.find(itr_photo_id->second);
      if (itr_displayer == used_displayers_.end())
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
    displayer_pool_.push_back(itr_displayer->second);
    itr_displayer = used_displayers_.erase(itr_displayer);
  }

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
    image_io.LoadImage(std_photo_path, image_data);
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
    image_opengl_window->DisplayImage(image_data);
    //image_opengl_window->DisplayImage(itr_tiepoint_photo->second.image_data);
    displayer_pool_.pop_back();
    displayer->show();
    used_displayers_[itr_photo_id->second] = displayer;
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


}
}

