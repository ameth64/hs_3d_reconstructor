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
  void set_number_of_threads(int number_of_threads);

  const std::string photo_orientation_path() const;
  const std::string workspace_path() const;
  int number_of_threads() const;

private:
  std::string photo_orientation_path_; //Photo_orientation 工作路径
  std::string workspace_path_; //Point Cloud 工作路径
  int number_of_threads_;

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
