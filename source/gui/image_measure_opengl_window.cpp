#include <limits>

#include <QImage>

#include "gui/image_measure_opengl_window.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

ImageMeasureOpenGLWindow::ImageMeasureOpenGLWindow(QWindow* parent)
  : Base(parent)
  , position_icon_2d_render_layer_predicated_(new PositionIcon2DRenderLayer)
  , position_icon_2d_render_layer_measure_(new PositionIcon2DRenderLayer)
  , photo_id_(std::numeric_limits<uint>::max())
{
  action_measure_ = new QAction(tr("Measure GCP"), this);
  action_delete_measure_ = new QAction(tr("Delete Measure"), this);

  menu_context_ = new QMenu;
  menu_context_->addAction(action_measure_);
  menu_context_->addAction(action_delete_measure_);

  pos_predicated_ << -std::numeric_limits<Float>::max(),
                     -std::numeric_limits<Float>::max();
  pos_measure_ << -std::numeric_limits<Float>::max(),
                  -std::numeric_limits<Float>::max();
  pos_menu_ << -std::numeric_limits<Float>::max(),
               -std::numeric_limits<Float>::max();

  AddRenderLayer(
    std::static_pointer_cast<RenderLayer>(
      position_icon_2d_render_layer_predicated_));
  AddRenderLayer(
    std::static_pointer_cast<RenderLayer>(
      position_icon_2d_render_layer_measure_));

  ImageData image_data_predicated =
    LoadIcon(QString(":/images/icon_tiepoint.png"));
  position_icon_2d_render_layer_predicated_->SetIcon(
    image_data_predicated, 0, image_data_predicated.height() / 2);
  ImageData image_data_measure =
    LoadIcon(QString(":/images/icon_gcp.png"));
  position_icon_2d_render_layer_measure_->SetIcon(
    image_data_measure, 0, image_data_measure.height() / 2);

  QObject::connect(this, &OpenGLWindow::MouseClicked,
                   this, &ImageMeasureOpenGLWindow::OnMouseClicked);
  QObject::connect(
    action_measure_, &QAction::triggered,
    this, &ImageMeasureOpenGLWindow::OnActionMeasureTriggered);
  QObject::connect(
    action_delete_measure_, &QAction::triggered,
    this, &ImageMeasureOpenGLWindow::OnActionDeleteMeasureTriggered);
}

ImageMeasureOpenGLWindow::Position
ImageMeasureOpenGLWindow::pos_predicated() const
{
  return WindowPosToPhotoPos(pos_predicated_);
}

ImageMeasureOpenGLWindow::Position
ImageMeasureOpenGLWindow::pos_measure() const
{
  return WindowPosToPhotoPos(pos_measure_);
}

void ImageMeasureOpenGLWindow::set_pos_predicated(
  const Position& pos_predicated)
{
  pos_predicated_ = PhotoPosToWindowPos(pos_predicated);
  PositionContainer positions(1, pos_predicated_);
  position_icon_2d_render_layer_predicated_->SetPositions(positions);
  LocateToScenePoint(pos_predicated_[0], pos_predicated_[1], Float(0.2));
  RenderNow();
}

void ImageMeasureOpenGLWindow::set_pos_measure(
  const Position& pos_measure)
{
  pos_measure_ = PhotoPosToWindowPos(pos_measure);
  PositionContainer positions(1, pos_measure_);
  position_icon_2d_render_layer_measure_->SetPositions(positions);
  LocateToScenePoint(pos_measure_[0], pos_measure_[1], Float(0.2));
  RenderNow();
}

uint ImageMeasureOpenGLWindow::photo_id() const
{
  return photo_id_;
}

void ImageMeasureOpenGLWindow::set_photo_id(uint photo_id)
{
  photo_id_ = photo_id;
}

void ImageMeasureOpenGLWindow::ClearPredicated()
{
  pos_predicated_ << -std::numeric_limits<Float>::max(),
                     -std::numeric_limits<Float>::max();
  PositionContainer positions_empty;
  position_icon_2d_render_layer_predicated_->SetPositions(positions_empty);
  RenderNow();
}

void ImageMeasureOpenGLWindow::ClearMeasure()
{
  pos_measure_ << -std::numeric_limits<Float>::max(),
                  -std::numeric_limits<Float>::max();
  PositionContainer positions_empty;
  position_icon_2d_render_layer_measure_->SetPositions(positions_empty);
  RenderNow();
}

void ImageMeasureOpenGLWindow::ClearPosition()
{
  pos_predicated_ << -std::numeric_limits<Float>::max(),
                     -std::numeric_limits<Float>::max();
  pos_measure_ << -std::numeric_limits<Float>::max(),
                  -std::numeric_limits<Float>::max();
  PositionContainer positions_empty;
  position_icon_2d_render_layer_predicated_->SetPositions(positions_empty);
  position_icon_2d_render_layer_measure_->SetPositions(positions_empty);
  RenderNow();
}

void ImageMeasureOpenGLWindow::OnMouseClicked(
  Qt::KeyboardModifiers state_key, Qt::MouseButton mouse_button, QPoint pos)
{
  typedef EIGEN_VECTOR(Float, 4) Vector4;
  typedef ViewingTransformer::TransformMatrix TransformMatrix;
  TransformMatrix transform_matrix =
    viewing_transformer_.ViewingTransformMatrix();
  Vector4 screen_pos;
  screen_pos << Float(pos.x()),
                Float(pos.y()),
                Float(0),
                Float(1);
  screen_pos[0] /=
    Float(viewing_transformer_.viewport_width()) * Float(0.5);
  screen_pos[1] /=
    Float(viewing_transformer_.viewport_height()) * Float(0.5);
  screen_pos[0] -= Float(1);
  screen_pos[1] -= Float(1);
  Vector4 object_pos = transform_matrix.inverse() * screen_pos;
  object_pos /= object_pos[3];

  if (state_key == Qt::ShiftModifier && mouse_button == Qt::LeftButton)
  {
    pos_measure_ << object_pos[0],
                    object_pos[1];
    PositionContainer positions(1, pos_measure_);
    position_icon_2d_render_layer_measure_->SetPositions(positions);
    emit Measured(photo_id_, WindowPosToPhotoPos(pos_measure_));

    RenderNow();
  }

  if (state_key == Qt::NoModifier && mouse_button == Qt::RightButton)
  {
    QPoint qt_pos = pos;
    qt_pos.setY(height() - qt_pos.y() - 1);

    pos_menu_ << object_pos[0],
                    object_pos[1];

    menu_context_->popup(mapToGlobal(qt_pos));
  }
  else
  {
    pos_menu_ << -std::numeric_limits<Float>::max(),
                 -std::numeric_limits<Float>::max();
    menu_context_->hide();
  }
}

void ImageMeasureOpenGLWindow::OnActionMeasureTriggered()
{
  pos_measure_ = pos_menu_;
  pos_menu_ << -std::numeric_limits<Float>::max(),
               -std::numeric_limits<Float>::max();

  PositionContainer positions(1, pos_measure_);
  position_icon_2d_render_layer_measure_->SetPositions(positions);
  emit Measured(photo_id_, WindowPosToPhotoPos(pos_measure_));

  RenderNow();
}

void ImageMeasureOpenGLWindow::OnActionDeleteMeasureTriggered()
{
  pos_menu_ << -std::numeric_limits<Float>::max(),
               -std::numeric_limits<Float>::max();
  ClearPosition();
  emit MeasureDeleted(photo_id_);
}

ImageMeasureOpenGLWindow::ImageData
ImageMeasureOpenGLWindow::LoadIcon(const QString& icon_path)
{
  typedef ImageData::Byte Byte;
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

  return icon_image_data;
}

ImageMeasureOpenGLWindow::Position
ImageMeasureOpenGLWindow::WindowPosToPhotoPos(const Position& window_pos) const
{
  Position photo_pos = window_pos;
  photo_pos[1] = Float(image_height()) - photo_pos[1] + Float(0.5);
  return photo_pos;
}

ImageMeasureOpenGLWindow::Position
ImageMeasureOpenGLWindow::PhotoPosToWindowPos(const Position& photo_pos) const
{
  Position window_pos = photo_pos;
  window_pos[1] = Float(image_height()) - window_pos[1] + Float(0.5);
  return window_pos;
}

}
}
}
