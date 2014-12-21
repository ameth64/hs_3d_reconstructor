#include "gui/property_field_asignment_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PropertyFieldAsignmentDialog::PropertyFieldAsignmentDialog(
  const std::string& file_content,
  const QStringList& field_names,
  QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , asignment_widget_(new PropertyFieldAsignmentWidget(file_content,
                                                       field_names, this))
  , layout_all_(new QVBoxLayout(this))
  , dialog_button_box_(new QDialogButtonBox(QDialogButtonBox::Cancel |
                                            QDialogButtonBox::Ok, this))
{
  layout_all_->addWidget(asignment_widget_);
  layout_all_->addWidget(dialog_button_box_);

  QObject::connect(dialog_button_box_, &QDialogButtonBox::accepted,
                   this, &QDialog::accept);
  QObject::connect(dialog_button_box_, &QDialogButtonBox::rejected,
                   this, &QDialog::reject);
}

int PropertyFieldAsignmentDialog::GetAssignedFieldValues(
  AssignedFieldValues& assigned_field_values)
{
  return asignment_widget_->GetAssignedFieldValues(assigned_field_values);
}

}
}
}
