#ifndef _HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_
#define _HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxlayout>
#include <QGroupBox>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QToolBar>
#include <QKeyEvent>

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
        QString StrippedName(const QString &full_file_name);

      private:
        void UpdateRecentFileActions();

        QHBoxLayout* layout_main_;
        QVBoxLayout* layout_group_quick_start_;
        QVBoxLayout* layout_group_recent_files_;
        QGroupBox* groupbox_quick_start_;
        QGroupBox* groupbox_recent_files_;
        QPushButton* button_create_project_;
        QPushButton* button_open_project_;
        QPushButton* button_tools_;
        QPushButton* button_cancel_;
        enum { MaxRecentFiles = 5 };
        QToolBar* menu_recent_file_[MaxRecentFiles];
        QAction *recent_file_acts_[MaxRecentFiles];

      private slots:
        void OnButtonCreateProjectClicked();
        void OnButtonOpenProjectClicked();
        void OnButtonCancelClicked();
        void OnButtonToolsClicked();
        void OnRecentProjectClicked();

      public slots:
        void SetCurrentFile(const QString& file_neam);
        //这里实例化reject函数的作用时用来禁止按下esc键之后调用QDialog的reject而关闭窗口！
        void keyPressEvent(QKeyEvent *e);
        
      signals:
        void NewProjcet();
        void OpenProject();
        void OpenTools();
        void OpenProjectWithFile(const QString &db_file);

      };
    }
  }
}

#endif //_HS_3D_RECONSTRUCTOR_START_UP_DIALOG_HPP_