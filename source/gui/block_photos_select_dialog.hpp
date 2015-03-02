#ifndef _HS_3D_RECONSTRUCTOR_GUI_BLOCK_PHOTOS_SELECT_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_BLOCK_PHOTOS_SELECT_DIALOG_HPP_

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "gui/block_photos_select_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class BlockPhotosSelectDialog : public QDialog
{
  Q_OBJECT
public:
  typedef BlockPhotosSelectWidget::PhotoEntry PhotoEntry;
  typedef BlockPhotosSelectWidget::GroupEntry GroupEntry;
  typedef BlockPhotosSelectWidget::GroupEntryContainer GroupEntryContainer;
  typedef BlockPhotosSelectWidget::BlockInfoEntry BlockInfoEntry;

public:
  BlockPhotosSelectDialog(const BlockInfoEntry& block_info_entry,
                          const GroupEntryContainer& groups);

  BlockInfoEntry GetBlockInfo() const;

  void GetSelectedPhotoIds(std::vector<uint>& selected_photo_ids) const;

private:
  BlockPhotosSelectWidget* block_photos_select_widget_;
  QDialogButtonBox* dialog_button_box_ok_cancel_;
  QVBoxLayout* layout_;
};

}
}
}

#endif
