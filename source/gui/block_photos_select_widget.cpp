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

}

}
}
}
