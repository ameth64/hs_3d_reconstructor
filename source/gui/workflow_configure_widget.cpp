#include "gui/workflow_configure_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

WorkflowConfigureWidget::WorkflowConfigureWidget(
  int start_configure_type, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , start_configure_type_(start_configure_type)
{
  h_layout_main_ = new QHBoxLayout;
  v_layout_widget_ = new QVBoxLayout;
  h_layout_config_ = new QHBoxLayout;
  h_layout_export_ = new QHBoxLayout;
  h_layout_button_ = new QHBoxLayout;
  button_next_ = new QPushButton(tr("Next"));
  button_previous_ = new QPushButton(tr("Previouse"));
  v_layout_widget_->addLayout(h_layout_config_);
  v_layout_widget_->addLayout(h_layout_export_);
  v_layout_widget_->addLayout(h_layout_button_);
  h_layout_button_->addWidget(button_previous_);
  h_layout_button_->addWidget(button_next_);
  this->setLayout(h_layout_main_);
  tree_widget_ = new QTreeWidget;
  tree_widget_->setHeaderHidden(true);
  tree_widget_->setColumnCount(1);
  h_layout_main_->addWidget(tree_widget_);
  h_layout_main_->addLayout(v_layout_widget_);

  QStringList string_list_feature_match;
  string_list_feature_match << tr("Feature Match");
  tree_item_feature_match_ = new QTreeWidgetItem(string_list_feature_match);
  tree_item_feature_match_->setCheckState(0, Qt::Checked);

  QStringList string_list_photo_orientation;
  string_list_photo_orientation << tr("Photo Orientation");
  tree_item_photo_orientation_ =
    new QTreeWidgetItem(string_list_photo_orientation);
  tree_item_photo_orientation_->setCheckState(0, Qt::Checked);

  QStringList string_list_point_cloud;
  string_list_point_cloud << tr("Point Cloud");
  tree_item_point_cloud_ = new QTreeWidgetItem(string_list_point_cloud);
  tree_item_point_cloud_->setCheckState(0, Qt::Checked);

  QStringList string_list_surface_model;
  string_list_surface_model << tr("Surface Model");
  tree_item_surface_model_ = new QTreeWidgetItem(string_list_surface_model);
  tree_item_surface_model_->setCheckState(0, Qt::Checked);

  QStringList string_list_texture;
  string_list_texture << tr("Texture");
  tree_item_texture_ = new QTreeWidgetItem(string_list_texture);
  tree_item_texture_->setCheckState(0, Qt::Checked);

  tree_item_feature_match_->addChild(tree_item_photo_orientation_);
  tree_item_photo_orientation_->addChild(tree_item_point_cloud_);
  tree_item_point_cloud_->addChild(tree_item_surface_model_);
  tree_item_surface_model_->addChild(tree_item_texture_);
  if (start_configure_type_ == CONFIGURE_FEATURE_MATCH)
  {
    tree_widget_->addTopLevelItem(tree_item_feature_match_);
  }
  else if (start_configure_type_ == CONFIGURE_PHOTO_ORIENTATION)
  {
    tree_item_feature_match_->removeChild(tree_item_photo_orientation_);
    tree_widget_->addTopLevelItem(tree_item_photo_orientation_);
    delete tree_item_feature_match_;
    tree_item_feature_match_ = nullptr;
  }
  else if (start_configure_type_ == CONFIGURE_POINT_CLOUD)
  {
    tree_item_photo_orientation_->removeChild(tree_item_point_cloud_);
    tree_widget_->addTopLevelItem(tree_item_point_cloud_);
    delete tree_item_photo_orientation_;
    tree_item_photo_orientation_ = nullptr;
    delete tree_item_feature_match_;
    tree_item_feature_match_ = nullptr;
  }
  else if (start_configure_type_ == CONFIGURE_SURFACE_MODEL)
  {
    tree_item_point_cloud_->removeChild(tree_item_surface_model_);
    tree_widget_->addTopLevelItem(tree_item_surface_model_);
    delete tree_item_feature_match_;
    tree_item_feature_match_ = nullptr;
    delete tree_item_photo_orientation_;
    tree_item_photo_orientation_ = nullptr;
    delete tree_item_point_cloud_;
    tree_item_point_cloud_ = nullptr;
  }
  else if (start_configure_type_ == CONFIGURE_TEXTURE)
  {
    tree_item_surface_model_->removeChild(tree_item_texture_);
    tree_widget_->addTopLevelItem(tree_item_texture_);
    delete tree_item_feature_match_;
    tree_item_feature_match_ = nullptr;
    delete tree_item_photo_orientation_;
    tree_item_photo_orientation_ = nullptr;
    delete tree_item_point_cloud_;
    tree_item_point_cloud_ = nullptr;
    delete tree_item_surface_model_;
    tree_item_surface_model_ = nullptr;
  }

  tree_widget_->expandAll();

  feature_match_configure_widget_ =
    new FeatureMatchConfigureWidget(this);
  feature_match_configure_widget_->setMinimumSize(300, 400);
  feature_match_configure_widget_->hide();
  h_layout_config_->addWidget(feature_match_configure_widget_);
  photo_orientation_configure_widget_ =
    new PhotoOrientationConfigureWidget(this);
  photo_orientation_configure_widget_->setMinimumSize(300, 400);
  photo_orientation_configure_widget_->hide();
  h_layout_config_->addWidget(photo_orientation_configure_widget_);
  point_cloud_configure_widget_ =
    new PointCloudConfigureWidget(this);
  point_cloud_configure_widget_->setMinimumSize(300, 400);
  point_cloud_configure_widget_->hide();
  h_layout_config_->addWidget(point_cloud_configure_widget_);
  surface_model_configure_widget_ =
    new SurfaceModelConfigureWidget(this);
  surface_model_configure_widget_->setMinimumSize(300, 400);
  surface_model_configure_widget_->hide();
  h_layout_config_->addWidget(surface_model_configure_widget_);
  texture_configure_widget_ =
    new TextureConfigureWidget(this);
  texture_configure_widget_->setMinimumSize(300, 400);
  texture_configure_widget_->hide();
  h_layout_config_->addWidget(texture_configure_widget_);

  //feature_match_export_widget_ =
  //  new FeatureMatchExportWidget(this);
  //feature_match_export_widget_->hide();
  //h_layout_export_->addWidget(feature_match_export_widget_);
  //photo_orientation_export_widget_ =
  //  new PhotoOrientationExportWidget(this);
  //photo_orientation_export_widget_->hide();
  //h_layout_export_->addWidget(photo_orientation_export_widget_);
  //point_cloud_export_widget_ =
  //  new PointCloudExportWidget(this);
  //point_cloud_export_widget_->hide();
  //h_layout_export_->addWidget(point_cloud_export_widget_);
  //surface_model_export_widget_ =
  //  new SurfaceModelExportWidget(this);
  //surface_model_export_widget_->hide();
  //h_layout_export_->addWidget(surface_model_export_widget_);
  //texture_export_widget_ =
  //  new TextureExportWidget(this);
  //texture_export_widget_->hide();
  //h_layout_export_->addWidget(texture_export_widget_);

  QObject::connect(tree_widget_, &QTreeWidget::itemChanged,
                   this, &WorkflowConfigureWidget::SyncTreeNodeCheckState);
  QObject::connect(tree_widget_, &QTreeWidget::currentItemChanged,
                   this, &WorkflowConfigureWidget::DisplayWidget);
  QObject::connect(button_next_, &QPushButton::clicked,
                   this, &WorkflowConfigureWidget::GotoNextTreeNode);
  QObject::connect(button_previous_, &QPushButton::clicked,
                   this, &WorkflowConfigureWidget::GotoPreviousTreeNode);
}

WorkflowConfigureWidget::ConfigureType
WorkflowConfigureWidget::FirstConfigureType() const
{
  if (tree_item_feature_match_ &&
      tree_item_feature_match_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_FEATURE_MATCH;
  }
  else if (tree_item_photo_orientation_ &&
           tree_item_photo_orientation_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_PHOTO_ORIENTATION;
  }
  else if (tree_item_point_cloud_ &&
           tree_item_point_cloud_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_POINT_CLOUD;
  }
  else if (tree_item_surface_model_ &&
           tree_item_surface_model_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_SURFACE_MODEL;
  }
  else if (tree_item_texture_ &&
           tree_item_texture_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_TEXTURE;
  }
  else
  {
    return CONFIGURE_NONE;
  }
}

WorkflowConfigureWidget::ConfigureType
WorkflowConfigureWidget::LastConfigureType() const
{
  if (tree_item_texture_ &&
      tree_item_texture_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_TEXTURE;
  }
  else if (tree_item_surface_model_ &&
           tree_item_surface_model_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_SURFACE_MODEL;
  }
  else if (tree_item_point_cloud_ &&
           tree_item_point_cloud_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_POINT_CLOUD;
  }
  else if (tree_item_photo_orientation_ &&
           tree_item_photo_orientation_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_PHOTO_ORIENTATION;
  }
  else if (tree_item_feature_match_ &&
           tree_item_feature_match_->checkState(0) == Qt::Checked)
  {
    return CONFIGURE_FEATURE_MATCH;
  }
  else
  {
    return CONFIGURE_NONE;
  }
}

void WorkflowConfigureWidget::FetchFeatureMatchConfig(
  workflow::FeatureMatchConfig& feature_match_config)
{
  feature_match_configure_widget_->FetchFeatureMatchConfig(
    feature_match_config);
}

void WorkflowConfigureWidget::FetchPhotoOrientationConfig(
  workflow::PhotoOrientationConfig& photo_orientation_config)
{
  photo_orientation_configure_widget_->FetchPhotoOrientationConfig(
    photo_orientation_config);
}

void WorkflowConfigureWidget::FetchPointCloudConfig(
  workflow::PointCloudConfig& point_cloud_config)
{
  point_cloud_configure_widget_->FetchPointCloudConfig(
    point_cloud_config);
}

void WorkflowConfigureWidget::FetchSurfaceModelConfig(
  workflow::MeshSurfaceConfig& surface_model_config)
{
  surface_model_configure_widget_->FetchSurfaceModelConfig(
    surface_model_config);
}

void WorkflowConfigureWidget::SyncTreeNodeCheckState(
  QTreeWidgetItem* tree_item, int column)
{
  if (tree_item->checkState(column) == Qt::Checked)
  {
    if (tree_item->parent() != NULL)
    {
      tree_item->parent()->setCheckState(column, Qt::Checked);
    }
  }

  if (tree_item->checkState(column) == Qt::Unchecked)
  {
    if (tree_item->child(0)!=NULL)
    {
      tree_item->child(0)->setCheckState(column, Qt::Unchecked);
    }
  }

}

void WorkflowConfigureWidget::DisplayWidget(
  QTreeWidgetItem* tree_item, QTreeWidgetItem* previouse_item)
{
  feature_match_configure_widget_->hide();
  photo_orientation_configure_widget_->hide();
  point_cloud_configure_widget_->hide();
  surface_model_configure_widget_->hide();
  texture_configure_widget_->hide();

  //feature_match_export_widget_->hide();
  //photo_orientation_export_widget_->hide();
  //point_cloud_export_widget_->hide();
  //surface_model_export_widget_->hide();
  //texture_export_widget_->hide();
  if (tree_item == tree_item_feature_match_)
  {
    feature_match_configure_widget_->show();
    //feature_match_export_widget_->show();
  }
  else if (tree_item == tree_item_photo_orientation_)
  {
    photo_orientation_configure_widget_->show();
    //photo_orientation_export_widget_->show();
  }
  else if (tree_item == tree_item_point_cloud_)
  {
    point_cloud_configure_widget_->show();
    //point_cloud_export_widget_->show();
  }
  else if (tree_item == tree_item_surface_model_)
  {
    surface_model_configure_widget_->show();
    //surface_model_export_widget_->show();
  }
  else if (tree_item == tree_item_texture_)
  {
    texture_configure_widget_->show();
    //texture_export_widget_->show();
  }

  if (tree_item->child(0) == NULL)
  {
    button_next_->setDisabled(true);
  }
  else
  {
    button_next_->setDisabled(false);
  }

  if (tree_item->parent() == NULL)
  {
    button_previous_->setDisabled(true);
  }
  else
  {
    button_previous_->setDisabled(false);
  }

}

void WorkflowConfigureWidget::GotoNextTreeNode()
{
  QTreeWidgetItem *current_item = tree_widget_->currentItem();
  if (current_item->child(0) != NULL)
  {
    tree_widget_->setCurrentItem(current_item->child(0));
  }
}

void WorkflowConfigureWidget::GotoPreviousTreeNode()
{
  QTreeWidgetItem *current_item = tree_widget_->currentItem();
  if (current_item->parent() != NULL)
  {
    tree_widget_->setCurrentItem(current_item->parent());
  }
}

}
}
}
