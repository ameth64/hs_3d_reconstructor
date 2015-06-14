#include "gui/photo_import_check_dialog.hpp"

namespace hs
{
namespace recon
{
namespace gui
{
  PhotoImportCheckDialog::PhotoImportCheckDialog(
    std::map<int, std::string>& photo_id_path)
  {
    resize(800, 600);
    main_v_layout_ = new QVBoxLayout;
    setLayout(main_v_layout_);

    photo_import_check_widget_ = new PhotoImportCheckWidget(photo_id_path);
    main_v_layout_->addWidget(photo_import_check_widget_);

    QHBoxLayout* control_v_layout = new QHBoxLayout;
    push_button_ok_ = new QPushButton(tr("OK"), this);
    push_button_cancel_ = new QPushButton(tr("Cancel"), this);
    control_v_layout->addWidget(push_button_ok_);
    control_v_layout->addWidget(push_button_cancel_);

    control_v_layout->setAlignment(Qt::AlignRight);
    main_v_layout_->addLayout(control_v_layout);

    connect(push_button_ok_, &QPushButton::clicked,
             this, &QDialog::accept);
    connect(push_button_cancel_, &QPushButton::clicked,
            this, &QDialog::reject);

  }
  PhotoImportCheckDialog::~PhotoImportCheckDialog()
  {

  }
  const std::map<int, std::string>& 
    PhotoImportCheckDialog::PhotoIDPaths() const
  {
    return photo_import_check_widget_->PhotoIDPaths();
  }


}//namespace gui
}
}