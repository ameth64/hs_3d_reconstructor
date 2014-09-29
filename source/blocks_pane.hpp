#ifndef _HS_3D_RECONSTRUCTOR_BLOCKS_PANE_HPP_
#define _HS_3D_RECONSTRUCTOR_BLOCKS_PANE_HPP_

#include "manager_pane.hpp"

namespace hs
{
namespace recon
{

class BlocksPane : public ManagerPane
{
  Q_OBJECT
public:
  BlocksPane(QWidget* parent = 0);

private:
  void CreateToolBar();
};

}
}

#endif
