#include <QFileDialog>
#include <QSettings>
#include <QThread>
#include <QMessageBox>

#include "gui/settings_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

SettingsDialog::SettingsDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  layout_all_ = new QVBoxLayout(this);

  label_number_of_thread_ = new QLabel(tr("Active Threads:"), this);
  layout_all_->addWidget(label_number_of_thread_);

  slider_number_of_thread_ = new QSlider(Qt::Horizontal, this);
  layout_all_->addWidget(slider_number_of_thread_);

  label_intermediate_dir_ = new QLabel(tr("Intermediate Directory:"), this);
  layout_all_->addWidget(label_intermediate_dir_);

  layout_intermediate_dir_ = new QHBoxLayout;
  layout_all_->addLayout(layout_intermediate_dir_);

  line_edit_intermediate_dir_ = new QLineEdit(this);
  line_edit_intermediate_dir_->setReadOnly(true);
  layout_intermediate_dir_->addWidget(line_edit_intermediate_dir_);

  push_button_intermediate_dir_ = new QPushButton(tr("Browse"), this);
  layout_intermediate_dir_->addWidget(push_button_intermediate_dir_);

  button_box_cancel_ok_ = new QDialogButtonBox(QDialogButtonBox::Ok |
                                               QDialogButtonBox::Cancel, this);
  layout_all_->addWidget(button_box_cancel_ok_);

  setLayout(layout_all_);

  QObject::connect(button_box_cancel_ok_, &QDialogButtonBox::accepted,
                   this, &QDialog::accept);
  QObject::connect(button_box_cancel_ok_, &QDialogButtonBox::rejected,
                   this, &QDialog::reject);
  QObject::connect(this, &QDialog::accepted, this, &SettingsDialog::OnAccept);
  QObject::connect(
    push_button_intermediate_dir_, &QPushButton::clicked,
    this, &SettingsDialog::OnPushButtonIntermediateDirClicked);
  QObject::connect(slider_number_of_thread_, &QSlider::valueChanged,
                   this, &SettingsDialog::OnSliderNumberOfThreadValueChanged);

  QSettings settings;

  QString number_of_threads_key = tr("number_of_threads");
  uint number_of_threads = settings.value(number_of_threads_key,
                                          QVariant(uint(1))).toUInt();
  slider_number_of_thread_->setMinimum(1);
  slider_number_of_thread_->setMaximum(QThread::idealThreadCount());
  slider_number_of_thread_->setSingleStep(1);
  slider_number_of_thread_->setPageStep(1);
  slider_number_of_thread_->setValue(int(number_of_threads));
  slider_number_of_thread_->setTickInterval(1);
  slider_number_of_thread_->setTickPosition(QSlider::TicksBothSides);

  QString intermediate_directory_key = tr("intermediate_directory");
  QString intermediate_directory =
    settings.value(intermediate_directory_key).toString();
  line_edit_intermediate_dir_->setText(intermediate_directory);

  resize(600, 200);
}

void SettingsDialog::OnAccept()
{
  QSettings settings;

  QString number_of_threads_key = tr("number_of_threads");
  uint number_of_threads = uint(slider_number_of_thread_->value());
  settings.setValue(number_of_threads_key, number_of_threads);

  QString intermediate_directory_key = tr("intermediate_directory");
  QString intermediate_directory = line_edit_intermediate_dir_->text();
  settings.setValue(intermediate_directory_key, intermediate_directory);
}

void SettingsDialog::OnPushButtonIntermediateDirClicked()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setDirectory(line_edit_intermediate_dir_->text());
  if (dialog.exec())
  {
    line_edit_intermediate_dir_->setText(dialog.selectedFiles()[0]);
  }
}

void SettingsDialog::OnSliderNumberOfThreadValueChanged(int value)
{
  QString text = QString(tr("Active Threads:%1/%2"))
                 .arg(value).arg(QThread::idealThreadCount());
  label_number_of_thread_->setText(text);
}

}
}
}
