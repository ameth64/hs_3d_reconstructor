#ifndef _HS_3D_RECONSTRUCTOR_IMAGE_CONTAINER_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_IMAGE_CONTAINER_WIDGET_HPP_

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "gui/image_opengl_window.hpp"

namespace hs
{
namespace recon
{

class ImageContainerWidget : public QWidget
{
  Q_OBJECT
public:
  ImageContainerWidget(ImageOpenGLWindow* image_window,
                       const QString& comment = QString(),
                       QWidget* parent = 0);

  ImageOpenGLWindow* image_window();
  QString comment() const;
  void SetComment(const QString& comment);

private:
  ImageOpenGLWindow* image_window_;
  QWidget* container_;
  QLabel* comment_label_;
  QVBoxLayout* layout_;
};

}
}

#endif
