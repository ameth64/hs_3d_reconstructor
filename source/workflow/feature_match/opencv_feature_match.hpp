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
  typedef FeatureMatchStep::KeysetContainer KeysetContainer;
protected:
  typedef FeatureMatchStep::MatchGuide MatchGuide;
  typedef FeatureMatchStep::RandomAccessMatchGuide RandomAccessMatchGuide;
public:
  OpenCVFeatureMatch();

private:
  int DetectFeature(WorkflowStepConfig* config, KeysetContainer& keysets);
  int MatchFeatures(WorkflowStepConfig* config,
                    const MatchGuide& match_guide,
                    hs::sfm::MatchContainer& matches);
  int FilterMatches(WorkflowStepConfig* config,
                    const KeysetContainer& keysets,
                    const hs::sfm::MatchContainer& matches_initial,
                    hs::sfm::MatchContainer& matches_filtered);
private:
  static cv::Mat LoadDescriptors(const std::string& key_path,
                                 const std::string& descriptor_path);

protected:
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif
