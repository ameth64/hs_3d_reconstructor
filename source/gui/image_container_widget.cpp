#include "gui/image_container_widget.hpp"

namespace hs
{
namespace recon
{

ImageContainerWidget::ImageContainerWidget(ImageOpenGLWindow* image_window,
                                           const QString& comment,
                                           QWidget* parent)
  : QWidget(parent)
  , image_window_(image_window)
  , comment_label_(nullptr)
  , layout_(new QVBoxLayout(this))
{
  layout_->setContentsMargins(0, 0, 0, 0);
  container_ = QWidget::createWindowContainer(image_window_, parent);
  container_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Expanding));
  layout_->addWidget(container_);
  comment_label_ = new QLabel(comment, this);
  comment_label_->setAlignment(Qt::AlignCenter);
  comment_label_->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                            QSizePolicy::Minimum));
  layout_->addWidget(comment_label_);
}

ImageOpenGLWindow* ImageContainerWidget::image_window()
{
  return image_window_;
}

QString ImageContainerWidget::comment() const
{
  return comment_label_->text();
}

void ImageContainerWidget::SetComment(const QString& comment)
{
  comment_label_->setText(comment);
}

}
}
