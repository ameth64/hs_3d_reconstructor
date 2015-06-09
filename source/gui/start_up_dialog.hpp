#ifndef _HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

namespace hs
{
  namespace recon
  {
    namespace gui
    {
      class StartUpDialog :public QDialog
      {
        Q_OBJECT
      public:
        StartUpDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

      private:
        QVBoxLayout* layout_main_;
        QPushButton* button_create_project_;
        QPushButton* button_open_project_;
        QPushButton* button_cancel_;

      private slots:
        void OnButtonCreateProjectClicked();
        void OnButtonOpenProjectClicked();
        void OnButtonCancelClicked();

      signals:
        void NewProjcet();
        void OpenProject();


      };
    }
  }
}

#endif //_HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_