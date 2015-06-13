#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_PMVS_POINT_CLOUD_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_PMVS_POINT_CLOUD_HPP_

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

class HS_EXPORT PointCloudConfig : public WorkflowStepConfig
{
public:
  PointCloudConfig();

  void set_workspace_path(const std::string& workspace_path);
  void set_photo_orientation_path(const std::string& photo_orientation_path);
  void set_s_number_of_threads(int s_number_of_threads);
  void set_s_pyramid_level(int s_pyramid_level);
  void set_s_patch_density(int s_patch_density);
  void set_s_patch_range(int s_patch_range);
  void set_c_accuracy_threshold(float c_accuracy_threshold);
  void set_c_coverage_threshold(float c_coverage_threshold);
  void set_c_cluster_size(int c_cluster_size);
  void set_p_visibility_threshold(int p_visibility_threshold);
  void set_p_consistency_threshold(float p_consistency_threshold);
  void set_p_group_threshold(float p_group_threshold);
  void set_m_quality_threshold(int m_quality_threshold);
  void set_m_visibility_threshold(int m_visibility_threshold);

  const std::string photo_orientation_path() const;
  const std::string workspace_path() const;
  int s_number_of_threads() const;
  int s_pyramid_level() const;
  int s_patch_density() const;
  int s_patch_range() const;
  float c_accuracy_threshold() const;
  float c_coverage_threshold() const;
  int c_cluster_size() const;
  float p_consistency_threshold() const;
  float p_group_threshold() const;
  int p_visibility_threshold() const;
  int m_quality_threshold() const;
  int m_visibility_threshold() const;

private:
  std::string photo_orientation_path_; //Photo_orientation 工作路径
  std::string workspace_path_; //Point Cloud 工作路径
  //share_section
  int s_number_of_threads_; //线程数
  int s_pyramid_level_; //金字塔层数
  int s_patch_density_;
  int s_patch_range_;
  //cluster_section
  float c_accuracy_threshold_;
  float c_coverage_threshold_;
  int   c_cluster_size_;
  //pmvs_section
  int   p_visibility_threshold_;
  float p_consistency_threshold_;
  float p_group_threshold_;
  //merge_section
  int m_quality_threshold_;
  int m_visibility_threshold_;

};

typedef std::shared_ptr<PointCloudConfig> PointCloudConfigPtr;

class HS_EXPORT PointCloud : public WorkflowStep
{
public:
  typedef double Scalar;

  PointCloud();

 protected:
  virtual int RunImplement(WorkflowStepConfig* config);

};

}//namespace workflow
}
}


#endif // !_HS_3D_RECONSTRUCTOR_WORKFLOW_INCREMENTAL_POINT_CLOUD_HPP_
