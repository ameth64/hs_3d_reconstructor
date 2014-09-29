#ifndef _HS_3D_RECONSTRUCTOR_GCP_MARK_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GCP_MARK_WIDGET_HPP_

#include <vector>

#include "hs_image_io/whole_io/image_data.hpp"

namespace hs
{
namespace recon
{

/**
 *  该Widget显示与像控点相关联的照片。
 *  有两种显示方式，
 *  一种是缩略图列表显示，此时只显示照片的缩略图，不可对照片作放大缩小等操作；
 *  另一种是快速标记模式，此时显示照片一定范围内的准确像素，可放大缩小平移。
 */
class GCPMarkWidget : public QWidget
{
  Q_OBJECT
public:
  typedef float Float;
  typedef hs::imgio::whole::ImageData ImageData;
  enum DisplayMode
  {
    THUMBNAIL_LIST,
    RAPID_MARK
  };

  struct ImagePos
  {
    enum State
    {
      PREDICATED,
      MEASURED
    };
    Float center[2];
  };
  typedef std::vector<ImagePos> ImagePosContainer;

  GCPMarkWidget(QWidget* parent = 0);
  uint gcp_id() const;
  DisplayMode current_display_mode() const;
  uint interest_range() const;
  int DisplayThumbnailList(const QStringList& photo_names,
                           const std::vector<ImageData>& thumbnails,
                           const std::vector<uint>& photo_ids);
  int StartDisplayRapidMark(const QStringList& photo_names,
                            const std::vector<ImageData>& thumbnails,
                            const std::vector<uint>& photo_ids,
                            const ImagePosContainer& centers);
  int DisplayRapidMarkPhoto(uint photo_id, const ImageData& photo);

signals:
  void GCPMarkerChanged(uint gcp_id, uint photo_id,
                        const ImagePos& pos_old, const ImagePos& pos_new);

private:
  uint gcp_id_;
  DisplayMode current_display_mode_;
  uint interest_range_;
};

}
}

#endif
