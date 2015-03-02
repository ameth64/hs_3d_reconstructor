#include <fstream>
#include <iomanip>
#include <iostream>

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_sfm/sfm_file_io/matches_saver.hpp"
#include "hs_sfm/fundamental/linear_8_points_ransac_refiner.hpp"

#include "opencv_feature_match.hpp"

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
  const std::vector<std::string>& image_paths)
{
  image_paths_ = image_paths;
}
void FeatureMatchConfig::set_key_paths(
  const std::vector<std::string>& key_paths)
{
  key_paths_ = key_paths;
}
void FeatureMatchConfig::set_descripor_paths(
  const std::vector<std::string>& descriptor_paths)
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
  std::map<size_t, PosEntry>& pos_entries)
{
  pos_entries_ = pos_entries;
}

const std::vector<std::string>& FeatureMatchConfig::image_paths() const
{
  return image_paths_;
}
const std::vector<std::string>& FeatureMatchConfig::key_paths() const
{
  return key_paths_;
}
const std::vector<std::string>& FeatureMatchConfig::descriptor_paths() const
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
std::map<size_t, FeatureMatchConfig::PosEntry>
FeatureMatchConfig::pos_entries() const
{
  return pos_entries_;
}

OpenCVFeatureMatch::OpenCVFeatureMatch()
{
  type_ = STEP_FEATURE_MATCH;
}

int OpenCVFeatureMatch::DetectFeature(WorkflowStepConfig* config,
                                      KeysetContainer& keysets)
{
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  size_t number_of_images = feature_match_config->image_paths().size();
  if (feature_match_config->key_paths().size() != number_of_images ||
      feature_match_config->descriptor_paths().size() != number_of_images)
  {
    return -1;
  }
  cv::SIFT sift(feature_match_config->keys_limits());
  auto itr_image_path = feature_match_config->image_paths().begin();
  auto itr_image_path_end = feature_match_config->image_paths().end();
  auto itr_key_path = feature_match_config->key_paths().begin();
  auto itr_key_path_end = feature_match_config->key_paths().end();
  auto itr_descriptor_path = feature_match_config->descriptor_paths().begin();
  auto itr_descriptor_path_end =
    feature_match_config->descriptor_paths().end();
  hs::imgio::whole::ImageIO image_io;
  for (size_t i = 0; itr_image_path != itr_image_path_end;
       ++itr_image_path, ++itr_key_path, ++itr_descriptor_path, i++)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }
    //imread doesn't work!And I don't know why!
    //cv::Mat image = cv::imread(*itr_image_path, cv::IMREAD_GRAYSCALE);
    hs::imgio::whole::ImageData image_data;
    if (image_io.LoadImage(*itr_image_path, image_data) != 0) return -1;
    cv::Mat image(image_data.height(), image_data.width(), CV_8UC3,
                  image_data.GetBuffer());
    cv::Mat image_gray;
    cv::cvtColor(image, image_gray, cv::COLOR_RGB2GRAY);
    cv::Mat mask;
    std::vector<cv::KeyPoint> keys;
    cv::Mat descriptors;
    sift(image_gray, mask, keys, descriptors);

    size_t number_of_keys = keys.size();
    Keyset keyset(number_of_keys);
    std::ofstream key_file(itr_key_path->c_str(), std::ios::out);
    if (!key_file) return -1;
    key_file.setf(std::ios::fixed);
    key_file<<std::setprecision(6);
    key_file<<number_of_keys<<"\n";
    for (size_t i = 0; i < number_of_keys; i++)
    {
      key_file<<keys[i].pt.x<<" "<<keys[i].pt.y<<"\n";
      keyset[i] << keys[i].pt.x,
                   keys[i].pt.y;
    }
    keysets.push_back(keyset);
    key_file.close();

    std::ofstream descriptor_file(itr_descriptor_path->c_str(),
                                  std::ios::out | std::ios::binary);
    if (!descriptor_file) return -1;
    descriptor_file.write((const char*)(descriptors.data),
                          number_of_keys * sizeof(float) * 128);
    descriptor_file.close();

    progress_manager_.SetCurrentSubProgressCompleteRatio(
      float(i + 1) / float(number_of_images));
  }
  if (keysets.size() == number_of_images)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

int OpenCVFeatureMatch::MatchFeatures(WorkflowStepConfig* config,
                                      hs::sfm::MatchContainer& matches)
{
  matches.clear();
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  size_t number_of_images = feature_match_config->image_paths().size();
  size_t number_of_matches = 0;
  for (size_t i = 0; i < number_of_images; i++)
  {
    for (size_t j = 0; j < i; j++)
    {
      number_of_matches++;
    }
  }

  size_t number_of_matched = 0;
  for (size_t i = 0; i < number_of_images; i++)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }
    std::cout<<"Matching train image "<<i<<"\n";
    cv::Mat descriptors_index = LoadDescriptors(
      feature_match_config->key_paths()[i],
      feature_match_config->descriptor_paths()[i]);
    cv::flann::Index index(descriptors_index,
                           cv::flann::KDTreeIndexParams(4));
    for (size_t j = 0; j < i; j++)
    {
      if (!progress_manager_.CheckKeepWorking())
      {
        break;
      }
      std::cout<<"Matching query image "<<j<<"\n";
      cv::Mat descriptors_match = LoadDescriptors(
        feature_match_config->key_paths()[j],
        feature_match_config->descriptor_paths()[j]);
      cv::Mat indices(descriptors_match.rows, 2, CV_32SC1);
      cv::Mat distances(descriptors_match.rows, 2, CV_32FC1);
      index.knnSearch(descriptors_match, indices, distances,
                      2, cv::flann::SearchParams(128));

      const float match_threshold = 0.6f;

      hs::sfm::ImagePair image_pair(i, j);
      hs::sfm::KeyPairContainer key_pairs;
      for (size_t k = 0; k < descriptors_match.rows; k++)
      {
        if (distances.at<float>(k, 0) <
            distances.at<float>(k, 1) * match_threshold)
        {
          //Found Match!
          key_pairs.push_back(
            hs::sfm::KeyPair(size_t(indices.at<int>(k, 0)), k));
        }
      }
      if (key_pairs.size() > 16)
      {
        matches[image_pair] = key_pairs;
      }

      number_of_matched++;
      progress_manager_.SetCurrentSubProgressCompleteRatio(
        float(number_of_matched) / float(number_of_matches));
    }
  }

  return 0;
}

int OpenCVFeatureMatch::FilterMatches(
  WorkflowStepConfig* config,
  const KeysetContainer& keysets,
  const hs::sfm::MatchContainer& matches_initial,
  hs::sfm::MatchContainer& matches_filtered)
{
  typedef hs::sfm::fundamental::Linear8PointsRansacRefiner<double> Refiner;
  typedef typename Refiner::Key Key;
  typedef typename Refiner::KeyPair RefinerKeyPair;
  typedef typename Refiner::KeyPairContainer RefinerKeyPairContainer;
  typedef typename Refiner::IndexSet IndexSet;

  auto itr_key_pairs = matches_initial.begin();
  auto itr_key_pairs_end = matches_initial.end();
  double distance_threshold = 32.0;
  Refiner refiner;
  for (; itr_key_pairs != itr_key_pairs_end; ++itr_key_pairs)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }
    std::cout<<"Filtering image pair "<<itr_key_pairs->first.first<<" "
                                      <<itr_key_pairs->first.second<<"\n";
    RefinerKeyPairContainer refiner_key_pairs_initial;
    RefinerKeyPairContainer refiner_key_pairs_refined;
    IndexSet inlier_indices;
    auto itr_key_pair = itr_key_pairs->second.begin();
    auto itr_key_pair_end = itr_key_pairs->second.end();
    for (; itr_key_pair != itr_key_pair_end; ++itr_key_pair)
    {
      RefinerKeyPair refiner_key_pair;
      refiner_key_pair.first =
        keysets[itr_key_pairs->first.first][itr_key_pair->first];
      refiner_key_pair.second =
        keysets[itr_key_pairs->first.second][itr_key_pair->second];
      refiner_key_pairs_initial.push_back(refiner_key_pair);
    }
    refiner(refiner_key_pairs_initial, distance_threshold,
            refiner_key_pairs_refined, inlier_indices, 2048);
    std::cout<<"inlier_indices.size():"<<inlier_indices.size()<<"\n";
    if (inlier_indices.size() > 100)
    {
      hs::sfm::KeyPairContainer key_pairs_refined;
      for (size_t i = 0; i < inlier_indices.size(); i++)
      {
        key_pairs_refined.push_back(
          itr_key_pairs->second[inlier_indices[i]]);
      }
      matches_filtered[itr_key_pairs->first] = key_pairs_refined;
    }
  }

  return 0;
}

cv::Mat OpenCVFeatureMatch::LoadDescriptors(const std::string& key_path,
                                            const std::string& descriptor_path)
{
  cv::Mat descriptors;
  while (1)
  {
    std::ifstream key_file(key_path.c_str(), std::ios::in);
    if(!key_file) break;
    size_t number_of_keys;
    key_file >> number_of_keys;
    key_file.close();
    std::ifstream descriptor_file(descriptor_path,
                                  std::ios::in |std::ios::binary);
    if (!descriptor_file) break;
    descriptors.create(number_of_keys, 128, CV_32FC1);
    descriptor_file.read((char*)(descriptors.ptr()),
                         number_of_keys * 128 * sizeof(float));
    descriptor_file.close();
    break;
  }
  return descriptors;
}

int OpenCVFeatureMatch::RunImplement(WorkflowStepConfig* config)
{
  int result = 0;
  progress_manager_.AddSubProgress(0.4);
  KeysetContainer keysets;
  result = DetectFeature(config, keysets);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.4);
  hs::sfm::MatchContainer matches_initial;
  result = MatchFeatures(config, matches_initial);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.19);
  hs::sfm::MatchContainer matches_filtered;
  result = FilterMatches(config, keysets, matches_initial, matches_filtered);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.01);
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  hs::sfm::fileio::MatchesSaver saver;
  result = saver(feature_match_config->matches_path(), matches_filtered);
  progress_manager_.FinishCurrentSubProgress();

  return result;
}

}
}
}
