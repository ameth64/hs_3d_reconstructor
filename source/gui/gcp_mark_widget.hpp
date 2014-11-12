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
 *  有三种显示方式：
 *  1.缩略图列表模式，此时只显示照片的缩略图，不可对照片作放大缩小平移等操作；
 *  2.快速标记模式，此时显示照片一定范围内的准确像素，可放大缩小至一定程度；
 *  3.误差列表模式，此时显示各张照片测量的重投影误差。
 */
class GCPMarkWidget : public QWidget
{
  Q_OBJECT
public:
  typedef float Float;
  typedef hs::imgio::whole::ImageData ImageData;
  enum DisplayMode
  {
    THUMBNAIL_TABLE,
    RAPID_MARK,
    ERROR_TABLE
  };

  struct ImagePos
  {
    enum State
    {
      PREDICATED = 0,
      MEASURED,
      NUMBER_OF_STATE
    };
    typedef std::bitset<NUMBER_OF_STATE> mask;
    Float predicated_pos_[2];
    Float measured_pos_[2];
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
