#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_OPENMVG_FEATURE_MATCH_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_OPENMVG_FEATURE_MATCH_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/feature_match/feature_match_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT OpenMVGFeatureMatch : public FeatureMatchStep
{
public:
  typedef FeatureMatchStep::Keyset Keyset;
  typedef FeatureMatchStep::KeysetContainer KeysetContainer;
protected:
  typedef FeatureMatchStep::MatchGuide MatchGuide;
  typedef FeatureMatchStep::RandomAccessMatchGuide RandomAccessMatchGuide;
public:
  OpenMVGFeatureMatch();

private:
  int DetectFeature(WorkflowStepConfig* config, KeysetContainer& keysets);
  int MatchFeatures(WorkflowStepConfig* config,
                    const MatchGuide& match_guide,
                    hs::sfm::MatchContainer& matches);
  int FilterMatches(WorkflowStepConfig* config,
                    const KeysetContainer& keysets,
                    const hs::sfm::MatchContainer& matches_initial,
                    hs::sfm::MatchContainer& matches_filtered);

protected:
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif