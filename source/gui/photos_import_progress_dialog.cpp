#include "photos_import_progress_dialog.hpp"
namespace hs
{
  namespace recon
  {
    namespace gui
    {
      ProgressDialog::ProgressDialog(QDialog *parent, Qt::WindowFlags f) :
        QDialog(parent, f)
      {

        layout_main_ = new QVBoxLayout;
        layout_progress_bar_ = new QHBoxLayout;
        layout_others_ = new QHBoxLayout;

        label_progress_ = new QLabel(tr("Progress:"));
        label_progress_->setAlignment(Qt::AlignRight);

        label_time_used_ = new QLabel(tr("Time Spent(sec):"));
        label_time_used_->setAlignment(Qt::AlignRight);

        label_time_left_ = new QLabel(tr("Time Left(sec):"));
        label_time_left_->setAlignment(Qt::AlignRight);

        time_used_ = new QLabel;
        time_used_->setAlignment(Qt::AlignLeft);

        time_left_ = new QLabel;
        time_left_->setAlignment(Qt::AlignLeft);

        button_cancel_ = new QPushButton(tr("Cancel"));

        progressbar_ = new QProgressBar;
        progressbar_->setFormat("%p%");

        layout_progress_bar_->addWidget(label_progress_);
        layout_progress_bar_->addWidget(progressbar_);

        layout_others_->addWidget(label_time_used_);
        layout_others_->addWidget(time_used_);

        layout_others_->addWidget(label_time_left_);
        layout_others_->addWidget(time_left_);

        layout_others_->addWidget(button_cancel_);

        layout_main_->addLayout(layout_progress_bar_);
        layout_main_->addLayout(layout_others_);
        this->setLayout(layout_main_);
        //    StartUpDialog* start_up_dlg=new StartUpDialog;
        //    start_up_dlg->show();
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        resize(400, 150);

      }

      void ProgressDialog::SetStartTime(const time_t start_time)
      {
        start_time_ = start_time;
      }

      void ProgressDialog::SetRange(const int begin, const int end)
      {
        progressbar_->setRange(begin, end);
      }

      void ProgressDialog::SetStatus(const int progress)
      {
        progressbar_->setValue(progress);
        double p = double(progressbar_->maximum() - progressbar_->minimum()) /
          double(progress - progressbar_->minimum());
        if (start_time_)
        {
          time_t now;
          time(&now);
          time_t used = now - start_time_;
          time_used_->setText(QString::number(int(used)));
          double left = double(used)*(p - 1);
          time_left_->setText(QString::number(int(left)));
        }
      }

    }
  }
}