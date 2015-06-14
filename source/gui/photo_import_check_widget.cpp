#include "gui/photo_import_check_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

  ClusterWidget::ClusterWidget(const QString cluster_name,
    ClusterItem* item, QWidget *parent)
    :QWidget(parent), item_(item)
  {
    layout_ = new QHBoxLayout(this);
    line_edit_ = new QLineEdit(this);
    label_cluster_name_ = new QLabel(cluster_name);
    button_ = new QPushButton(tr("Browse..."), this);
    layout_->addWidget(label_cluster_name_);
    layout_->addWidget(line_edit_);
    layout_->addWidget(button_);
    setLayout(layout_);
    connect(button_, &QPushButton::clicked, this, &ClusterWidget::OnClicked);
  }

  ClusterWidget::~ClusterWidget()
  {
  }

  QString ClusterWidget::cluster_dir() const
  {
    return cluster_dir_;
  }

  void ClusterWidget::OnClicked()
  {
    cluster_dir_ = QFileDialog::getExistingDirectory(
      this,
      tr("Open Directory"),
      ".",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    emit UpdateClusterDir(cluster_dir_, item_);
    line_edit_->setText(cluster_dir_);
  }

  PhotoItem::PhotoItem(int photo_id, QString photo_path)
  :photo_id_(photo_id), photo_path_(photo_path)
  {
    setIcon(0, QIcon(":images/icon_invalid.png"));
    setText(0, photo_path_);
  }

  PhotoItem::~PhotoItem()
  {
  }

  int PhotoItem::photo_id() const
  {
    return photo_id_;
  }
  QString PhotoItem::photo_path() const
  {
    return photo_path_;
  }

  ClusterItem::ClusterItem():is_ok_(false){}
  ClusterItem::~ClusterItem(){}

  void ClusterItem::set_is_ok(bool is_ok)
  {
    is_ok_ = is_ok;
  }
  bool ClusterItem::is_ok() const { return is_ok_; }

  PhotoImportCheckWidget::PhotoImportCheckWidget(
    std::map<int, std::string>& photo_id_path, QWidget *parent)
  :QWidget(parent), photo_id_path_(photo_id_path)
  {
    resize(600, 500);
    tree_widget_ = new QTreeWidget;
    tree_widget_->clear();
    tree_widget_->header()->hide();
    main_v_layout_ = new QVBoxLayout;
    setLayout(main_v_layout_);

    QLabel* label_main = new QLabel(
      tr("The path of the following photo group needs to be changed!"));
    main_v_layout_->addWidget(label_main);
    main_v_layout_->addWidget(tree_widget_);

    //Cluster the photo path
    for(auto iter = photo_id_path_.begin();
        iter != photo_id_path_.end(); ++iter)
    {
      boost::filesystem::path photo_path(iter->second);
      std::string photo_dir = photo_path.parent_path().string();
      std::string photo_cluster_name =
        boost::filesystem::path(photo_dir).leaf().string();
      std::string photo_name = photo_path.filename().string();

      std::pair<int, std::string> photo_id_path(
        std::make_pair(iter->first, photo_name));
      photo_cluster_id_name_.insert(
        std::make_pair(photo_cluster_name, photo_id_path));
    }

    //Add Items
    for(auto iter = photo_cluster_id_name_.begin();
        iter != photo_cluster_id_name_.end();)
    {
      //Add Cluster Item

      ClusterItem* top_item = new ClusterItem;
      ClusterWidget* cluster_widget = new ClusterWidget(
        QString::fromStdString(iter->first), top_item);

      connect(cluster_widget, &ClusterWidget::UpdateClusterDir,
        this, &PhotoImportCheckWidget::UpdateClusterDir);

      top_item->setIcon(0, QIcon(":images/icon_invalid.png"));
      top_item->setSizeHint(0, cluster_widget->sizeHint());

      size_t mun_key = photo_cluster_id_name_.count(iter->first);
      //for Add Photos item to Cluster Item
      for(size_t i = 0; i < mun_key; ++i)
      {
        auto iter_photo_id = photo_id_path_.find(iter->second.first);
        if(iter_photo_id == photo_id_path_.end()) continue;

        PhotoItem* photo_item = new PhotoItem(iter->second.first,
          QString::fromStdString(iter_photo_id->second));
        top_item->addChild(photo_item);
        iter++;
      }

      tree_widget_->addTopLevelItem(top_item);
      tree_widget_->setItemWidget(top_item, 0, cluster_widget);
    }
  }

  PhotoImportCheckWidget::~PhotoImportCheckWidget()
  {

  }


  void PhotoImportCheckWidget::UpdateClusterDir(
    QString cluster_dir, ClusterItem* item)
  {
    int mun_child = item->childCount();
    bool is_cluster_all_right = false;
    for(int i = 0; i < mun_child; ++i)
    {
      PhotoItem* child_item = (PhotoItem*)item->child(i);
      QString photo_path = child_item->photo_path();

      boost::filesystem::path photo_name(photo_path.toStdString());

      std::string photo_new_path =
        boost::str(boost::format("%1%/%2%") %
        cluster_dir.toStdString() % photo_name.filename().string());
      child_item->setText(0, QString::fromStdString(photo_new_path));

      //Checkfile
      boost::filesystem::path check_photo_path(photo_new_path);
      if(boost::filesystem::exists(check_photo_path) &&
         boost::filesystem::is_regular_file(check_photo_path))
      {
        is_cluster_all_right = true;
        child_item->setIcon(0, QIcon(":images/icon_valid.png"));

        //更新数据
        photo_id_path_.at(child_item->photo_id()) = photo_new_path;
      }
      else
      {
        is_cluster_all_right = false;
      }
    }
    if(is_cluster_all_right)
    {
      item->set_is_ok(true);
      item->setIcon(0, QIcon(":images/icon_valid.png"));
    }
  }

  const std::map<int, std::string>& 
    PhotoImportCheckWidget::PhotoIDPaths() const
  {
    return photo_id_path_;
  }


}//namespace gui
}
}