#include <QMessageBox>

#include "gui/gcps_table_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

QStringList GCPsTableWidget::HeaderLabels()
{
  return QStringList() << tr("Name")
                       << tr("Coordinate X")
                       << tr("Coordinate Y")
                       << tr("Coordinate Z")
                       << tr("Type");
}

QStringList GCPsTableWidget::TypeLabels()
{
  return QStringList() << tr("Control Point")
                       << tr("Check Point")
                       << tr("Not Used");
}

GCPsTableWidget::GCPsTableWidget(QWidget* parent)
  : QTableWidget(parent)
  , display_mode_(SHOW_MEASUREMENT)
  , combo_box_row_(-1)
  , is_refresh_complete_(true)
{
  QStringList header_labels = HeaderLabels();
  setColumnCount(int(header_labels.size()));
  setHorizontalHeaderLabels(header_labels);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  QObject::connect(this, &QTableWidget::cellClicked,
                   this, &GCPsTableWidget::OnCellClicked);
  QObject::connect(this, &QTableWidget::itemSelectionChanged,
                   this, &GCPsTableWidget::OnItemSelectionChanged);
  QObject::connect(this, &QTableWidget::itemChanged,
                   this, &GCPsTableWidget::OnItemChanged);
}

GCPsTableWidget::DisplayMode
GCPsTableWidget::display_mode() const
{
  return DisplayMode(display_mode_);
}

void GCPsTableWidget::set_display_mode(DisplayMode display_mode)
{
  display_mode_ = display_mode;
  RefreshTable();
}

void GCPsTableWidget::ClearGCPs()
{
  gcps_.clear();
  while (rowCount() > 0)
  {
    removeRow(0);
  }
}

void GCPsTableWidget::AddGCPs(const GCPContainer& gcps)
{
  auto itr_gcp = gcps.begin();
  auto itr_gcp_end = gcps.end();
  for (; itr_gcp != itr_gcp_end; ++itr_gcp)
  {
    if (gcps_.find(itr_gcp->first) == gcps_.end())
    {
      gcps_[itr_gcp->first] = itr_gcp->second;
      gcp_type_editable_map_[itr_gcp->first] = false;
    }
  }
  RefreshTable();
}

void GCPsTableWidget::SetGCPType(uint gcp_id, int type)
{
  auto itr_gcp = gcps_.find(gcp_id);
  if (itr_gcp != gcps_.end())
  {
    itr_gcp->second.type = type;
    RefreshTable();
  }
}

int GCPsTableWidget::DeleteGCPsByIds(const std::vector<uint>& group_ids)
{
  return 0;
}

int GCPsTableWidget::DeleteGCPsBySelectedItems()
{
  return 0;
}

int GCPsTableWidget::UpdateGCPEstimatePos(uint gcp_id,
                                          Float x, Float y, Float z)
{
  auto itr_gcp = gcps_.find(gcp_id);
  if (itr_gcp == gcps_.end())
  {
    return -1;
  }
  auto itr_row = gcp_row_map_.find(gcp_id);
  if (itr_row == gcp_row_map_.end())
  {
    return -1;
  }
  itr_gcp->second.estimate_pos[0] = x;
  itr_gcp->second.estimate_pos[1] = y;
  itr_gcp->second.estimate_pos[2] = z;
  if (display_mode_ == SHOW_ESTIMATE)
  {
    QString x_text;
    if (x > -1e100)
    {
      x_text = QString::number(x, 'f', 6);
    }
    QString y_text;
    if (y > -1e100)
    {
      y_text = QString::number(y, 'f', 6);
    }
    QString z_text;
    if (z > -1e100)
    {
      z_text = QString::number(z, 'f', 6);
    }

    QTableWidgetItem* x_item = item(itr_row->second, 1);
    if (!x_item)
    {
      return -1;
    }
    x_item->setText(x_text);
    QTableWidgetItem* y_item = item(itr_row->second, 2);
    if (!y_item)
    {
      return -1;
    }
    y_item->setText(y_text);
    QTableWidgetItem* z_item = item(itr_row->second, 3);
    if (!z_item)
    {
      return -1;
    }
    z_item->setText(z_text);
  }
  else if (display_mode_ == SHOW_ERROR)
  {
    Float x_measure = itr_gcp->second.measure_pos[0];
    Float x_estimate = itr_gcp->second.estimate_pos[0];
    QString x_text;
    if (x_estimate > -1e100)
    {
      x_text = QString::number(x_measure - x_estimate, 'f', 6);
    }
    Float y_measure = itr_gcp->second.measure_pos[1];
    Float y_estimate = itr_gcp->second.estimate_pos[1];
    QString y_text;
    if (y_estimate > -1e100)
    {
      y_text = QString::number(y_measure - y_estimate, 'f', 6);
    }
    Float z_measure = itr_gcp->second.measure_pos[2];
    Float z_estimate = itr_gcp->second.estimate_pos[2];
    QString z_text;
    if (z_estimate > -1e100)
    {
      z_text = QString::number(z_measure - z_estimate, 'f', 6);
    }

    QTableWidgetItem* x_item = item(itr_row->second, 1);
    if (!x_item)
    {
      return -1;
    }
    x_item->setText(x_text);
    QTableWidgetItem* y_item = item(itr_row->second, 2);
    if (!y_item)
    {
      return -1;
    }
    y_item->setText(y_text);
    QTableWidgetItem* z_item = item(itr_row->second, 3);
    if (!z_item)
    {
      return -1;
    }
    z_item->setText(z_text);
  }
  return 0;
}

void GCPsTableWidget::SetGCPTypeEditable(uint gcp_id, bool is_type_editable)
{
  auto itr_type_editable = gcp_type_editable_map_.find(gcp_id);
  if (itr_type_editable != gcp_type_editable_map_.end())
  {
    itr_type_editable->second = is_type_editable;
  }
}

void GCPsTableWidget::OnCellClicked(int row, int column)
{
  QTableWidgetItem* item_name = item(row, 0);
  if (!item_name) return;
  uint gcp_id = item_name->data(Qt::UserRole).toUInt();
  auto itr_type_editable = gcp_type_editable_map_.find(gcp_id);
  if (itr_type_editable == gcp_type_editable_map_.end())
  {
    return;
  }
  if (column == 4 && itr_type_editable->second)
  {
    QTableWidgetItem* item_type = item(row, column);
    uint type = item_type->data(Qt::UserRole).toUInt();

    if (combo_box_row_ >= 0)
    {
      removeCellWidget(combo_box_row_, 4);
    }

    combo_box_type_ = new QComboBox(this);
    combo_box_type_->addItems(TypeLabels());
    combo_box_type_->setCurrentIndex(int(type));
    setCellWidget(row, column, combo_box_type_);
    void (QComboBox::*index_changed_signal)(int) =
      &QComboBox::currentIndexChanged;
    QObject::connect(combo_box_type_, index_changed_signal,
                     this, &GCPsTableWidget::OnCurrentIndexChanged);
    combo_box_row_ = row;
  }
  else if (combo_box_row_ >= 0)
  {
    removeCellWidget(combo_box_row_, 4);
    combo_box_row_ = -1;
  }
}

void GCPsTableWidget::OnItemSelectionChanged()
{
  QList<QTableWidgetItem*> selected_items = selectedItems();
  if (selected_items.empty() && combo_box_row_ >= 0)
  {
    removeCellWidget(combo_box_row_, 4);
    combo_box_row_ = -1;
  }
  else if (!selected_items.empty())
  {
    std::vector<uint> gcp_ids;
    for (size_t i = 0; i < selected_items.size(); i++)
    {
      if (selected_items[int(i)]->column() == 0)
      {
        gcp_ids.push_back(selected_items[int(i)]->data(Qt::UserRole).toUInt());
      }
    }
    emit GCPsSelected(gcp_ids);
  }
}

void GCPsTableWidget::OnCurrentIndexChanged(int index)
{
  QTableWidgetItem* name_item = item(combo_box_row_, 0);
  uint gcp_id = name_item->data(Qt::UserRole).toUInt();

  gcps_[gcp_id].type = index;

  emit GCPUpdated(gcp_id, gcps_[gcp_id]);

  removeCellWidget(combo_box_row_, 4);
  combo_box_row_ = -1;

  RefreshTable();
}

void GCPsTableWidget::OnItemChanged(QTableWidgetItem* item)
{
  if (!is_refresh_complete_) return;
  int row = item->row();
  int col = item->column();
  QTableWidgetItem* item_name = this->item(row, 0);
  uint gcp_id = item_name->data(Qt::UserRole).toUInt();
  auto itr_gcp_entry = gcps_.find(gcp_id);
  if (itr_gcp_entry == gcps_.end())
  {
    QMessageBox msg_box;
    msg_box.setText(tr("Ground Control Point Not Exist! Data Error!"));
    msg_box.exec();
    return;
  }
  bool updated = false;
  if (col == 0)
  {
    itr_gcp_entry->second.name = item->text();
    updated = true;
  }
  if (display_mode_ == SHOW_MEASUREMENT)
  {
    if (col == 1)
    {
      itr_gcp_entry->second.measure_pos[0] = Float(item->text().toDouble());
    }
    else if (col == 2)
    {
      itr_gcp_entry->second.measure_pos[1] = Float(item->text().toDouble());
    }
    else if (col == 3)
    {
      itr_gcp_entry->second.measure_pos[2] = Float(item->text().toDouble());
    }

    updated = true;
  }
  if (updated)
  {
    emit GCPUpdated(gcp_id, itr_gcp_entry->second);
  }

}

void GCPsTableWidget::RefreshTable()
{
  is_refresh_complete_ = false;
  while (rowCount() > 0)
  {
    removeRow(0);
  }
  setRowCount(int(gcps_.size()));
  auto itr_gcp = gcps_.begin();
  auto itr_gcp_end = gcps_.end();
  QStringList type_labels = TypeLabels();
  gcp_row_map_.clear();
  for (int row = 0; itr_gcp != itr_gcp_end; ++itr_gcp, row++)
  {
    QTableWidgetItem* name_item = new QTableWidgetItem(itr_gcp->second.name);
    name_item->setData(Qt::UserRole, QVariant(itr_gcp->first));
    setItem(row, 0, name_item);
    gcp_row_map_[itr_gcp->first] = row;

    if (display_mode_ == SHOW_MEASUREMENT)
    {
      QTableWidgetItem* x_item =
        new QTableWidgetItem(QString::number(itr_gcp->second.measure_pos[0],
                                             'f', 6));
      setItem(row, 1, x_item);
      QTableWidgetItem* y_item =
        new QTableWidgetItem(QString::number(itr_gcp->second.measure_pos[1],
                                             'f', 6));
      setItem(row, 2, y_item);
      QTableWidgetItem* z_item =
        new QTableWidgetItem(QString::number(itr_gcp->second.measure_pos[2],
                                             'f', 6));
      setItem(row, 3, z_item);
    }
    else if(display_mode_ == SHOW_ESTIMATE)
    {
      Float x = itr_gcp->second.estimate_pos[0];
      QString x_text;
      if (x > -1e100)
      {
        x_text = QString::number(x, 'f', 6);
      }
      Float y = itr_gcp->second.estimate_pos[1];
      QString y_text;
      if (y > -1e100)
      {
        y_text = QString::number(y, 'f', 6);
      }
      Float z = itr_gcp->second.estimate_pos[2];
      QString z_text;
      if (z > -1e100)
      {
        z_text = QString::number(z, 'f', 6);
      }

      QTableWidgetItem* x_item = new QTableWidgetItem(x_text);
      setItem(row, 1, x_item);
      QTableWidgetItem* y_item = new QTableWidgetItem(y_text);
      setItem(row, 2, y_item);
      QTableWidgetItem* z_item = new QTableWidgetItem(z_text);
      setItem(row, 3, z_item);
    }
    else if (display_mode_ == SHOW_ERROR)
    {
      Float x_measure = itr_gcp->second.measure_pos[0];
      Float x_estimate = itr_gcp->second.estimate_pos[0];
      QString x_text;
      if (x_estimate > -1e100)
      {
        x_text = QString::number(x_measure - x_estimate, 'f', 6);
      }
      Float y_measure = itr_gcp->second.measure_pos[1];
      Float y_estimate = itr_gcp->second.estimate_pos[1];
      QString y_text;
      if (y_estimate > -1e100)
      {
        y_text = QString::number(y_measure - y_estimate, 'f', 6);
      }
      Float z_measure = itr_gcp->second.measure_pos[2];
      Float z_estimate = itr_gcp->second.estimate_pos[2];
      QString z_text;
      if (z_estimate > -1e100)
      {
        z_text = QString::number(z_measure - z_estimate, 'f', 6);
      }

      QTableWidgetItem* x_item = new QTableWidgetItem(x_text);
      setItem(row, 1, x_item);
      QTableWidgetItem* y_item = new QTableWidgetItem(y_text);
      setItem(row, 2, y_item);
      QTableWidgetItem* z_item = new QTableWidgetItem(z_text);
      setItem(row, 3, z_item);
    }

    QString type_text = type_labels[itr_gcp->second.type];
    QTableWidgetItem* type_item = new QTableWidgetItem(type_text);
    type_item->setFlags(type_item->flags() ^ Qt::ItemIsEditable);
    type_item->setData(Qt::UserRole, QVariant(uint(itr_gcp->second.type)));
    setItem(row, 4, type_item);
  }
  resizeColumnsToContents();
  is_refresh_complete_ = true;
}

}
}
}
