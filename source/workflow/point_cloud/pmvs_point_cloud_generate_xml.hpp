#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_PMVS_POINT_CLOUD_GENERATE_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_PMVS_POINT_CLOUD_GENERATE_HPP_

#include <memory>
#include <string>

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/common/workflow_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

#include <string>
#include <vector>

struct PhotoList
{
  bool used;
  int id;
  int group_id;
  std::string file;
  double rotation_matrix[9]; //列主序
  double cx,cy,cz;
  PhotoList()
    :used(false),id(-1),group_id(-1),cx(0.0),cy(0.0),cz(0.0)
  {
    for (int i = 0; i < 9; ++i)
    {
      rotation_matrix[i] = 0.0;
    }
  }
};

struct PointCloudInputData
{
  //share_section
  std::string workspace;
  int parallel_number;
  int pyramid_level;
  int patch_density;
  int patch_range;
  std::vector<PhotoList> photo_list;
  //TODO 暂时不考虑多个相机
  int camera_id;
  double intrinsic_matrix[9]; //列主序
  double k1,k2,k3;
  double d1,d2;

  //cluster_section
  float accuracy_threshold;
  float coverage_threshold;
  int cluster_size;
  std::string sparse_cloud;

  //pmvs_section
  int pmvs_visibility_threshold;
  float pmvs_consistency_threshold;
  float pmvs_group_threshold;

  //merge_section
  int merge_quality_threshold;
  int merge_visibility_threshold;

  PointCloudInputData()
    :parallel_number(3),pyramid_level(3),patch_density(2),patch_range(7),
    accuracy_threshold(0.7f),coverage_threshold(0.7f),cluster_size(50),
    pmvs_visibility_threshold(2),pmvs_consistency_threshold(0.7f),pmvs_group_threshold(0.001f),
    merge_quality_threshold(1),merge_visibility_threshold(2)
  {}

  ~PointCloudInputData(){}

};


class HS_EXPORT PointCloudGenerateXml
{
public:
  static bool ReadIntrinsic(const std::string& filename, PointCloudInputData& pcid);
  static bool ReadExtrinsic(const std::string& filename, PointCloudInputData& pcid);

};

}
}
}

#endif