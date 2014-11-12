#ifndef _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_INFO_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_PHOTO_ORIENTATION_INFO_WIDGET_HPP_

#include <map>

#include <QTableWidget>
#include <QIcon>

namespace hs
{
namespace recon
{

class PhotoOrientationInfoWidget : public QTableWidget
{
  Q_OBJECT
public:
  typedef double Float;
  enum PositionUnitFlag
  {
    POSITION_UNIT_NONE = 0,
    POSITION_UNIT_DEGREE,
    POSITION_UNIT_METER,
    NUMBER_OF_POSITION_UNIT_FLAGS
  };
  enum RotationUnitFlag
  {
    ROTATION_UNIT_DEGREE = 0,
    ROTATION_UNIT_RADIAN,
    NUMBER_OF_ROTATION_UNIT_FLAGS
  };
public:
  static QString position_unit_texts_[NUMBER_OF_POSITION_UNIT_FLAGS];
  static QString rotation_unit_texts_[NUMBER_OF_ROTATION_UNIT_FLAGS];
  static const int number_of_columns_ = 8;
  static QString column_names_[number_of_columns_];
public:
  struct PhotoOrientationEntry
  {
    uint photo_id;
    QString photo_name;
    Float pos_x;
    Float pos_y;
    Float pos_z;
    Float pitch;
    Float roll;
    Float heading;
    Float reprojection_error;
  };
  typedef std::map<uint, PhotoOrientationEntry> PhotoOrientationEntryContainer;
  PhotoOrientationInfoWidget(QWidget* parent = 0);

  void SetPlanarUnit(PositionUnitFlag unit);
  void SetHeightUnit(PositionUnitFlag unit);
  void SetRotationUnit(RotationUnitFlag unit);
  void SetEntries(const PhotoOrientationEntryContainer& entries);
  void Clear();

private:
  QIcon photo_icon_;
};

}
}

#endif
