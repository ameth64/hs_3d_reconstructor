#ifndef _HS_3D_RECONSTRUCTOR_BLOCK_INFO_WIDGET_HPP_
#define _HS_3D_RECONSTRUCTOR_BLOCK_INFO_WIDGET_HPP_

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

namespace hs
{
namespace recon
{
namespace gui
{

class BlockInfoWidget : public QWidget
{
  Q_OBJECT
public:
  struct BlockInfoEntry
  {
    QString name;
    QString description;
  };
public:
  BlockInfoWidget(const BlockInfoEntry& block_info_entry,
                  QWidget* parent = 0);

  BlockInfoEntry GetBlockInfo() const;

private:
  QLabel* label_name_;
  QLabel* label_description_;
  QLineEdit* line_edit_name_;
  QTextEdit* text_edit_description_;
  QGridLayout* layout_;
};

}
}
}

#endif
