#ifndef _HS_3D_RECONSTRUCTOR_BLOCKS_PANE_HPP_
#define _HS_3D_RECONSTRUCTOR_BLOCKS_PANE_HPP_

#include <QAction>
#include <QIcon>

#include "database/database_mediator.hpp"
#include "gui/manager_pane.hpp"
#include "gui/blocks_tree_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class BlocksPane : public ManagerPane, public hs::recon::db::DatabaseObserver
{
  Q_OBJECT
public:
  BlocksPane(QWidget* parent = 0);

  virtual void Response(int request_flag, void* response);

private slots:
  void OnActionAddBlockTriggered();

  void OnBlockItemSelected(uint block_id);
  void OnPhotosInOneBlockSelected(uint block_id,
                                  const std::vector<uint>& photo_ids);
  void OnPhotosItemSelected(uint block_id);
  void OnFeatureMatchItemSelected(uint feature_match_id);
  void OnPhotoOrientationItemSelected(uint photo_orientation_id);
  void OnPointCloudItemSelected(uint point_cloud_id);
  void OnSurfaceModelItemSelected(uint surface_model_id);
  void OnDEMItemSelected(uint dem_id);
  void OnTextureItemSelected(uint texture_id);
  void OnDOMItemSelected(uint dom_id);

private:
  BlocksTreeWidget* blocks_tree_widget_;

  QIcon icon_add_block_;
  QIcon icon_remove_block_;
  QIcon icon_remove_photos_;
  QIcon icon_add_workflow_;

  QAction* action_add_block_;
  QAction* action_remove_block_;
  QAction* action_remove_photos_;
  QAction* action_add_workflow_;

  uint selected_block_id_;
  std::vector<uint> photo_ids_;
  uint selected_feature_match_id_;
  uint selected_photo_orientation_id_;
  uint selected_point_cloud_id_;
  uint selected_surface_model_id_;
  uint selected_dem_id_;
  uint selected_texture_id_;
  uint selected_dom_id_;
};

}
}
}

#endif
