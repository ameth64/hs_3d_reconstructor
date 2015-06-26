#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_OPENCV_FEATURE_MATCH_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_OPENCV_FEATURE_MATCH_HPP_

#include <opencv2/core/core.hpp>

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/feature_match/feature_match_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT OpenCVFeatureMatch : public FeatureMatchStep
{
public:
  typedef FeatureMatchStep::Keyset Keyset;
  typedef FeatureMatchStep::KeysetMap KeysetMap;
protected:
  typedef FeatureMatchStep::MatchGuide MatchGuide;
public:
  OpenCVFeatureMatch();

private:
  int DetectFeature(WorkflowStepConfig* config, KeysetMap& keysets);
  int MatchFeatures(WorkflowStepConfig* config,
                    const KeysetMap& keysets,
                    const MatchGuide& match_guide,
                    hs::sfm::MatchContainer& matches);
  int FilterMatches(WorkflowStepConfig* config,
                    const KeysetMap& keysets,
                    const hs::sfm::MatchContainer& matches_initial,
                    hs::sfm::MatchContainer& matches_filtered);
#if 1
  int FilterMatchesOpenMVG(WorkflowStepConfig* config,
                           const KeysetMap& keysets,
                           const hs::sfm::MatchContainer& matches_initial,
                           hs::sfm::MatchContainer& matches_filtered);
#endif
private:
  static cv::Mat LoadDescriptors(size_t number_of_keys,
                                 const std::string& descriptor_path);

protected:
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif
