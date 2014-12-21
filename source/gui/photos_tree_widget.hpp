#ifndef _HS_3D_RECONSTRUCTOR_PHOTOS_TREE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTOS_TREE_WIDGET_HPP_

#include <map>

#include <QTreeWidget>
#include <QIcon>

namespace hs
{
namespace recon
{

class PhotosTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  typedef double Float;
  struct PhotoEntry
  {
    uint id;
    QString file_name;
    Float x;
    Float y;
    Float z;
    Float pitch;
    Float roll;
    Float heading;
    QString projection;
  };
  typedef std::map<uint, PhotoEntry> PhotoContainer;
  struct GroupEntry
  {
    uint id;
    QString name;
    PhotoContainer photos;
  };

  typedef std::vector<QTreeWidgetItem> ItemVector;
  typedef std::map<uint, QTreeWidgetItem*> ItemMap;

public:
  PhotosTreeWidget(QWidget* parent = 0);
  int ClearGroups();
  int AddGroup(uint group_id, const GroupEntry& group);
  int AddPhotos(uint group_id, const PhotoContainer& photos);

public slots:
  int DeleteGroupsByIds(const std::vector<uint>& group_ids);
  int DeleteGroupsBySelectedItems();
  int RemovePhotosByIds(const std::vector<uint>& photo_ids);
  int RemovePhotosBySelectedItems();

signals:
  void SelectedGroupsDeleted(const std::vector<uint>& group_ids);
  void SelectedPhotosRemoved(const std::vector<uint>& photo_ids);
  void PhotoSelected(uint photo_id);
  void GroupsOnlySelected(const std::vector<uint>& block_ids);
  void SingleGroupSelected(uint group_id);
  void PhotosOnlySelected(const std::vector<uint>& photo_ids);

private:
  ItemMap group_item_map_;
  ItemMap photo_item_map_;

  QIcon* group_icon_;
  QIcon* photo_icon_;
};

}
}

#endif
