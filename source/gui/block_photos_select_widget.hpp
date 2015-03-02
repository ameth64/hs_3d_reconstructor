#ifndef _HS_3D_RECONSTRUCTOR_GUI_BLOCK_PHOTOS_SELECT_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_BLOCK_PHOTOS_SELECT_WIDGET_HPP_

#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

#include "gui/block_info_widget.hpp"
#include "gui/photos_tree_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class BlockPhotosSelectWidget : public QWidget
{
  Q_OBJECT
public:
  typedef PhotosTreeWidget::PhotoEntry PhotoEntry;
  typedef PhotosTreeWidget::GroupEntry GroupEntry;
  typedef std::map<uint, GroupEntry> GroupEntryContainer;
  typedef BlockInfoWidget::BlockInfoEntry BlockInfoEntry;

public:
  BlockPhotosSelectWidget(const BlockInfoEntry& block_info_entry,
                          const GroupEntryContainer& groups);

  BlockInfoEntry GetBlockInfo() const;

  void GetSelectedPhotoIds(std::vector<uint>& selected_photo_ids) const;

private slots:
  void OnSinglePhotoSelected(uint photo_id);
  void OnPhotosOnlySelected(const std::vector<uint>& photo_ids);
  void OnPhotosAndGroupsSelected(const std::vector<uint>& group_ids,
                                 const std::vector<uint>& photo_ids);

private:
  int AddPhotoToSelectedWidget(uint photo_id);

private:
  BlockInfoWidget* block_info_widget_;
  PhotosTreeWidget* photos_tree_widget_;
  QListWidget* selected_photos_list_widget_;

  QHBoxLayout* layout_all_;
  QSplitter* splitter_;
};

}
}
}

#endif
