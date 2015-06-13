#ifndef _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_IMPORT_PROGRESS_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_IMPORT_PROGRESS_DIALOG_HPP_


#include <QDialog>
#include <ctime>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
        void SetStartTime(const time_t start_time);
        void SetRange(const int begin, const int end);
        public slots:
        void SetStatus(const int progress);
      private:
        time_t start_time_;
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
      };
   }
  }
}
#endif // _HS_3D_RECONSTRUCTOR_GUI_PHOTOS_IMPORT_PROGRESS_DIALOG_HPP_
 