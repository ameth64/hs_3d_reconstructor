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
  typedef EIGEN_STD_MAP(size_t, Keyset) KeysetMap;

protected:
  typedef std::map<size_t, std::set<size_t> > MatchGuide;

public:
  FeatureMatchStep();

protected:
  int GuideMatchesByPos(WorkflowStepConfig* config, MatchGuide& match_guide);
};

}
}
}

#endif