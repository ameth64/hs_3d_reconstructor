#ifndef _HS_3D_RECONSTRUCTOR_GUI_PROPERTY_FIELD_ASIGNMENT_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PROPERTY_FIELD_ASIGNMENT_DIALOG_HPP_

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "gui/property_field_asignment_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PropertyFieldAsignmentDialog : public QDialog
{
  Q_OBJECT
public:
  typedef PropertyFieldAsignmentWidget::AssignedFieldValues
          AssignedFieldValues;
public:
  PropertyFieldAsignmentDialog(const std::string& file_content,
                               const QStringList& field_names,
                               QWidget* parent = 0, Qt::WindowFlags f = 0);
public:
  int GetAssignedFieldValues(AssignedFieldValues& assigned_field_values);

private:
  QVBoxLayout* layout_all_;
  PropertyFieldAsignmentWidget* asignment_widget_;
  QDialogButtonBox* dialog_button_box_;
};

}
}
}

#endif
