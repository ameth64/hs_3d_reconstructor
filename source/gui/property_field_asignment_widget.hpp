#ifndef _HS_3D_RECONSTRUCTOR_GUI_PROPERTY_FIELD_ASIGNMENT_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PROPERTY_FIELD_ASIGNMENT_WIDGET_HPP_

#include <vector>
#include <map>
#include <string>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

namespace hs
{
namespace recon
{
namespace gui
{

class TemporaryComboBox : public QComboBox
{
  Q_OBJECT
public:
  TemporaryComboBox(QWidget* parent = 0);
public:
  int column();
  void set_column(int column);
signals:
  void Conceal();
protected:
  virtual void focusOutEvent(QFocusEvent* e);
private:
  int column_;
};

class PropertyFieldAsignmentWidget : public QWidget
{
  Q_OBJECT
public:
  typedef QStringList Record;
  typedef std::vector<Record> RecordContainer;
  typedef QStringList FieldValue;
  typedef std::map<QString, FieldValue> AssignedFieldValues;
public:
  PropertyFieldAsignmentWidget(const std::string& file_content,
                               const QStringList& field_names,
                               QWidget* parent = 0, Qt::WindowFlags f = 0);

public:
  int GetAssignedFieldValues(AssignedFieldValues& assigned_field_values);

private slots:
  int UpdateRecords();
  int DisplayRecords();

  void OnCheckBoxSeperatorBlankStateChanged();
  void OnCheckBoxSeperatorCommaStateChanged();
  void OnCheckBoxSeperatorSemicolonStateChanged();
  void OnCheckBoxSeperatorColonStateChanged();
  void OnCheckBoxSeperatorVerticalBarStateChanged();
  void OnSpinBoxSkipLinesValueChanged();
  void OnLineEditCommentTextChanged();
  void OnRecordsSectionClicked(int column);
  void OnAssignChanged(int index);
  void OnComboBoxConceal();

private:
  int SeperateFields(int skip_lines, const std::string& seperators,
                     const std::string& comment_string,
                     RecordContainer& records);

private:
  RecordContainer records_;
  std::string file_content_;
  QStringList field_names_;
  TemporaryComboBox* combo_box_assign_;

  QVBoxLayout* layout_all_;
  QHBoxLayout* layout_header_;
  QLabel* label_skip_lines_;
  QSpinBox* spin_box_skip_lines_;
  QLabel* label_seperator_;
  QCheckBox* check_box_seperator_blank_;
  QCheckBox* check_box_seperator_comma_;
  QCheckBox* check_box_seperator_semicolon_;
  QCheckBox* check_box_seperator_colon_;
  QCheckBox* check_box_seperator_vertical_bar_;
  QLabel* label_comment_;
  QLineEdit* line_edit_comment_;
  QTableWidget* table_widget_property_assignment_;
};

}
}
}

#endif
