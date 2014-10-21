#include "photos_tree_widget.hpp"

namespace hs
{
namespace recon
{

PhotosTreeWidget::PhotosTreeWidget(QWidget* parent)
  : QTreeWidget(parent),
    group_icon_(new QIcon(":/images/icon_group.png")),
    photo_icon_(new QIcon(":/images/icon_photo.png"))
{
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setExpandsOnDoubleClick(false);
  setColumnCount(4);
  QTreeWidgetItem* header_item =
    new QTreeWidgetItem(QStringList()
                        <<tr("File Name")
                        <<tr("X")
                        <<tr("Y")
                        <<tr("Z")
                        <<tr("Pitch")
                        <<tr("Roll")
                        <<tr("Heading")
                        <<tr("Projection"));
  setHeaderItem(header_item);
}

int PhotosTreeWidget::ClearGroups()
{
  return 0;
}

int PhotosTreeWidget::AddGroup(uint group_id, const GroupEntry& group)
{
  if (group_item_map_.find(group_id) != group_item_map_.end())
  {
    return -1;
  }

  QTreeWidgetItem* group_item =
    new QTreeWidgetItem(this, QStringList()<<group.name);
  group_item->setData(0, Qt::UserRole, QVariant(group_id));
  group_item->setIcon(0, *group_icon_);
  addTopLevelItem(group_item);
  group_item_map_[group_id] = group_item;
  auto itr_photo = group.photos.begin();
  auto itr_photo_end = group.photos.end();
  for (; itr_photo != itr_photo_end; ++itr_photo)
  {
    if (photo_item_map_.find(itr_photo->second.id) == photo_item_map_.end())
    {
      QString photo_item_text = itr_photo->second.file_name;
      QString x_text = QString::number(double(itr_photo->second.x));
      QString y_text = QString::number(double(itr_photo->second.y));
      QString z_text = QString::number(double(itr_photo->second.z));
      QString pitch_text = QString::number(double(itr_photo->second.pitch));
      QString roll_text = QString::number(double(itr_photo->second.roll));
      QString heading_text =
        QString::number(double(itr_photo->second.heading));
      QString projection_text = itr_photo->second.projection;
      QTreeWidgetItem* photo_item =
        new QTreeWidgetItem(
          group_item, QStringList()<<photo_item_text
                                   <<x_text
                                   <<y_text
                                   <<z_text
                                   <<pitch_text
                                   <<roll_text
                                   <<heading_text
                                   <<projection_text);
      photo_item->setData(0, Qt::UserRole, QVariant(itr_photo->second.id));
      photo_item->setIcon(0, *photo_icon_);
      group_item->addChild(photo_item);
      photo_item_map_[itr_photo->second.id] = photo_item;
    }
  }

  group_item->setExpanded(true);
  return 0;
}

int PhotosTreeWidget::AddPhotos(uint group_id, const PhotoContainer& photos)
{
  return 0;
}

int PhotosTreeWidget::DeleteGroupsByIds(const std::vector<uint>& group_ids)
{
  return 0;
}

int PhotosTreeWidget::DeleteGroupsBySelectedItems()
{
  return 0;
}

int PhotosTreeWidget::RemovePhotosByIds(const std::vector<uint>& photo_ids)
{
  return 0;
}

int PhotosTreeWidget::RemovePhotosBySelectedItems()
{
  return 0;
}

}
}
