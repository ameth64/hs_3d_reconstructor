#include "gui/block_photos_select_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlockPhotosSelectWidget::BlockPhotosSelectWidget(
  const QString& block_name, const QString& block_description,
  const GroupEntryContainer& groups)
{
  QHBoxLayout* layout_all_ = new QHBoxLayout(this);
  splitter_ = new QSplitter(this);
  layout_all_->addWidget(splitter_);

  block_info_widget_ =
    new BlockInfoWidget(block_name, block_description, splitter_);
  splitter_->addWidget(block_info_widget_);

  selected_photos_list_widget_ = new QListWidget(splitter_);
  splitter_->addWidget(selected_photos_list_widget_);

  photos_tree_widget_ = new PhotosTreeWidget(splitter_);
  auto itr_group = groups.begin();
  auto itr_group_end = groups.end();
  for (size_t i = 0; i < groups.size(); i++)
  {
    photos_tree_widget_->AddGroup(itr_group->first, itr_group->second);
  }
  splitter_->addWidget(photos_tree_widget_);
  QObject::connect(
    photos_tree_widget_, &PhotosTreeWidget::PhotoSelected
    , this, &BlockPhotosSelectWidget::SinglePhotoSelected);
  QObject::connect(photos_tree_widget_, &PhotosTreeWidget::PhotosOnlySelected
    , this, &BlockPhotosSelectWidget::PhotosOnlySelected);
}

int BlockPhotosSelectWidget::AddPhotoToSelectedWidget(uint photo_id)
{
  PhotoEntry photo_entry;
  if(photos_tree_widget_->GetPhotoEntry(photo_id, photo_entry)!=0)
    return -1;
  selected_photos_list_widget_->addItem(new QListWidgetItem(photo_entry.file_name));
  return 0;
}

void BlockPhotosSelectWidget::SinglePhotoSelected(uint photo_id)
{
  selected_photos_list_widget_->clear();
  AddPhotoToSelectedWidget(photo_id);
}

void BlockPhotosSelectWidget::PhotosOnlySelected(const std::vector<uint>& photo_ids)
{
  selected_photos_list_widget_->clear();
  for (size_t i = 0; i != photo_ids.size(); ++i)
  {
    AddPhotoToSelectedWidget(photo_ids[i]);
  }
}

}
}
}
