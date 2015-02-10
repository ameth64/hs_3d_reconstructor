#include "photos_tree_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
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

  QObject::connect(this, &QTreeWidget::itemSelectionChanged,
                   this, &PhotosTreeWidget::OnItemSelectionChanged);
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
  Float invalid_value = -std::numeric_limits<Float>::max();
  for (; itr_photo != itr_photo_end; ++itr_photo)
  {
    if (photo_item_map_.find(itr_photo->second.id) == photo_item_map_.end())
    {
      QString photo_item_text = itr_photo->second.file_name;
      QString x_text;
      if (itr_photo->second.x != invalid_value)
      {
        x_text = QString::number(double(itr_photo->second.x), 'f', 4);
      }
      QString y_text;
      if (itr_photo->second.y != invalid_value)
      {
        y_text = QString::number(double(itr_photo->second.y), 'f', 4);
      }
      QString z_text;
      if (itr_photo->second.z != invalid_value)
      {
        z_text = QString::number(double(itr_photo->second.z), 'f', 4);
      }
      QString pitch_text;
      if (itr_photo->second.pitch != invalid_value)
      {
        pitch_text = QString::number(double(itr_photo->second.pitch), 'f', 4);
      }
      QString roll_text;
      if (itr_photo->second.roll != invalid_value)
      {
        roll_text = QString::number(double(itr_photo->second.roll), 'f', 4);
      }
      QString heading_text;
      if (itr_photo->second.heading != invalid_value)
      {
        heading_text =
          QString::number(double(itr_photo->second.heading), 'f', 4);
      }
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
      photo_item_map_[itr_photo->first] = photo_item;
    }
  }

  group_item->setExpanded(true);
  return 0;
}

int PhotosTreeWidget::AddPhotos(uint group_id, const PhotoContainer& photos)
{
  auto itr_group = group_item_map_.find(group_id);
  if (itr_group == group_item_map_.end()) return -1;
  auto itr_photo = photos.begin();
  auto itr_photo_end = photos.end();
  Float invalid_value = -std::numeric_limits<Float>::max();
  for (; itr_photo != itr_photo_end; ++itr_photo)
  {
    if (photo_item_map_.find(itr_photo->first) != photo_item_map_.end())
    {
      continue;
    }
    QString photo_item_text = itr_photo->second.file_name;
    QString x_text;
    Float x = itr_photo->second.x;
    if (itr_photo->second.x != invalid_value)
    {
      x_text = QString::number(double(itr_photo->second.x), 'f', 4);
    }
    QString y_text;
    if (itr_photo->second.x != invalid_value)
    {
      y_text = QString::number(double(itr_photo->second.y), 'f', 4);
    }
    QString z_text;
    if (itr_photo->second.x != invalid_value)
    {
      z_text = QString::number(double(itr_photo->second.z), 'f', 4);
    }
    QString pitch_text;
    if (itr_photo->second.x != invalid_value)
    {
      pitch_text = QString::number(double(itr_photo->second.pitch), 'f', 4);
    }
    QString roll_text;
    if (itr_photo->second.x != invalid_value)
    {
      roll_text = QString::number(double(itr_photo->second.roll), 'f', 4);
    }
    QString heading_text;
    if (itr_photo->second.x != invalid_value)
    {
      heading_text =
        QString::number(double(itr_photo->second.heading), 'f', 4);
    }
    QString projection_text = itr_photo->second.projection;
    QTreeWidgetItem* photo_item =
      new QTreeWidgetItem(
        itr_group->second, QStringList()<<photo_item_text
                                        <<x_text
                                        <<y_text
                                        <<z_text
                                        <<pitch_text
                                        <<roll_text
                                        <<heading_text
                                        <<projection_text);
    photo_item->setData(0, Qt::UserRole, QVariant(itr_photo->second.id));
    photo_item->setIcon(0, *photo_icon_);
    itr_group->second->addChild(photo_item);
    photo_item_map_[itr_photo->first] = photo_item;
  }
  return 0;
}

int PhotosTreeWidget::DeleteGroupsByIds(const std::vector<uint>& group_ids)
{
  auto itr_group_id = group_ids.begin();
  auto itr_group_id_end = group_ids.end();
  for (; itr_group_id != itr_group_id_end; ++itr_group_id)
  {
    auto itr_group = group_item_map_.find(*itr_group_id);
    if (itr_group == group_item_map_.end())
    {
      continue;
    }
    //删除照片组包含的所有图片
    int child_count = itr_group->second->childCount();
    for (int i = 0; i < child_count; i++)
    {
      QTreeWidgetItem* photo_item = itr_group->second->child(i);
      uint photo_id = photo_item->data(0, Qt::UserRole).toUInt();
      auto itr_photo_item = photo_item_map_.find(photo_id);
      if (itr_photo_item == photo_item_map_.end() ||
          itr_photo_item->second != photo_item) return -1;
      photo_item_map_.erase(itr_photo_item);
    }
    delete itr_group->second;
    group_item_map_.erase(itr_group);
  }
  return 0;
}

int PhotosTreeWidget::DeleteGroupsBySelectedItems()
{
  QList<QTreeWidgetItem*> selected_items = selectedItems();
  QList<QTreeWidgetItem*> remove_items;
  auto itr_selected_item = selected_items.begin();
  auto itr_selected_item_end = selected_items.end();
  for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
  {
    uint group_id = (*itr_selected_item)->data(0, Qt::UserRole).toUInt();
    auto itr_group_item = group_item_map_.find(group_id);
    if (itr_group_item != group_item_map_.end() &&
        itr_group_item->second == *itr_selected_item)
    {
      remove_items.push_back(*itr_selected_item);
    }
  }
  if (remove_items == selected_items)
  {
    itr_selected_item = selected_items.begin();
    itr_selected_item_end = selected_items.end();
    std::vector<uint> group_ids;
    for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
    {
      uint group_id = (*itr_selected_item)->data(0, Qt::UserRole).toUInt();
      auto itr_group_item = group_item_map_.find(group_id);
      group_item_map_.erase(itr_group_item);
      delete *itr_selected_item;
      group_ids.push_back(group_id);
    }
    emit SelectedGroupsDeleted(group_ids);
    return 0;
  }
  else
  {
    return -1;
  }
}

int PhotosTreeWidget::RemovePhotosByIds(const std::vector<uint>& photo_ids)
{
  //给定的照片全部存在才会删除
  std::vector<QTreeWidgetItem*> remove_items;
  auto itr_photo_id = photo_ids.begin();
  auto itr_photo_id_end = photo_ids.end();
  for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
  {
    auto itr_photo_item = photo_item_map_.find(*itr_photo_id);
    if (itr_photo_item != photo_item_map_.end())
    {
      remove_items.push_back(itr_photo_item->second);
    }
  }
  if (remove_items.size() == photo_ids.size())
  {
    itr_photo_id = photo_ids.begin();
    itr_photo_id_end = photo_ids.end();
    for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
    {
      auto itr_photo_item = photo_item_map_.find(*itr_photo_id);
      if (itr_photo_item != photo_item_map_.end())
      {
        photo_item_map_.erase(itr_photo_item);
        delete itr_photo_item->second;
      }
    }
    return 0;
  }
  else
  {
    return -1;
  }
}

int PhotosTreeWidget::RemovePhotosBySelectedItems()
{
  QList<QTreeWidgetItem*> selected_items = selectedItems();
  QList<QTreeWidgetItem*> remove_items;
  auto itr_selected_item = selected_items.begin();
  auto itr_selected_item_end = selected_items.end();
  for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
  {
    uint photo_id = (*itr_selected_item)->data(0, Qt::UserRole).toUInt();
    auto itr_photo_item = photo_item_map_.find(photo_id);
    if (itr_photo_item != photo_item_map_.end() &&
        itr_photo_item->second == *itr_selected_item)
    {
      remove_items.push_back(*itr_selected_item);
    }
  }
  if (remove_items == selected_items)
  {
    itr_selected_item = selected_items.begin();
    itr_selected_item_end = selected_items.end();
    std::vector<uint> photo_ids;
    for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
    {
      uint photo_id = (*itr_selected_item)->data(0, Qt::UserRole).toUInt();
      auto itr_photo_item = photo_item_map_.find(photo_id);
      photo_item_map_.erase(itr_photo_item);
      delete *itr_selected_item;
      photo_ids.push_back(photo_id);
    }
    emit SelectedPhotosRemoved(photo_ids);
    return 0;
  }
  else
  {
    return -1;
  }
  return 0;
}

void PhotosTreeWidget::OnItemSelectionChanged()
{
  auto selected_items = selectedItems();
  if (selected_items.empty())
  {
    emit NothingSelected();
    return;
  }
  QList<QTreeWidgetItem*> selected_group_items;
  QList<QTreeWidgetItem*> selected_photo_items;
  auto itr_selected_item = selected_items.begin();
  auto itr_selected_item_end = selected_items.end();
  for (; itr_selected_item != itr_selected_item_end; ++itr_selected_item)
  {
    //有父亲项的为照片项，否则为照片组项
    if ((*itr_selected_item)->parent() != nullptr)
    {
      selected_photo_items.push_back(*itr_selected_item);
    }
    else
    {
      selected_group_items.push_back(*itr_selected_item);
    }
  }
  //if (selected_group_items.empty() && selected_photo_items.size() == 1)
  //{
  //  QTreeWidgetItem* item = *(selected_photo_items.begin());
  //  emit PhotoSelected(item->data(0, Qt::UserRole).toUInt());
  //}
  //else if (selected_group_items.size() > 1 && selected_photo_items.empty())
  //{
  //  auto itr_group_item = selected_group_items.begin();
  //  auto itr_group_item_end = selected_group_items.end();
  //  std::vector<uint> group_ids;
  //  for (; itr_group_item != itr_group_item_end; ++itr_group_item)
  //  {
  //    group_ids.push_back((*itr_group_item)->data(0, Qt::UserRole).toUInt());
  //  }
  //  emit GroupsOnlySelected(group_ids);
  //}
  //else if (selected_group_items.size() == 1 && selected_photo_items.empty())
  //{
  //  QTreeWidgetItem* item = *(selected_group_items.begin());
  //  emit SingleGroupSelected(item->data(0, Qt::UserRole).toUInt());
  //}
  //else if (selected_group_items.empty() && selected_photo_items.size() > 1)
  //{
  //  auto itr_photo_item = selected_photo_items.begin();
  //  auto itr_photo_item_end = selected_photo_items.end();
  //  std::vector<uint> photo_ids;
  //  for (; itr_photo_item != itr_photo_item_end; ++itr_photo_item)
  //  {
  //    photo_ids.push_back((*itr_photo_item)->data(0, Qt::UserRole).toUInt());
  //  }
  //  emit PhotosOnlySelected(photo_ids);
  //}
  //else
  //{
  //  emit PhotosAndGroupsSelected();
  //}

  auto itr_selected_photo_items = selected_photo_items.begin();
  auto itr_selected_photo_items_end = selected_photo_items.end();
  std::vector<uint> photo_ids;
  for (; itr_selected_photo_items != itr_selected_photo_items_end
    ; ++itr_selected_photo_items)
  {
    photo_ids.push_back((*itr_selected_photo_items)->data(0, Qt::UserRole).toUInt());
  }

  auto itr_selected_group_items = selected_group_items.begin();
  auto itr_selected_group_items_end = selected_group_items.end();
  std::vector<uint> group_ids;
  for (; itr_selected_group_items != itr_selected_group_items_end
    ; ++itr_selected_group_items)
  {
    group_ids.push_back((*itr_selected_group_items)->data(0, Qt::UserRole).toUInt());
  }
  if (group_ids.empty() && photo_ids.size() == 1)
  {
    emit PhotoSelected(photo_ids[0]);
  }
  else if (group_ids.empty() && photo_ids.size() > 1)
  {
    emit PhotosOnlySelected(photo_ids);
  }
  else if (group_ids.size() == 1 && photo_ids.empty())
  {
    emit SingleGroupSelected(group_ids[0]);
  }
  else if (group_ids.size() > 1 && photo_ids.empty())
  {
    emit GroupsOnlySelected(group_ids);
  }
  else
  {
    emit PhotosAndGroupsSelected(group_ids, photo_ids);
  }
}

int PhotosTreeWidget::GetPhotoEntry(uint photo_id, PhotoEntry& photo_entry)
{
  auto itr_photo_item = photo_item_map_.find(photo_id);
  if (itr_photo_item == photo_item_map_.end())
  {
    return -1;
  }
  photo_entry.id = photo_id;
  photo_entry.file_name = itr_photo_item->second->text(0);
  photo_entry.x = itr_photo_item->second->text(1).toFloat();
  photo_entry.y = itr_photo_item->second->text(2).toFloat();
  photo_entry.z = itr_photo_item->second->text(3).toFloat();
  photo_entry.pitch = itr_photo_item->second->text(4).toFloat();
  photo_entry.roll = itr_photo_item->second->text(5).toFloat();
  photo_entry.heading = itr_photo_item->second->text(6).toFloat();
  photo_entry.projection = itr_photo_item->second->text(7);
  return 0;
}

}
}
}
