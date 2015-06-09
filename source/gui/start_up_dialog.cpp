#include "gui/start_up_dialog.hpp"
#include "gui/main_window.hpp"

namespace hs
{
  namespace recon
  {
    namespace gui
    {

      StartUpDialog::StartUpDialog(QWidget* parent, Qt::WindowFlags f) 
        : QDialog(parent, f)
      {
        this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
        layout_main_ = new QVBoxLayout;
        button_create_project_ = new QPushButton(tr("Create Project..."), this);
        button_open_project_ = new QPushButton(tr("Open Project..."), this);
        button_cancel_ = new QPushButton(tr("Cancel"), this);
        layout_main_->addWidget(button_create_project_);
        layout_main_->addWidget(button_open_project_);
        layout_main_->addWidget(button_cancel_);
        this->setLayout(layout_main_);



        this->setWindowTitle(tr("Quick Start"));

        QObject::connect(button_create_project_, &QPushButton::clicked
          , this,&StartUpDialog::OnButtonCreateProjectClicked);
        QObject::connect(button_open_project_, &QPushButton::clicked
          , this, &StartUpDialog::OnButtonOpenProjectClicked);
        QObject::connect(button_cancel_, &QPushButton::clicked
          , this, &StartUpDialog::OnButtonCancelClicked);
      }

      void StartUpDialog::OnButtonCreateProjectClicked()
      {
        NewProjcet();
      }

      void StartUpDialog::OnButtonOpenProjectClicked()
      {
        OpenProject();
      }

      void StartUpDialog::OnButtonCancelClicked()
      {
        exit(0);
      }

    }
  }
}