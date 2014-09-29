#ifndef _HS_3D_RECONSTRUCTOR_BLOCKS_TREE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_BLOCKS_TREE_WIDGET_HPP_

#include <bitset>
#include <map>
#include <vector>

#include <QTreeWidget>
#include <QIcon>
#include <QMenu>
#include <QAction>

namespace hs
{
namespace recon
{

class BlocksTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  struct PhotoEntry
  {
    enum PhotoStatus
    {
      NOT_AT_ALL = 0,
      ORIENTED,
      POSITION,
      ROTATION,
      NUMBER_OF_PHOTO_STATUS
    };
    typedef std::bitset<NUMBER_OF_PHOTO_STATUS> PhotoMask;
    uint id;
    QString file_name;
    PhotoMask mask;
  };
  typedef std::map<uint, PhotoEntry> PhotoContainer;
  struct BlockEntry
  {
    uint id;
    QString name;
    PhotoContainer photos;
  };

  typedef std::vector<QTreeWidgetItem*> ItemVector;
  typedef std::map<uint, QTreeWidgetItem*> ItemMap;
  typedef std::pair<uint, std::vector<uint> > BlockPhotosPair;
  typedef std::vector<BlockPhotosPair> BlockPhotosPairContainer;

public:
  BlocksTreeWidget(QWidget* parent = 0);
  int ClearBlocks();
  int AddBlock(uint block_id, const BlockEntry& block);
  int AddPhotos(uint block_id, const PhotoContainer& photos);
  uint activated_block_id() const;

public slots:
  int DeleteBlocksByIds(const std::vector<uint>& block_ids);
  int DeleteBlocksBySelectedItems();
  int ActivateBlockById(uint block_id);
  int RemovePhotosByIds(const std::vector<uint> photo_ids);
  int RemovePhotosBySelectedItems();

protected slots:
  void OnItemSelectionChanged();
  void OnItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
  int DeleteBlocksInternal(const ItemVector& block_items);
  int RemovePhotosInternal(const ItemVector& photo_items);

signals:
  void SeletedBlocksDeleted(const std::vector<uint>& block_ids);
  void SeletedBlockActivated(uint block_id);
  void SeletedPhotosRemoved(
    const BlockPhotosPairContainer& block_photos_pairs);
  void PhotoSelected(uint photo_id);
  void BlocksOnlySelected(const std::vector<uint>& block_ids);
  void SingleBlockSelected(uint block_id);
  void PhotosOnlySelected(const std::vector<uint>& photo_ids);

private:
  ItemMap block_item_map_;
  ItemMap photo_item_map_;

  uint activated_block_id_;

  QIcon* photo_icon_;
  QIcon* valid_icon_;
  QIcon* invalid_icon_;

  QTreeWidgetItem* workspace_item_;
};

}
}

#endif
