#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_OPENCV_FEATURE_MATCH_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_OPENCV_FEATURE_MATCH_HPP_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "hs_sfm/sfm_utility/key_type.hpp"
#include "hs_sfm/sfm_utility/match_type.hpp"

#include <opencv2/core/core.hpp>

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/common/workflow_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT FeatureMatchConfig : public WorkflowStepConfig
{
public:
  struct PosEntry
  {
    double x;
    double y;
    double z;
  };
public:
  FeatureMatchConfig();

public:
  void set_image_paths(const std::vector<std::string>& image_paths);
  void set_key_paths(const std::vector<std::string>& key_paths);
  void set_descripor_paths(const std::vector<std::string>& descriptor_paths);
  void set_matches_path(const std::string matches_path);
  void set_keys_limits(int keys_limits);
  void set_pos_entries(const std::map<size_t, PosEntry>& pos_entries);
  void set_number_of_threads(int number_of_threads);

  const std::vector<std::string>& image_paths() const;
  const std::vector<std::string>& key_paths() const;
  const std::vector<std::string>& descriptor_paths() const;
  const std::string& matches_path() const;
  int keys_limits() const;
  const std::map<size_t, PosEntry>& pos_entries() const;
  int number_of_threads() const;

private:
  std::vector<std::string> image_paths_;
  std::vector<std::string> key_paths_;
  std::vector<std::string> descriptor_paths_;
  std::string matches_path_;
  int keys_limits_;
  std::map<size_t, PosEntry> pos_entries_;
  int number_of_threads_;
};
typedef std::shared_ptr<FeatureMatchConfig> FeatureMatchConfigPtr;

class HS_EXPORT OpenCVFeatureMatch : public WorkflowStep
{
public:
  typedef hs::sfm::ImageKeys<double> Keyset;
  typedef EIGEN_STD_VECTOR(Keyset) KeysetContainer;
private:
  typedef std::vector<std::set<size_t> > MatchGuide;
  typedef std::vector<std::vector<size_t> > RandomAccessMatchGuide;
public:
  OpenCVFeatureMatch();

private:
  int DetectFeature(WorkflowStepConfig* config, KeysetContainer& keysets);
  int GuideMatchesByPos(WorkflowStepConfig* config, MatchGuide& match_guide);
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
