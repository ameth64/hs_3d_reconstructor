#include <gtest/gtest.h>

#include "workflow/point_cloud/pmvs_point_cloud.hpp"

namespace
{

TEST(TestPMVSPointCloud, SimpleTest)
{
  hs::recon::workflow::PointCloudConfig config;
  config.set_workspace_path(std::string("E:/workspace/try/try_hs_3d_reconstructor/build/Release/test_fangcheng2_block_flowmodule/photo_orientation/1/"));
  config.set_photo_orientation_path(std::string("E:/workspace/try/try_hs_3d_reconstructor/build/Release/test_fangcheng2_block_flowmodule/point_cloud/8/"));
  config.set_number_of_threads(6);

  hs::recon::workflow::PointCloud point_cloud;
  point_cloud.RunImplement(&config);
}

}