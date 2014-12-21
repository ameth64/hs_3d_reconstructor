#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_PANE_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_PANE_HPP_

#include <QAction>
#include <QIcon>

#include "database/database_mediator.hpp"
#include "gui/manager_pane.hpp"
#include "gui/photos_tree_widget.hpp"

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

private:
  void OnActionAddPhotogroupTriggered();

public:
  PhotosTreeWidget* photos_tree_widget_;

  QIcon icon_add_photogroup_;
  QIcon icon_add_photos_;
  QIcon icon_remove_photogroup_;
  QIcon icon_remove_photos_;

  QAction* action_add_photogroup_;
  QAction* action_add_photos_;
  QAction* action_remove_photogroup_;
  QAction* action_remove_photos_;
};

}
}
}

#endif
