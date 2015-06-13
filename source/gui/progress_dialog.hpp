#ifndef _HS_3D_RECONSTRUCTOR_GUI_PROGRESS_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PROGRESS_DIALOG_HPP_

#include <thread>
#include <mutex>
#include <type_traits>

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

#include "hs_progress/progress_utility/progress_manager.hpp"

namespace hs
{
namespace recon
{
namespace gui
{

class ProgressDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ProgressDialog(QDialog *parent = 0, Qt::WindowFlags f = 0);

public:
  hs::progress::ProgressManager* GetProgressManagerPtr();

  template <typename... Args>
  void Start(Args&& ... a)
  {
    progress_manager_.StartWorking();
    //working_thread_ =
    //  std::thread(FunctionWrapper<typename std::decay<Function>::type, Args...>,
    //              std::forward<Function>(f), std::forward<Args>(a)...);

    working_thread_ = std::thread(std::forward<Args>(a)...);
    timer_->start(100);
    QObject::connect(timer_, &QTimer::timeout,
                     this, &ProgressDialog::OnTimeout);
  }

  void Stop()
  {
    progress_manager_.StopWorking();
    if (working_thread_.joinable())
    {
      working_thread_.join();
    }
  }

private:
  void OnTimeout();

private:
  QLabel* label_progress_;
  QLabel* label_time_used_;
  QLabel* label_time_left_;
  QLabel* time_left_;
  QLabel* time_used_;
  QProgressBar* progressbar_;
  QPushButton* button_cancel_;
  QVBoxLayout* layout_main_;
  QHBoxLayout* layout_progress_bar_;
  QHBoxLayout* layout_others_;

  QTimer* timer_;

  std::thread working_thread_;

  hs::progress::ProgressManager progress_manager_;
};

}
}
}
#endif // _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_IMPORT_PROGRESS_DIALOG_HPP_
 