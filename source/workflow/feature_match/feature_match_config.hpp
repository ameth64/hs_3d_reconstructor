#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_FEATURE_MATCH_CONFIG_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_FEATURE_MATCH_CONFIG_HPP_

#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>

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

}
}
}
#endif