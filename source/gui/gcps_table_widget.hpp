#ifndef _HS_3D_RECONSTRUCTOR_GCPS_TABLE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GCPS_TABLE_WIDGET_HPP_

#include <QTableWidget>

namespace hs
{
namespace recon
{

class GCPsTableWidget : public QTableWidget
{
  Q_OBJECT
public:
  typedef double Float;
  enum DisplayMode
  {
    SHOW_MEASUREMENT,
    SHOW_ESTIMATE,
    SHOW_ERROR
  };
  enum ValueUnit
  {
    DEGREE,
    RADIAN,
    METRE,
    FEET
  };
  struct GCPEntry
  {
    enum GCPType
    {
      CALCULATE_POINT = 0,
      CHECK_POINT,
      DISCARD_POINT
    };
    uint id;
    QString name;
    Float pos[3];
    Float err[3];
    GCPType type;
  };
  typedef std::map<uint, GCPEntry> GCPContainer;

  GCPsTableWidget(QWidget* parent = 0);
  ValueUnit value_unit();
  DisplayMode display_mode();

  int ClearGCPs();
  int AddGCPs(const GCPContainer& gcps,
              ValueUnit value_unit, DisplayMode display_mode);

public slots:
  int DeleteGCPsByIds(const std::vector<uint>& group_ids);
  int DeleteGCPsBySelectedItems();

signals:
  int SeletedGCPsDeleted(const std::vector<uint>& gcp_ids);
  int GCPSelected(uint gcp_id);

private:
  ValueUnit value_unit_;
  DisplayMode display_mode_;
};

}
}

#endif
