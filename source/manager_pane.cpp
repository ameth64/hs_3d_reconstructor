#include <QResizeEvent>
#include <QMessageBox>

#include "manager_pane.hpp"

namespace hs
{
namespace recon
{

ManagerPane::ManagerPane(const QString& title, QWidget* parent)
  : QDockWidget(title, parent),
    tool_bar_(NULL),
    main_window_(new QMainWindow(0))
{
  splitter_ = new QSplitter(main_window_);
  main_window_->setCentralWidget(splitter_);
  main_window_->setParent(this);
  setWidget(main_window_);

  QObject::connect(this, &QDockWidget::dockLocationChanged,
                   this, &ManagerPane::OnDockLocationChanged);
}

void ManagerPane::SetToolBar(QToolBar* tool_bar)
{
  if (!tool_bar_ && tool_bar)
  {
    tool_bar_ = tool_bar;
    tool_bar_->setParent(main_window_);
    tool_bar_->setFloatable(false);
    tool_bar_->setMovable(false);
    main_window_->addToolBar(tool_bar_);
  }
}

void ManagerPane::AddWidget(QWidget* widget)
{
  int index = splitter_->indexOf(widget);
  if (index < 0 || index >= WidgetsCount())
  {
    splitter_->addWidget(widget);
  }
}

void ManagerPane::InsertWidget(int index, QWidget* widget)
{
  int index_1 = splitter_->indexOf(widget);
  if (index_1 < 0 || index_1 >= WidgetsCount())
  {
    splitter_->insertWidget(index, widget);
  }
}

void ManagerPane::RemoveWidget(QWidget* widget)
{
  int index = splitter_->indexOf(widget);
  if (index >= 0 && index < WidgetsCount())
  {
    widget->setParent(NULL);
  }
}

void ManagerPane::ReplaceWidget(int index, QWidget* widget)
{
  int index_exist = splitter_->indexOf(widget);
  if ((index_exist < 0 || index_exist >= WidgetsCount()) &&
      index >= 0 && index < WidgetsCount() && index_exist != index)
  {
    QWidget* widget_to_remove = splitter_->widget(index);
    if (widget_to_remove)
    {
      widget_to_remove->setParent(NULL);
      splitter_->insertWidget(index, widget);
    }
  }
}

QWidget* ManagerPane::GetWidget(int index)
{
  return splitter_->widget(index);
}

int ManagerPane::WidgetsCount()
{
  return splitter_->count();
}

Qt::Orientation ManagerPane::WidgetsOrientation() const
{
  return splitter_->orientation();
}

void ManagerPane::OnDockLocationChanged(Qt::DockWidgetArea area)
{
  if (area == Qt::LeftDockWidgetArea ||
      area == Qt::RightDockWidgetArea)
  {
    splitter_->setOrientation(Qt::Vertical);
  }
  if (area == Qt::TopDockWidgetArea ||
      area == Qt::BottomDockWidgetArea)
  {
    splitter_->setOrientation(Qt::Horizontal);
  }
}

}
}
