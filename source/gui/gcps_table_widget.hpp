#ifndef _HS_3D_RECONSTRUCTOR_GCPS_TABLE_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GCPS_TABLE_WIDGET_HPP_

#include <QTableWidget>
#include <QComboBox>

namespace hs
{
namespace recon
{
namespace gui
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
  struct GCPEntry
  {
    enum GCPType
    {
      CALCULATE_POINT = 0,
      CHECK_POINT,
      NOT_USED
    };
    uint id;
    QString name;
    Float measure_pos[3];
    Float estimate_pos[3];
    int type;
  };
  typedef std::map<uint, GCPEntry> GCPContainer;
  typedef std::map<uint, int> GCPRowMap;
  typedef std::map<uint, bool> GCPTypeEditableMap;

  GCPsTableWidget(QWidget* parent = 0);
  DisplayMode display_mode() const;
  void set_display_mode(DisplayMode display_mode);

  void ClearGCPs();
  void AddGCP(GCPEntry& gcp);
  void AddGCPs(const GCPContainer& gcps);
  void SetGCPType(uint gcp_id, int type);

public slots:
  int DeleteGCPsByIds(const std::vector<uint>& group_ids);
  int DeleteGCPsBySelectedItems();
  int UpdateGCPEstimatePos(uint gcp_id, Float x, Float y, Float z);
  void SetGCPTypeEditable(uint gcp_id, bool is_type_editable);

private slots:
  void OnCellClicked(int row, int column);
  void OnItemSelectionChanged();
  void OnCurrentIndexChanged(int index);
  void OnItemChanged(QTableWidgetItem* item);
  void SortByColumn(int column);

signals:
  void SeletedGCPsDeleted(const std::vector<uint>& gcp_ids);
  void GCPsSelected(const std::vector<uint>& gcp_ids);
  void GCPUpdated(uint gcp_id, const GCPEntry& gcp_updated);

private:
  static QStringList HeaderLabels();
  static QStringList TypeLabels();

  void RefreshTable();

private:
  int display_mode_;
  GCPContainer gcps_;

  QComboBox* combo_box_type_;
  int combo_box_row_;
  bool is_refresh_complete_;

  GCPRowMap gcp_row_map_;
  GCPTypeEditableMap gcp_type_editable_map_;
  Qt::SortOrder *sort_order_;
};

}
}
}

#endif
