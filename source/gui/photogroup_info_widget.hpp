#ifndef _HS_3D_RECONSTRUCTOR_PHOTOGROUP_INFO_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTOGROUP_INFO_WIDGET_HPP_

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QIntValidator>
#include <QDoubleValidator>


namespace hs
{
namespace recon
{
namespace gui
{

struct PhotogroupInfo
{
  PhotogroupInfo();

  QString name;
  QString description;
  int width;
  int height;
  double focal_length;
  double pixel_size_x;
  double pixel_size_y;
  double principal_x;
  double principal_y;
  double skew;
  double radial1;;
  double radial2;
  double radial3;
  double decentering1;
  double decentering2;
};

class PhotogroupInfoWidget : public QWidget
{
  Q_OBJECT
public:
  PhotogroupInfoWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
  PhotogroupInfoWidget(const PhotogroupInfo& info,
                       QWidget* parent = 0, Qt::WindowFlags f = 0);
  void SetFromPhotogroupInfo(const PhotogroupInfo& info);
  PhotogroupInfo GetPhotogroupInfo();

private:
  void Initialize();

private:
  QVBoxLayout* layout_all_;

  QHBoxLayout* layout_name_;
  QLabel* label_name_;
  QLineEdit* line_edit_name_;

  QHBoxLayout* layout_description_;
  QLabel* label_description_;
  QTextEdit* text_edit_description_;

  QHBoxLayout* layout_dimension_;
  QLabel* label_dimension_;
  QLineEdit* line_edit_width_;
  QLineEdit* line_edit_height_;

  QHBoxLayout* layout_focal_length_;
  QLabel* label_focal_length_;
  QLineEdit* line_edit_focal_length_;

  QHBoxLayout* layout_pixel_size_;
  QLabel* label_pixel_size_;
  QLineEdit* line_edit_pixel_x_size_;
  QLineEdit* line_edit_pixel_y_size_;

  QHBoxLayout* layout_principal_point_;
  QLabel* label_principal_point_;
  QLineEdit* line_edit_principal_point_x_;
  QLineEdit* line_edit_principal_point_y_;

  QHBoxLayout* layout_skew_;
  QLabel* label_skew_;
  QLineEdit* line_edit_skew_;

  QHBoxLayout* layout_radial_;
  QLabel* label_radial_;
  QLineEdit* line_edit_radial1_;
  QLineEdit* line_edit_radial2_;
  QLineEdit* line_edit_radial3_;

  QHBoxLayout* layout_decentering_;
  QLabel* label_decentering_;
  QLineEdit* line_edit_decentering1_;
  QLineEdit* line_edit_decentering2_;

  QIntValidator int_validator_;
  QDoubleValidator double_validator_;
};

}
}
}

#endif
