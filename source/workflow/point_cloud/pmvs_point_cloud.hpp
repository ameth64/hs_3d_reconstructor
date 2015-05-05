#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_PMVS_POINT_CLOUD_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_PMVS_POINT_CLOUD_HPP_

#include <memory>

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
typedef double Scalar;

};

typedef std::shared_ptr<PointCloudConfig> PointCloudConfigPtr;

class HS_EXPORT PMVSPointCloud : public WorkflowStep
{
public:
  PMVSPointCloud();

};

}//namespace workflow
}
}


#endif // !_HS_3D_RECONSTRUCTOR_WORKFLOW_INCREMENTAL_POINT_CLOUD_HPP_
