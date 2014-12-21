#include "gui/photogroup_info_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotogroupInfo::PhotogroupInfo()
  : width(0), height(0)
  , focal_length(0.0)
  , pixel_size_x(0.0), pixel_size_y(0.0)
  , principal_x(0.0), principal_y(0.0)
  , skew(0.0)
  , radial1(0.0), radial2(0.0), radial3(0.0)
  , decentering1(0.0), decentering2(0.0)
{
}

PhotogroupInfoWidget::PhotogroupInfoWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  Initialize();
  PhotogroupInfo info;
  SetFromPhotogroupInfo(info);
}

PhotogroupInfoWidget::PhotogroupInfoWidget(
  const PhotogroupInfo& info, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  Initialize();
  SetFromPhotogroupInfo(info);
}

void PhotogroupInfoWidget::Initialize()
{
  layout_all_ = new QVBoxLayout(this);
  layout_name_ = new QHBoxLayout;
  label_name_ = new QLabel(tr("Name:"), this);
  line_edit_name_ = new QLineEdit(this);
  layout_description_ = new QHBoxLayout;
  label_description_ = new QLabel(tr("Description:"), this);
  text_edit_description_ = new QTextEdit(this);
  layout_dimension_ = new QHBoxLayout;
  label_dimension_ = new QLabel(tr("Photo Dimension: (in pixel)"), this);
  line_edit_width_ = new QLineEdit(this);
  line_edit_height_ = new QLineEdit(this);
  layout_focal_length_ = new QHBoxLayout;
  label_focal_length_ = new QLabel(tr("Focal Length (in mm):"), this);
  line_edit_focal_length_ = new QLineEdit(this);
  layout_pixel_size_ = new QHBoxLayout;
  label_pixel_size_ = new QLabel(tr("Pixel Size(in mm):"), this);
  line_edit_pixel_x_size_ = new QLineEdit(this);
  line_edit_pixel_y_size_ = new QLineEdit(this);
  layout_principal_point_ = new QHBoxLayout;
  label_principal_point_ = new QLabel(tr("Principal Point (in pixel)"), this);
  line_edit_principal_point_x_ = new QLineEdit(this);
  line_edit_principal_point_y_ = new QLineEdit(this);
  layout_skew_ = new QHBoxLayout;
  label_skew_ = new QLabel(tr("Skew:"), this);
  line_edit_skew_ = new QLineEdit(this);
  layout_radial_ = new QHBoxLayout;
  label_radial_ = new QLabel(tr("Radial:"), this);
  line_edit_radial1_ = new QLineEdit(this);
  line_edit_radial2_ = new QLineEdit(this);
  line_edit_radial3_ = new QLineEdit(this);
  layout_decentering_ = new QHBoxLayout;
  label_decentering_ = new QLabel(tr("Decentering:"), this);
  line_edit_decentering1_ = new QLineEdit(this);
  line_edit_decentering2_ = new QLineEdit(this);

  layout_all_->addLayout(layout_name_);
  layout_all_->addLayout(layout_description_);
  layout_all_->addLayout(layout_dimension_);
  layout_all_->addLayout(layout_focal_length_);
  layout_all_->addLayout(layout_pixel_size_);
  layout_all_->addLayout(layout_principal_point_);
  layout_all_->addLayout(layout_skew_);
  layout_all_->addLayout(layout_radial_);
  layout_all_->addLayout(layout_decentering_);

  layout_name_->addWidget(label_name_);
  layout_name_->addWidget(line_edit_name_);

  layout_description_->addWidget(label_description_);
  layout_description_->addWidget(text_edit_description_);

  layout_dimension_->addWidget(label_dimension_);
  layout_dimension_->addWidget(line_edit_width_);
  layout_dimension_->addWidget(line_edit_height_);
  line_edit_width_->setValidator(&int_validator_);
  line_edit_height_->setValidator(&int_validator_);

  layout_focal_length_->addWidget(label_focal_length_);
  layout_focal_length_->addWidget(line_edit_focal_length_);
  line_edit_focal_length_->setValidator(&double_validator_);

  layout_pixel_size_->addWidget(label_pixel_size_);
  layout_pixel_size_->addWidget(line_edit_pixel_x_size_);
  layout_pixel_size_->addWidget(line_edit_pixel_y_size_);
  line_edit_pixel_x_size_->setValidator(&double_validator_);
  line_edit_pixel_y_size_->setValidator(&double_validator_);

  layout_principal_point_->addWidget(label_principal_point_);
  layout_principal_point_->addWidget(line_edit_principal_point_x_);
  layout_principal_point_->addWidget(line_edit_principal_point_y_);
  line_edit_principal_point_x_->setValidator(&double_validator_);
  line_edit_principal_point_y_->setValidator(&double_validator_);

  layout_skew_->addWidget(label_skew_);
  layout_skew_->addWidget(line_edit_skew_);
  line_edit_skew_->setValidator(&double_validator_);

  layout_radial_->addWidget(label_radial_);
  layout_radial_->addWidget(line_edit_radial1_);
  layout_radial_->addWidget(line_edit_radial2_);
  layout_radial_->addWidget(line_edit_radial3_);
  line_edit_radial1_->setValidator(&double_validator_);
  line_edit_radial2_->setValidator(&double_validator_);
  line_edit_radial3_->setValidator(&double_validator_);

  layout_decentering_->addWidget(label_decentering_);
  layout_decentering_->addWidget(line_edit_decentering1_);
  layout_decentering_->addWidget(line_edit_decentering2_);
  line_edit_decentering1_->setValidator(&double_validator_);
  line_edit_decentering2_->setValidator(&double_validator_);

  int_validator_.setRange(0, 200000);
}

void PhotogroupInfoWidget::SetFromPhotogroupInfo(const PhotogroupInfo& info)
{
  line_edit_name_->setText(info.name);

  text_edit_description_->setPlainText(info.description);

  line_edit_width_->setText(QString::number(info.width));
  line_edit_height_->setText(QString::number(info.height));

  line_edit_focal_length_->setText(QString::number(info.focal_length));

  line_edit_pixel_x_size_->setText(QString::number(info.pixel_size_x));
  line_edit_pixel_y_size_->setText(QString::number(info.pixel_size_y));

  line_edit_principal_point_x_->setText(QString::number(info.principal_x));
  line_edit_principal_point_y_->setText(QString::number(info.principal_y));

  line_edit_skew_->setText(QString::number(info.skew));

  line_edit_radial1_->setText(QString::number(info.radial1));
  line_edit_radial2_->setText(QString::number(info.radial2));
  line_edit_radial3_->setText(QString::number(info.radial3));

  line_edit_decentering1_->setText(QString::number(info.decentering1));
  line_edit_decentering2_->setText(QString::number(info.decentering2));
}

PhotogroupInfo PhotogroupInfoWidget::GetPhotogroupInfo()
{
  PhotogroupInfo info;
  info.name = line_edit_name_->text();
  info.description = text_edit_description_->toPlainText();
  info.width = line_edit_width_->text().toInt();
  info.height = line_edit_height_->text().toInt();
  info.focal_length = line_edit_focal_length_->text().toDouble();
  info.pixel_size_x = line_edit_pixel_x_size_->text().toDouble();
  info.pixel_size_y = line_edit_pixel_y_size_->text().toDouble();
  info.principal_x = line_edit_principal_point_x_->text().toDouble();
  info.principal_y = line_edit_principal_point_y_->text().toDouble();
  info.skew = line_edit_skew_->text().toDouble();
  info.radial1 = line_edit_radial1_->text().toDouble();
  info.radial2 = line_edit_radial2_->text().toDouble();
  info.radial3 = line_edit_radial3_->text().toDouble();
  info.decentering1 = line_edit_decentering1_->text().toDouble();
  info.decentering2 = line_edit_decentering2_->text().toDouble();
  return info;
}

}
}
}
