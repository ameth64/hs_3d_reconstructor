#include "feature_match_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

FeatureMatchConfig::FeatureMatchConfig()
{
  type_ = STEP_FEATURE_MATCH;
}

void FeatureMatchConfig::set_image_paths(
  const std::map<size_t, std::string>& image_paths)
{
  image_paths_ = image_paths;
}
void FeatureMatchConfig::set_keysets_path(
  const std::string& keysets_path)
{
  keysets_path_ = keysets_path;
}
void FeatureMatchConfig::set_descripor_paths(
  const std::map<size_t, std::string>& descriptor_paths)
{
  descriptor_paths_ = descriptor_paths;
}
void FeatureMatchConfig::set_matches_path(const std::string matches_path)
{
  matches_path_ = matches_path;
}
void FeatureMatchConfig::set_keys_limits(int keys_limits)
{
  keys_limits_ = keys_limits;
}
void FeatureMatchConfig::set_pos_entries(
  const std::map<size_t, PosEntry>& pos_entries)
{
  pos_entries_ = pos_entries;
}
void FeatureMatchConfig::set_number_of_threads(int number_of_threads)
{
  number_of_threads_ = number_of_threads;
}

const std::map<size_t, std::string>& FeatureMatchConfig::image_paths() const
{
  return image_paths_;
}
const std::string& FeatureMatchConfig::keysets_path() const
{
  return keysets_path_;
}
const std::map<size_t, std::string>& FeatureMatchConfig::descriptor_paths() const
{
  return descriptor_paths_;
}
const std::string& FeatureMatchConfig::matches_path() const
{
  return matches_path_;
}
int FeatureMatchConfig::keys_limits() const
{
  return keys_limits_;
}
const std::map<size_t, FeatureMatchConfig::PosEntry>&
FeatureMatchConfig::pos_entries() const
{
  return pos_entries_;
}
int FeatureMatchConfig::number_of_threads() const
{
  return number_of_threads_;
}

}
}
}
