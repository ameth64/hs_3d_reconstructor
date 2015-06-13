#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_PANE_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_PANE_HPP_

#include <QAction>
#include <QIcon>
#include <QTimer>

#include "database/database_mediator.hpp"
#include "gui/manager_pane.hpp"
#include "gui/photos_tree_widget.hpp"
#include "gui/photogroup_info_setting_widget.hpp"
#include "gui/photo_display_widget.hpp"
//#include "gui/photos_import_progress_dialog.hpp"
#include "gui/photos_import_configure_dialog.hpp"
#include "hs_progress/progress_utility/progress_manager.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class PhotosPane : public ManagerPane, public hs::recon::db::DatabaseObserver
{
  Q_OBJECT
public:
  PhotosPane(QWidget* parent = 0);

  virtual void Response(int request_flag, void* response);

signals:
  void ProgressStatus(int i);

private slots:
  void OnActionAddPhotogroupTriggered();
  void OnActionAddPhotosTriggered();
  void OnActionRemovePhotogroupTriggered();
  void OnActionRemovePhotosTriggered();

  void OnPhotoSelected(uint photo_id);
  void OnGroupsOnlySelected(const std::vector<uint>& group_ids);
  void OnSingleGroupSelected(uint group_id);
  void OnPhotosOnlySelected(const std::vector<uint>& photo_ids);
  void OnNothingSelected();
  void OnPhotosAndGroupsSelected();

  void OnSelectedGroupsDeleted(const std::vector<uint>& group_ids);
  void OnSelectedPhotosRemoved(const std::vector<uint>& photo_ids);

  void OnPhotogroupInfoUpdated(uint id, const PhotogroupInfo& photogroup_info);
  //void SetProgress();

private:
  void ImportPhotos(
    const PhotogroupInfo &photogroup_info,
    const PhotogroupPOSConfigureWidget::POSEntryContainer &pos_entries,
    hs::progress::ProgressManager* progress_manager);


private:
  PhotosTreeWidget* photos_tree_widget_;
  PhotogroupInfoSettingWidget* photogroup_info_widget_;
  PhotoDisplayWidget* photo_display_widget_;

  QIcon icon_add_photogroup_;
  //QIcon icon_add_photos_;
  QIcon icon_remove_photogroup_;
  QIcon icon_remove_photos_;

  QAction* action_add_photogroup_;
  //QAction* action_add_photos_;
  QAction* action_remove_photogroup_;
  QAction* action_remove_photos_;
  //ProgressDialog* progress_import_photos_;
  //hs::progress::ProgressManager* progress_manager_;
  //QTimer* timer_;
};

}
}
}

#endif
