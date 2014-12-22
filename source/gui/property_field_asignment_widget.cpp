#include <fstream>
#include <streambuf>
#include <sstream>
#include <limits>
#include <cctype>
#include <functional>

#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QFocusEvent>
#include <QHeaderView>

#include <boost/tokenizer.hpp>

#include "gui/property_field_asignment_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

TemporaryComboBox::TemporaryComboBox(QWidget* parent)
  : QComboBox(parent)
{

}

void TemporaryComboBox::focusOutEvent(QFocusEvent* e)
{
  if (e->reason() == Qt::MouseFocusReason)
  {
    emit Conceal();
  }
  QComboBox::focusOutEvent(e);
}

int TemporaryComboBox::column()
{
  return column_;
}

void TemporaryComboBox::set_column(int column)
{
  column_ = column;
}

PropertyFieldAsignmentWidget::PropertyFieldAsignmentWidget(
  const std::string& file_content,
  const QStringList& field_names,
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f), file_content_(file_content)
  , field_names_(field_names)
  , layout_all_(new QVBoxLayout(this))
  , layout_header_(new QHBoxLayout)
  , label_skip_lines_(new QLabel(tr("Skip lines:"), this))
  , spin_box_skip_lines_(new QSpinBox(this))
  , label_seperator_(new QLabel(tr("Seperator:"), this))
  , check_box_seperator_blank_(new QCheckBox(tr("Blank"), this))
  , check_box_seperator_comma_(new QCheckBox(tr("Comma"), this))
  , check_box_seperator_semicolon_(new QCheckBox(tr("Semicolon"), this))
  , check_box_seperator_colon_(new QCheckBox(tr("Colon"), this))
  , check_box_seperator_vertical_bar_(new QCheckBox(tr("Vertical bar"), this))
  , label_comment_(new QLabel(tr("Comment characters:"), this))
  , line_edit_comment_(new QLineEdit(this))
  , table_widget_property_assignment_(new QTableWidget(this))
{
  layout_all_->addLayout(layout_header_);
  layout_all_->addWidget(table_widget_property_assignment_);

  layout_header_->addWidget(label_skip_lines_);
  layout_header_->addWidget(spin_box_skip_lines_);
  layout_header_->addWidget(label_comment_);
  layout_header_->addWidget(line_edit_comment_);
  layout_header_->addWidget(label_seperator_);
  layout_header_->addWidget(check_box_seperator_blank_);
  layout_header_->addWidget(check_box_seperator_comma_);
  layout_header_->addWidget(check_box_seperator_semicolon_);
  layout_header_->addWidget(check_box_seperator_colon_);
  layout_header_->addWidget(check_box_seperator_vertical_bar_);

  QHeaderView* header =
    table_widget_property_assignment_->horizontalHeader();
  combo_box_assign_ = new TemporaryComboBox(header);
  combo_box_assign_->hide();

  QObject::connect(table_widget_property_assignment_->horizontalHeader(),
                   &QHeaderView::sectionClicked,
                   this,
                   &PropertyFieldAsignmentWidget::OnRecordsSectionClicked);
  QObject::connect(combo_box_assign_, &TemporaryComboBox::Conceal,
                   this, &PropertyFieldAsignmentWidget::OnComboBoxConceal);
  QObject::connect(
    check_box_seperator_blank_, &QCheckBox::stateChanged,
    this, &PropertyFieldAsignmentWidget::OnCheckBoxSeperatorBlankStateChanged);
  QObject::connect(
    check_box_seperator_comma_, &QCheckBox::stateChanged,
    this, &PropertyFieldAsignmentWidget::OnCheckBoxSeperatorCommaStateChanged);
  QObject::connect(
    check_box_seperator_semicolon_, &QCheckBox::stateChanged,
    this,
    &PropertyFieldAsignmentWidget::OnCheckBoxSeperatorSemicolonStateChanged);
  QObject::connect(
    check_box_seperator_colon_, &QCheckBox::stateChanged,
    this, &PropertyFieldAsignmentWidget::OnCheckBoxSeperatorColonStateChanged);
  QObject::connect(
    check_box_seperator_vertical_bar_, &QCheckBox::stateChanged,
    this,
    &PropertyFieldAsignmentWidget::OnCheckBoxSeperatorVerticalBarStateChanged);
  void (QSpinBox::*spin_box_value_changed)(int) = &QSpinBox::valueChanged;
  QObject::connect(
    spin_box_skip_lines_, spin_box_value_changed,
    this, &PropertyFieldAsignmentWidget::OnSpinBoxSkipLinesValueChanged);

  UpdateRecords();
  DisplayRecords();
}

int PropertyFieldAsignmentWidget::GetAssignedFieldValues(
  AssignedFieldValues& assigned_field_values)
{
  assigned_field_values.clear();
  int column_count =
    table_widget_property_assignment_->columnCount();
  for (int i = 0; i < column_count; i++)
  {
    QTableWidgetItem* item_header =
      table_widget_property_assignment_->horizontalHeaderItem(i);
    QVariant data = item_header->data(Qt::UserRole);
    int field_id = data.toInt();
    if (field_id != -1)
    {
      const QString& field_name = field_names_[field_id];
      FieldValue field_value;
      for (int j = 0;
           j < table_widget_property_assignment_->rowCount(); j++)
      {
        QTableWidgetItem* item_field =
          table_widget_property_assignment_->item(j, i);
        QString text_field = item_field->text();
        field_value.push_back(text_field);
      }
      assigned_field_values[field_name] = field_value;
    }
  }

  if (assigned_field_values.size() != field_names_.size())
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

int PropertyFieldAsignmentWidget::UpdateRecords()
{
  int skip_lines = spin_box_skip_lines_->value();
  std::string comment_string = line_edit_comment_->text().toLocal8Bit().data();
  std::string seperators;
  if (check_box_seperator_blank_->isChecked())
  {
    for (unsigned char i = 0;
         i < std::numeric_limits<unsigned char>::max(); i++)
    {
      if (std::isspace(i))
      {
        seperators.push_back(i);
      }
    }
  }
  if (check_box_seperator_comma_->isChecked())
  {
    seperators.push_back(',');
  }
  if (check_box_seperator_semicolon_->isChecked())
  {
    seperators.push_back(';');
  }
  if (check_box_seperator_colon_->isChecked())
  {
    seperators.push_back(':');
  }
  if (check_box_seperator_vertical_bar_->isChecked())
  {
    seperators.push_back('|');
  }

  if (seperators.empty())
  {
    bool detect_seperator_success = false;
    RecordContainer records;
    while (1)
    {
      std::string blank_seperator;
      for (unsigned char i = 0;
           i < std::numeric_limits<unsigned char>::max(); i++)
      {
        if (std::isspace(i))
        {
          blank_seperator.push_back(i);
        }
      }
      if (SeperateFields(skip_lines, blank_seperator, comment_string,
                         records) == 0)
      {
        detect_seperator_success = true;
        break;
      }
      std::string comma_seperator = ",";
      if (SeperateFields(skip_lines, comma_seperator, comment_string,
                         records) == 0)
      {
        detect_seperator_success = true;
        break;
      }
      std::string semicolon_seperator = ";";
      if (SeperateFields(skip_lines, semicolon_seperator, comment_string,
                         records) == 0)
      {
        detect_seperator_success = true;
        break;
      }
      std::string colon_seperator = ":";
      if (SeperateFields(skip_lines, colon_seperator, comment_string,
                         records) == 0)
      {
        detect_seperator_success = true;
        break;
      }
      std::string vertical_bar_seperator = "|";
      if (SeperateFields(skip_lines, vertical_bar_seperator, comment_string,
                         records) == 0)
      {
        break;
      }
      break;
    }
    if (detect_seperator_success)
    {
      records_.swap(records);
      DisplayRecords();
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    int result =
      SeperateFields(skip_lines, seperators, comment_string, records_);
    DisplayRecords();
    return result;
  }
  return 0;
}

int PropertyFieldAsignmentWidget::DisplayRecords()
{
  int row_count = table_widget_property_assignment_->rowCount();
  for (int i = 0; i < row_count; i++)
  {
    table_widget_property_assignment_->removeRow(0);
  }

  int number_of_records = int(records_.size());
  int number_of_fields = 0;
  for (int i = 0; i < number_of_records; i++)
  {
    number_of_fields = std::max(int(records_[i].size()), number_of_fields);
  }

  table_widget_property_assignment_->setRowCount(number_of_records);
  table_widget_property_assignment_->setColumnCount(number_of_fields);
  table_widget_property_assignment_->setEditTriggers(
    QAbstractItemView::NoEditTriggers);
  for (int i = 0; i < number_of_fields; i++)
  {
    QTableWidgetItem* header_item =
      new QTableWidgetItem(QString::fromLocal8Bit("?"));
    header_item->setData(Qt::UserRole,
                         QVariant(-1));
    header_item->setTextAlignment(Qt::AlignLeft);
    table_widget_property_assignment_->setHorizontalHeaderItem(
      i, header_item);
  }
  for (int i = 0; i < number_of_records; i++)
  {
    const Record& record = records_[i];
    for (int j = 0; j < record.size(); j++)
    {
      QTableWidgetItem* item = new QTableWidgetItem(record[j]);
      table_widget_property_assignment_->setItem(i, j, item);
    }
  }
  table_widget_property_assignment_->resizeColumnsToContents();

  return 0;
}

void PropertyFieldAsignmentWidget::OnCheckBoxSeperatorBlankStateChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnCheckBoxSeperatorCommaStateChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnCheckBoxSeperatorSemicolonStateChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnCheckBoxSeperatorColonStateChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnCheckBoxSeperatorVerticalBarStateChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnSpinBoxSkipLinesValueChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnLineEditCommentTextChanged()
{
  UpdateRecords();
}

void PropertyFieldAsignmentWidget::OnRecordsSectionClicked(int column)
{
  QHeaderView* header =
    table_widget_property_assignment_->horizontalHeader();
  int section_width = header->sectionSize(column);
  int section_pos = header->sectionPosition(column) - header->offset();
  int section_height = header->rect().height();
  combo_box_assign_->set_column(column);
  combo_box_assign_->show();
  combo_box_assign_->setFocus(Qt::MouseFocusReason);
  combo_box_assign_->resize(section_width, section_height);
  combo_box_assign_->move(section_pos, 0);
  int item_count = combo_box_assign_->count();
  for (int i = 0; i < item_count; i++)
  {
    combo_box_assign_->removeItem(0);
  }
  combo_box_assign_->addItem(tr("?"));
  combo_box_assign_->setItemData(0, QVariant(-1));
  for (int i = 0; i < int(field_names_.size()); i++)
  {
    combo_box_assign_->addItem(field_names_[i]);
    combo_box_assign_->setItemData(i + 1, QVariant(i));
  }
  QTableWidgetItem* item =
    table_widget_property_assignment_->horizontalHeaderItem(column);
  int field_id = item->data(Qt::UserRole).toInt();
  if (field_id == -1)
  {
    combo_box_assign_->setCurrentIndex(0);
  }
  else
  {
    combo_box_assign_->setCurrentIndex(field_id + 1);
  }
  void (QComboBox:: *current_index_changed)(int) =
    &QComboBox::currentIndexChanged;
  QObject::connect(combo_box_assign_, current_index_changed,
                   this, &PropertyFieldAsignmentWidget::OnAssignChanged);
}

void PropertyFieldAsignmentWidget::OnAssignChanged(int index)
{
  int column = combo_box_assign_->column();
  QTableWidgetItem* current_header_item =
    table_widget_property_assignment_->horizontalHeaderItem(column);
  QVariant current_data = combo_box_assign_->itemData(index, Qt::UserRole);
  int current_id = current_data.toInt();
  if (current_id != -1)
  {
    current_header_item->setText(field_names_[current_id]);
    current_header_item->setData(Qt::UserRole, current_data);
    for (int i = 0; i < int(field_names_.size()); i++)
    {
      if (i != column)
      {
        QTableWidgetItem* item =
          table_widget_property_assignment_->horizontalHeaderItem(i);
        QVariant data = item->data(Qt::UserRole);
        if (data == current_data)
        {
          item->setText(QString::fromLocal8Bit("?"));
          item->setData(Qt::UserRole, QVariant(-1));
        }
      }
    }
  }
  else
  {
    current_header_item->setText(QString::fromLocal8Bit("?"));
    current_header_item->setData(Qt::UserRole, current_data);
  }
  OnComboBoxConceal();
}

void PropertyFieldAsignmentWidget::OnComboBoxConceal()
{
  void (QComboBox:: *current_index_changed)(int) =
    &QComboBox::currentIndexChanged;
  QObject::disconnect(combo_box_assign_, current_index_changed,
                      this, &PropertyFieldAsignmentWidget::OnAssignChanged);
  combo_box_assign_->hide();
}

int PropertyFieldAsignmentWidget::SeperateFields(
  int skip_lines, const std::string& seperators,
  const std::string& comment_string,
  RecordContainer& records)
{
  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
  std::stringstream ss;
  ss.str(file_content_);
  records.clear();
  int line_id = 0;
  while (!ss.eof())
  {
    std::string line;
    std::getline(ss, line);
    if (line.empty()) break;

    //跳过指定行数
    if (line_id < skip_lines)
    {
      line_id++;
      continue;
    }
    std::string line_trimmed = line;
    line_trimmed.erase(line_trimmed.begin(),
                       std::find_if(line_trimmed.begin(), line_trimmed.end(),
                                    std::not1(std::ptr_fun<int, int>(
                                                std::isspace))));
    //跳过注释行
    if (!comment_string.empty() && line_trimmed.find(comment_string) == 0)
      continue;

    Record record;
    boost::char_separator<char> sep(seperators.c_str(), "");
    Tokenizer tokenizer(line, sep);
    auto itr_token = tokenizer.begin();
    auto itr_token_end = tokenizer.end();
    for (; itr_token != itr_token_end; ++itr_token)
    {
      record.push_back(QString::fromLocal8Bit(std::string(*itr_token).c_str()));
    }

    records.push_back(record);
  }

  if (records.empty()) return -1;
  auto itr_record = records.begin();
  auto itr_record_end = records.end();
  size_t size = itr_record->size();
  for (; itr_record != itr_record_end; ++itr_record)
  {
    if (itr_record->size() != size) return -1;
  }
  if (size <= 1) return -1;

  return 0;
}

}
}
}
