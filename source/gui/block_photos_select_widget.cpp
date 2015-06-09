#include "gui/block_photos_select_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlockPhotosSelectWidget::BlockPhotosSelectWidget(
  const BlockInfoEntry& block_info_entry, const GroupEntryContainer& groups)
{
  setMinimumSize(800, 600);
  QHBoxLayout* layout_all_ = new QHBoxLayout(this);
  splitter_ = new QSplitter(this);
  layout_all_->addWidget(splitter_);

  block_info_widget_ =
    new BlockInfoWidget(block_info_entry, splitter_);
  splitter_->addWidget(block_info_widget_);

  selected_photos_list_widget_ = new QListWidget(splitter_);
  splitter_->addWidget(selected_photos_list_widget_);

  photos_tree_widget_ = new PhotosTreeWidget(splitter_);
  photos_tree_widget_->setColumnWidth(0, 300);
  auto itr_group = groups.begin();
  auto itr_group_end = groups.end();
  for (; itr_group != itr_group_end;++itr_group)
  {
    photos_tree_widget_->AddGroup(itr_group->first, itr_group->second);
  }
  splitter_->addWidget(photos_tree_widget_);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::PhotoSelected,
    this, &BlockPhotosSelectWidget::OnSinglePhotoSelected);
  QObject::connect(photos_tree_widget_, &PhotosTreeWidget::PhotosOnlySelected,
                   this, &BlockPhotosSelectWidget::OnPhotosOnlySelected);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::PhotosAndGroupsSelected,
    this, &BlockPhotosSelectWidget::OnPhotosAndGroupsSelected);
}

int BlockPhotosSelectWidget::AddPhotoToSelectedWidget(uint photo_id)
{
  PhotoEntry photo_entry;
  if(photos_tree_widget_->GetPhotoEntry(photo_id, photo_entry)!=0)
    return -1;
  QListWidgetItem* item = new QListWidgetItem(photo_entry.file_name);
  item->setData(Qt::UserRole, QVariant(photo_id));
  selected_photos_list_widget_->addItem(item);
  return 0;
}

void BlockPhotosSelectWidget::OnSinglePhotoSelected(uint photo_id)
{
  selected_photos_list_widget_->clear();
  AddPhotoToSelectedWidget(photo_id);
}

void BlockPhotosSelectWidget::OnPhotosOnlySelected(
  const std::vector<uint>& photo_ids)
{
  selected_photos_list_widget_->clear();
  for (size_t i = 0; i != photo_ids.size(); ++i)
  {
    AddPhotoToSelectedWidget(photo_ids[i]);
  }
}

void BlockPhotosSelectWidget::OnPhotosAndGroupsSelected(
  const std::vector<uint>& group_ids,
  const std::vector<uint>& photo_ids)
{
  selected_photos_list_widget_->clear();
  for (size_t i = 0; i != photo_ids.size(); ++i)
  {
    AddPhotoToSelectedWidget(photo_ids[i]);
  }
}

BlockPhotosSelectWidget::BlockInfoEntry
BlockPhotosSelectWidget::GetBlockInfo() const
{
  return block_info_widget_->GetBlockInfo();
}

void BlockPhotosSelectWidget::GetSelectedPhotoIds(
  std::vector<uint>& selected_photo_ids) const
{
  selected_photo_ids.clear();
  for (int i = 0; i < selected_photos_list_widget_->count(); i++)
  {
    QListWidgetItem* item = selected_photos_list_widget_->item(i);
    uint id = item->data(Qt::UserRole).toUInt();
    selected_photo_ids.push_back(id);
  }
}

}
}
}
