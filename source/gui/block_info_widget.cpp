#include "block_info_widget.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

BlockInfoWidget::BlockInfoWidget(const BlockInfoEntry& block_info_entry,
                                 QWidget* parent)
  : QWidget(parent),
    label_name_(new QLabel(tr("Block Name:"))),
    label_description_(new QLabel(tr("Block Description:"))),
    line_edit_name_(new QLineEdit(block_info_entry.name)),
    text_edit_description_(new QTextEdit)
{
  text_edit_description_->setPlainText(block_info_entry.description);
  layout_ = new QGridLayout(this);
  layout_->addWidget(label_name_, 0, 0, Qt::AlignTop | Qt::AlignLeft);
  layout_->addWidget(line_edit_name_, 0, 1);
  layout_->addWidget(label_description_, 1, 0, Qt::AlignTop | Qt::AlignLeft);
  layout_->addWidget(text_edit_description_, 1, 1);
}

BlockInfoWidget::BlockInfoEntry
BlockInfoWidget::GetBlockInfo() const
{
  BlockInfoEntry block_info_entry;
  block_info_entry.name = line_edit_name_->text();
  block_info_entry.description = text_edit_description_->toPlainText();
  return block_info_entry;
}

}
}
}
