#ifndef _HS_3D_RECONSTRUCTOR_SETTINGS_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_SETTINGS_DIALOG_HPP_

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>

namespace hs
{
namespace recon
{
namespace gui
{

class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  SettingsDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

private slots:
  void OnAccept();
  void OnPushButtonIntermediateDirClicked();
  void OnSliderNumberOfThreadValueChanged(int value);

private:
  QVBoxLayout* layout_all_;
  QHBoxLayout* layout_number_of_thread_;
  QLabel* label_number_of_thread_;
  QSlider* slider_number_of_thread_;
  QLabel* label_intermediate_dir_;
  QHBoxLayout* layout_intermediate_dir_;
  QLineEdit* line_edit_intermediate_dir_;
  QPushButton* push_button_intermediate_dir_;
  QDialogButtonBox* button_box_cancel_ok_;
};

}
}
}

#endif
