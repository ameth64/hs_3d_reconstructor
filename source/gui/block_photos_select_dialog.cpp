#include "gui/block_photos_select_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlockPhotosSelectDialog::BlockPhotosSelectDialog(
  const BlockInfoEntry& block_info_entry, const GroupEntryContainer& groups)
{
  layout_ = new QVBoxLayout(this);

  block_photos_select_widget_ =
    new BlockPhotosSelectWidget(block_info_entry, groups);
  layout_->addWidget(block_photos_select_widget_);

  dialog_button_box_ok_cancel_ =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                         this);
  layout_->addWidget(dialog_button_box_ok_cancel_);

  QObject::connect(
    dialog_button_box_ok_cancel_, &QDialogButtonBox::accepted,
    this, &QDialog::accept);
  QObject::connect(
    dialog_button_box_ok_cancel_, &QDialogButtonBox::rejected,
    this, &QDialog::reject);
}

BlockPhotosSelectDialog::BlockInfoEntry
BlockPhotosSelectDialog::GetBlockInfo() const
{
  return block_photos_select_widget_->GetBlockInfo();
}

void BlockPhotosSelectDialog::GetSelectedPhotoIds(
  std::vector<uint>& selected_photo_ids) const
{
  block_photos_select_widget_->GetSelectedPhotoIds(selected_photo_ids);
}

}
}
}
