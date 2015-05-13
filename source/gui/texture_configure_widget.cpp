#include "gui/texture_configure_widget.hpp"

#include <QIntValidator>
#include <QDoubleValidator>
#include <QFileDialog>

namespace hs
{
namespace recon
{
namespace gui
{

TextureConfigureWidget::TextureConfigureWidget(
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  main_layout_ = new QVBoxLayout;
  setLayout(main_layout_);
  layout_group_box_dem_ = new QVBoxLayout;
  layout_group_box_dom_ = new QVBoxLayout;

  layout_dem_path_ = new QHBoxLayout;
  label_dem_path_ = new QLabel(tr("DEM Path:"));
  line_edit_dem_path_ = new QLineEdit;
  line_edit_dem_path_->setEnabled(false);
  button_browse_dem_ = new QPushButton(tr("Browse"));
  layout_dem_path_->addWidget(label_dem_path_);
  layout_dem_path_->addWidget(line_edit_dem_path_);
  layout_dem_path_->addWidget(button_browse_dem_);
  layout_group_box_dem_->addLayout(layout_dem_path_);

  layout_dom_path_ = new QHBoxLayout;
  label_dom_path_ = new QLabel(tr("DOM Path:"));
  line_edit_dom_path_ = new QLineEdit;
  line_edit_dom_path_->setEnabled(false);
  button_browse_dom_ = new QPushButton(tr("Browse"));
  layout_dom_path_->addWidget(label_dom_path_);
  layout_dom_path_->addWidget(line_edit_dom_path_);
  layout_dom_path_->addWidget(button_browse_dom_);
  layout_group_box_dom_->addLayout(layout_dom_path_);

  QIntValidator* int_validator = new QIntValidator(this);
  int_validator->setBottom(0);
  QDoubleValidator* double_validator = new QDoubleValidator(this);
  double_validator->setBottom(0.0);
  layout_dem_scale_ = new QHBoxLayout;
  label_dem_x_scale_ = new QLabel(tr("DEM X Scale:"));
  line_edit_dem_x_scale_ = new QLineEdit;
  line_edit_dem_x_scale_->setValidator(double_validator);
  line_edit_dem_x_scale_->setText(QString::number(0.1));
  label_dem_y_scale_ = new QLabel(tr("DEM Y Scale:"));
  line_edit_dem_y_scale_ = new QLineEdit;
  line_edit_dem_y_scale_->setValidator(double_validator);
  line_edit_dem_y_scale_->setText(QString::number(0.1));
  layout_dem_scale_->addWidget(label_dem_x_scale_);
  layout_dem_scale_->addWidget(line_edit_dem_x_scale_);
  layout_dem_scale_->addWidget(label_dem_y_scale_);
  layout_dem_scale_->addWidget(line_edit_dem_y_scale_);
  layout_group_box_dem_->addLayout(layout_dem_scale_);

  layout_dem_tile_size_ =new QHBoxLayout;
  label_dem_tile_x_size_ = new QLabel(tr("DEM Tile Width:"));
  line_edit_dem_tile_x_size_ = new QLineEdit;
  line_edit_dem_tile_x_size_->setValidator(int_validator);
  line_edit_dem_tile_x_size_->setText(QString::number(2048));
  label_dem_tile_y_size_ = new QLabel(tr("DEM Tile Height:"));
  line_edit_dem_tile_y_size_ = new QLineEdit;
  line_edit_dem_tile_y_size_->setValidator(int_validator);
  line_edit_dem_tile_y_size_->setText(QString::number(2048));
  layout_dem_tile_size_->addWidget(label_dem_tile_x_size_);
  layout_dem_tile_size_->addWidget(line_edit_dem_tile_x_size_);
  layout_dem_tile_size_->addWidget(label_dem_tile_y_size_);
  layout_dem_tile_size_->addWidget(line_edit_dem_tile_y_size_);
  layout_group_box_dem_->addLayout(layout_dem_tile_size_);

  layout_dom_scale_ = new QHBoxLayout;
  label_dom_x_scale_ = new QLabel(tr("DOM X Scale:"));
  line_edit_dom_x_scale_ = new QLineEdit;
  line_edit_dom_x_scale_->setValidator(double_validator);
  line_edit_dom_x_scale_->setText(QString::number(0.1));
  label_dom_y_scale_ = new QLabel(tr("DOM Y Scale:"));
  line_edit_dom_y_scale_ = new QLineEdit;
  line_edit_dom_y_scale_->setValidator(double_validator);
  line_edit_dom_y_scale_->setText(QString::number(0.1));
  layout_dom_scale_->addWidget(label_dom_x_scale_);
  layout_dom_scale_->addWidget(line_edit_dom_x_scale_);
  layout_dom_scale_->addWidget(label_dom_y_scale_);
  layout_dom_scale_->addWidget(line_edit_dom_y_scale_);
  layout_group_box_dom_->addLayout(layout_dom_scale_);

  layout_dom_tile_size_ =new QHBoxLayout;
  label_dom_tile_x_size_ = new QLabel(tr("DOM Tile Width:"));
  line_edit_dom_tile_x_size_ = new QLineEdit;
  line_edit_dom_tile_x_size_->setValidator(int_validator);
  line_edit_dom_tile_x_size_->setText(QString::number(2048));
  label_dom_tile_y_size_ = new QLabel(tr("DOM Tile Height:"));
  line_edit_dom_tile_y_size_ = new QLineEdit;
  line_edit_dom_tile_y_size_->setValidator(int_validator);
  line_edit_dom_tile_y_size_->setText(QString::number(2048));
  layout_dom_tile_size_->addWidget(label_dom_tile_x_size_);
  layout_dom_tile_size_->addWidget(line_edit_dom_tile_x_size_);
  layout_dom_tile_size_->addWidget(label_dom_tile_y_size_);
  layout_dom_tile_size_->addWidget(line_edit_dom_tile_y_size_);
  layout_group_box_dom_->addLayout(layout_dom_tile_size_);

  group_box_dem_ = new QGroupBox(tr("DEM Generation"), this);
  group_box_dem_->setLayout(layout_group_box_dem_);
  group_box_dem_->setCheckable(true);
  group_box_dem_->setChecked(false);
  main_layout_->addWidget(group_box_dem_);

  group_box_dom_ = new QGroupBox(tr("DOM Generation"), this);
  group_box_dom_->setLayout(layout_group_box_dom_);
  group_box_dom_->setCheckable(true);
  group_box_dom_->setChecked(false);
  main_layout_->addWidget(group_box_dom_);

  QObject::connect(button_browse_dem_, &QPushButton::clicked,
                   this,  &TextureConfigureWidget::OnButtonBrowseDEMClicked);
  QObject::connect(button_browse_dom_, &QPushButton::clicked,
                   this,  &TextureConfigureWidget::OnButtonBrowseDOMClicked);
}

void TextureConfigureWidget::FetchTextureConfig(
  workflow::TextureConfig& texture_config)
{
  if (group_box_dem_->isChecked())
  {
    std::string dem_path =
      line_edit_dem_path_->text().toLocal8Bit().data();
    texture_config.set_dem_path(dem_path);

    int dem_tile_x_size = line_edit_dem_tile_x_size_->text().toInt();
    int dem_tile_y_size = line_edit_dem_tile_y_size_->text().toInt();
    double dem_x_scale = line_edit_dem_x_scale_->text().toDouble();
    double dem_y_scale = line_edit_dem_y_scale_->text().toDouble();
    texture_config.set_dem_tile_x_size(dem_tile_x_size);
    texture_config.set_dem_tile_y_size(dem_tile_y_size);
    texture_config.set_dem_x_scale(dem_x_scale);
    texture_config.set_dem_y_scale(dem_y_scale);
  }

  if (group_box_dom_->isChecked())
  {
    std::string dom_path =
      line_edit_dom_path_->text().toLocal8Bit().data();
    texture_config.set_dom_path(dom_path);

    int dom_tile_x_size = line_edit_dom_tile_x_size_->text().toInt();
    int dom_tile_y_size = line_edit_dom_tile_y_size_->text().toInt();
    double dom_x_scale = line_edit_dom_x_scale_->text().toDouble();
    double dom_y_scale = line_edit_dom_y_scale_->text().toDouble();
    texture_config.set_dom_tile_x_size(dom_tile_x_size);
    texture_config.set_dom_tile_y_size(dom_tile_y_size);
    texture_config.set_dom_x_scale(dom_x_scale);
    texture_config.set_dom_y_scale(dom_y_scale);
  }
}

void TextureConfigureWidget::OnButtonBrowseDEMClicked()
{
  QFileDialog dialog;
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (dialog.exec())
  {
    QString dem_path = dialog.selectedFiles()[0];
    line_edit_dem_path_->setText(dem_path);
  }
}

void TextureConfigureWidget::OnButtonBrowseDOMClicked()
{
  QFileDialog dialog;
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (dialog.exec())
  {
    QString dom_path = dialog.selectedFiles()[0];
    line_edit_dom_path_->setText(dom_path);
  }
}


}
}
}
