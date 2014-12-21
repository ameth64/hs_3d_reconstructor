#include <QToolBar>

#include "gui/photos_pane.hpp"
#include "gui/photos_import_configure_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

PhotosPane::PhotosPane(QWidget* parent)
  : ManagerPane(tr("Photos"), parent)
  , photos_tree_widget_(new PhotosTreeWidget(this))
  , icon_add_photogroup_(":/images/icon_group_add.png")
  , icon_add_photos_(":/images/icon_photo_add.png")
  , icon_remove_photogroup_(":/images/icon_group_remove.png")
  , icon_remove_photos_(":/images/icon_photo_remove.png")
{
  AddWidget(photos_tree_widget_);

  action_add_photogroup_ = new QAction(icon_add_photogroup_,
                                       tr("Add Photogroup"), this);
  action_add_photos_ = new QAction(icon_add_photos_,
                                  tr("Add Photos"), this);
  action_add_photos_->setEnabled(false);
  action_remove_photogroup_ = new QAction(icon_remove_photogroup_,
                                          tr("Remove Photogroup"), this);
  action_remove_photogroup_->setEnabled(false);
  action_remove_photos_ = new QAction(icon_remove_photos_,
                                      tr("Remove Photos"), this);
  action_remove_photos_->setEnabled(false);

  tool_bar_ = new QToolBar(this);
  tool_bar_->addAction(action_add_photogroup_);
  tool_bar_->addAction(action_add_photos_);
  tool_bar_->addAction(action_remove_photogroup_);
  tool_bar_->addAction(action_remove_photos_);

  main_window_->addToolBar(tool_bar_);

  QObject::connect(action_add_photogroup_, &QAction::triggered,
                   this, &PhotosPane::OnActionAddPhotogroupTriggered);
}

void PhotosPane::Response(int request_flag, void* response)
{
}

void PhotosPane::OnActionAddPhotogroupTriggered()
{
  PhotosImportConfigureDialog dialog;
  dialog.resize(1300, 800);
  dialog.setWindowTitle(tr("Import Photos"));
  if (dialog.exec())
  {

  }
}

}
}
}
