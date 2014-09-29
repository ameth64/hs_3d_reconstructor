#ifndef _HS_3D_RECONSTRUCTOR_PHOTOS_SELELCT_WINDOW_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTOS_SELELCT_WINDOW_HPP_

#include "opengl_window.hpp"

namespace hs
{
namespace recon
{

class PhotosSelectWindow : public OpenGLWindow
{
  Q_OBJECT
public:
  typedef double Float;
  struct PhotoEntry
  {
    uint id;
    uint block_id;
    uint group_id;
    QString file_name;
    Float position[3];
    Float rotation[3];
  };
  enum ColouringMode
  {
    NO_COLORING = 0,
    BY_BLOCKS,
    BY_GROUPS
  };
  typedef std::map<uint, PhotoEntry> PhotoContainer;
  PhotosSelectWindow(QWindow* parent, const PhotoContainer& photos);
  uint excluded_block_id() const;
  ColouringMode colouring_mode() const;
  void SetColouringMode(ColouringMode colouring_mode);
  PhotoContainer GetSelectedPhotos();

public:
  ColouringMode colouring_mode_;
  uint excluded_block_id_;
};

}
}

#endif
