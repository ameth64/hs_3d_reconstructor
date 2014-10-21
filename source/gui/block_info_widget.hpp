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

class BlockInfoWidget : public QWidget
{
  Q_OBJECT
public:
  BlockInfoWidget(const QString& name,
                  const QString& description,
                  QWidget* parent = 0);

signals:
  void UpdateBlock(const QString& name, const QString& description);

private slots:
  void OnUpdateClicked();

private:
  QLabel* label_name_;
  QLabel* label_description_;
  QLineEdit* line_edit_name_;
  QTextEdit* text_edit_description_;
  QPushButton* push_button_update_;
  QGridLayout* layout_;
};

}
}

#endif
