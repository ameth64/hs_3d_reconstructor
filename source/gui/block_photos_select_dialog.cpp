#include "gui/block_photos_select_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlockPhotosSelectDialog::BlockPhotosSelectDialog(
  const QString& block_name, const QString& block_description,
  const GroupEntryContainer& groups)
{
  layout_ = new QVBoxLayout(this);

  block_photos_select_widget_ =
    new BlockPhotosSelectWidget(block_name, block_description, groups);
  layout_->addWidget(block_photos_select_widget_);

  dialog_button_box_ok_cancel_ =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                         this);
  layout_->addWidget(dialog_button_box_ok_cancel_);
}

}
}
}
