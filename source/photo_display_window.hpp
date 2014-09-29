#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_DISPLAY_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_DISPLAY_WINDOW_HPP_

#include "opengl_window.hpp"

#include "hs_image_io/whole_io/image_data.hpp"

namespace hs
{
namespace recon
{

class PhotoDisplayWindow : public OpenGLWindow
{
  Q_OBJECT
public:
  typedef float Float;
  typedef hs::imgio::whole::ImageData ImageData;
  PhotoDisplayWindow(QWindow* parent = 0);

public:
  struct GCPPos
  {
    uint id;
    QString name;
    Float pos[2];
  };
  int DisplayPhoto(const QString& photo_path,
                   const std::vector<ImageData>& thumbnails,
                   const std::vector<ImagePos>& gcp_poss);

public slots:
  int GCPPosChange(uint id, Float new_pos);
};

}
}

#endif
