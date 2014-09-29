#include <limits>

#include <QHeaderView>
#include <QFont>

#include <QMessageBox>

#include "blocks_tree_widget.hpp"

namespace hs
{
namespace recon
{

BlocksTreeWidget::BlocksTreeWidget(QWidget* parent)
  : QTreeWidget(parent),
    activated_block_id_(std::numeric_limits<uint>::max()),
    photo_icon_(new QIcon(":/images/icon_photo.png")),
    valid_icon_(new QIcon(":/images/icon_valid.png")),
    invalid_icon_(new QIcon(":/images/icon_invalid.png"))
{
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setExpandsOnDoubleClick(false);

  setColumnCount(4);
  QTreeWidgetItem* header_item =
    new QTreeWidgetItem(QStringList()
                          <<tr("")
                          <<tr("Position")
                          <<tr("Rotation")
                          <<tr("Oriented"));
  setHeaderItem(header_item);
  workspace_item_ =
    new QTreeWidgetItem(this, QStringList()<<tr("Workspace"));
  workspace_item_->setExpanded(true);
  addTopLevelItem(workspace_item_);

  QObject::connect(this, &QTreeWidget::itemDoubleClicked,
                   this, &BlocksTreeWidget::OnItemDoubleClicked);
}

int BlocksTreeWidget::ClearBlocks()
{
  return 0;
}

int BlocksTreeWidget::AddBlock(uint block_id, const BlockEntry& block)
{
  if (block_item_map_.find(block_id) != block_item_map_.end())
  {
    return -1;
  }
  QTreeWidgetItem* block_item =
    new QTreeWidgetItem(workspace_item_, QStringList()<<block.name);
  block_item->setData(0, Qt::UserRole, QVariant(block_id));
  workspace_item_->addChild(block_item);
  block_item_map_[block_id] = block_item;
  auto itr_photo = block.photos.begin();
  auto itr_photo_end = block.photos.end();
  for (; itr_photo != itr_photo_end; ++itr_photo)
  {
    if (photo_item_map_.find(itr_photo->second.id) == photo_item_map_.end())
    {
      QString photo_item_text = itr_photo->second.file_name;
      QTreeWidgetItem* photo_item =
        new QTreeWidgetItem(block_item, QStringList()<<photo_item_text
                                                     <<""<<""<<"");
      photo_item->setData(0, Qt::UserRole, QVariant(itr_photo->second.id));
      photo_item->setIcon(0, *photo_icon_);
      if (itr_photo->second.mask[PhotoEntry::POSITION])
      {
        photo_item->setIcon(1, *valid_icon_);
      }
      else
      {
        photo_item->setIcon(1, *invalid_icon_);
      }
      if (itr_photo->second.mask[PhotoEntry::ROTATION])
      {
        photo_item->setIcon(2, *valid_icon_);
      }
      else
      {
        photo_item->setIcon(2, *invalid_icon_);
      }
      if (itr_photo->second.mask[PhotoEntry::ORIENTED])
      {
        photo_item->setIcon(3, *valid_icon_);
      }
      else
      {
        photo_item->setIcon(3, *invalid_icon_);
      }
      block_item->addChild(photo_item);
      photo_item_map_[itr_photo->second.id] = photo_item;
    }
  }

  block_item->setExpanded(true);
  return 0;
}

int BlocksTreeWidget::AddPhotos(uint block_id, const PhotoContainer& photos)
{
  return 0;
}

uint BlocksTreeWidget::activated_block_id() const
{
  return activated_block_id_;
}

int BlocksTreeWidget::DeleteBlocksByIds(const std::vector<uint>& block_ids)
{
  return 0;
}

int BlocksTreeWidget::DeleteBlocksBySelectedItems()
{
  return 0;
}

int BlocksTreeWidget::ActivateBlockById(uint block_id)
{
  return 0;
}

int BlocksTreeWidget::RemovePhotosByIds(const std::vector<uint> photo_ids)
{
  return 0;
}

int BlocksTreeWidget::RemovePhotosBySelectedItems()
{
  return 0;
}

void BlocksTreeWidget::OnItemSelectionChanged()
{

}

void BlocksTreeWidget::OnItemDoubleClicked(QTreeWidgetItem* item, int column)
{
  QMessageBox msg_box;
  msg_box.setText(tr("Double Clicked!"));
  msg_box.exec();
}

int BlocksTreeWidget::DeleteBlocksInternal(const ItemVector& block_items)
{
  return 0;
}

int BlocksTreeWidget::RemovePhotosInternal(const ItemVector& photo_items)
{
  return 0;
}

}
}
