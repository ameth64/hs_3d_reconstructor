#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_FEATURE_MATCH_STEP_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_FEATURE_MATCH_STEP_HPP_

#include <set>
#include <limits>

#include "hs_sfm/sfm_utility/key_type.hpp"
#include "hs_sfm/sfm_utility/match_type.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/common/workflow_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT FeatureMatchStep : public WorkflowStep
{
public:
  typedef hs::sfm::ImageKeys<double> Keyset;
  typedef EIGEN_STD_VECTOR(Keyset) KeysetContainer;

protected:
  typedef std::vector<std::set<size_t> > MatchGuide;
  typedef std::vector<std::vector<size_t> > RandomAccessMatchGuide;

public:
  FeatureMatchStep();

protected:
  int GuideMatchesByPos(WorkflowStepConfig* config, MatchGuide& match_guide);
};

}
}
}

#endif