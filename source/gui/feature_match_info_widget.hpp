#ifndef _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_INFO_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_FEATURE_MATCH_INFO_WIDGET_HPP_

#include <map>

#include <QTableWidget>
#include <QIcon>

namespace hs
{
namespace recon
{

class FeatureMatchInfoWidget : public QTableWidget
{
  Q_OBJECT
public:
  struct PhotoFeatureEntry
  {
    uint photo_id;
    QString photo_name;
    uint number_of_keypoints;
    uint number_of_matches;
  };
  typedef std::map<uint, PhotoFeatureEntry> PhotoFeatureEntryContainer;
  typedef std::map<uint, int> RowMap;
  FeatureMatchInfoWidget(
    const PhotoFeatureEntryContainer& photo_feature_entries =
      PhotoFeatureEntryContainer(),
    QWidget* parent = 0);

private:
  QIcon photo_icon_;
  RowMap row_map_;
};

}
}

#endif
