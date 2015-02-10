#ifndef _HS_3D_RECONSTRUCTOR_MANAGER_PANE_HPP_
#define _HS_3D_RECONSTRUCTOR_MANAGER_PANE_HPP_

#include <QDockWidget>
#include <QSplitter>
#include <QMainWindow>
#include <QToolBar>

namespace hs
{
namespace recon
{
namespace gui
{

class ManagerPane : public QDockWidget
{
  Q_OBJECT
public:
  ManagerPane(const QString& title, QWidget* parent = 0);

public:
  void SetToolBar(QToolBar* tool_bar);
  void AddWidget(QWidget* widget);
  void InsertWidget(int index, QWidget* widget);
  void RemoveWidget(QWidget* widget);
  void ReplaceWidget(int index, QWidget* widget);
  QWidget* GetWidget(int index);
  int WidgetsCount();
  Qt::Orientation WidgetsOrientation() const;

protected slots:
  void OnDockLocationChanged(Qt::DockWidgetArea area);

protected:
  QSplitter* splitter_;
  QMainWindow* main_window_;
  QToolBar* tool_bar_;
};

}
}
}

#endif
