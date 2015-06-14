#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTO_IMPORT_CHECK_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTO_IMPORT_CHECK_WIDGET_HPP_

#include <map>
#include <string>
#include <set>
#include <iostream>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QTreeView>
#include <QHeaderView>
#include <QMessageBox>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace hs
{
namespace recon
{
namespace gui
{
  typedef int PhotoID;
  typedef std::pair<int, std::string> PhotoID_Path;
  typedef std::map<int, std::string> PhotoID_Paths;
  typedef std::multimap<std::string, PhotoID_Path> PhotoClusterpath_ID_Paths;

  class PhotoItem: public QTreeWidgetItem
  {
  public:
    PhotoItem(int photo_id, QString photo_path);
    ~PhotoItem();

    int photo_id() const;
    QString photo_path() const;

  private:
    bool is_exist_;
    int photo_id_;
    QString photo_path_;
  };

  class ClusterItem: public QTreeWidgetItem
  {
  public:
    ClusterItem();
    ~ClusterItem();

    void set_is_ok(bool is_ok);
    bool is_ok() const;

  private:
    bool is_ok_;
  };

  class ClusterWidget:public QWidget
  {
    Q_OBJECT
  public:
    ClusterWidget(const QString cluster_name,
      ClusterItem* item, QWidget *parent = 0);
    ~ClusterWidget();

    QString cluster_dir() const;

    public slots:
    void OnClicked();

signals:
    void UpdateClusterDir(const QString cluster_dir, ClusterItem* item);

  private:
    QHBoxLayout* layout_;
    QLabel* label_cluster_name_;
    QLineEdit* line_edit_;
    QString cluster_dir_;
    QPushButton* button_;
    ClusterItem* item_;
  };

  class PhotoImportCheckWidget: public QWidget
  {
    Q_OBJECT
  public:
    PhotoImportCheckWidget(std::map<int, std::string>& photo_id_path, QWidget *parent = 0);
    ~PhotoImportCheckWidget();

    void UpdateClusterDir(QString cluster_dir, ClusterItem* item);
    const std::map<int, std::string>& PhotoIDPaths() const;

signals:
    void PhotoReModify(const QString &db_file);

  private:
    //Cluster the photo path
    void ClusterPhotoPath();

  private:
    QTreeWidget* tree_widget_;
    QVBoxLayout* main_v_layout_;

    std::map<int, std::string> photo_id_path_;
    PhotoClusterpath_ID_Paths photo_cluster_id_name_;

  };
}
}
}

#endif //_HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_