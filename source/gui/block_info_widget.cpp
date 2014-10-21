#include "block_info_widget.hpp"

namespace hs
{
namespace recon
{

BlockInfoWidget::BlockInfoWidget(const QString& name,
                                 const QString& description,
                                 QWidget* parent)
  : QWidget(parent),
    label_name_(new QLabel(tr("Block Name:"))),
    label_description_(new QLabel(tr("Block Description:"))),
    line_edit_name_(new QLineEdit(name)),
    text_edit_description_(new QTextEdit),
    push_button_update_(new QPushButton(tr("Update")))
{
  text_edit_description_->setPlainText(description);
  layout_ = new QGridLayout(this);
  layout_->addWidget(label_name_, 0, 0, Qt::AlignTop | Qt::AlignLeft);
  layout_->addWidget(line_edit_name_, 0, 1);
  layout_->addWidget(label_description_, 1, 0, Qt::AlignTop | Qt::AlignLeft);
  layout_->addWidget(text_edit_description_, 1, 1);
  layout_->addWidget(push_button_update_, 2, 0, 1, 2);

  QObject::connect(push_button_update_, &QAbstractButton::clicked,
                   this, &BlockInfoWidget::OnUpdateClicked);
}

void BlockInfoWidget::OnUpdateClicked()
{
  emit UpdateBlock(line_edit_name_->text(),
                   text_edit_description_->toPlainText());
}

}
}
