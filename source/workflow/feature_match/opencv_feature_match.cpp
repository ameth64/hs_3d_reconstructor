#include <fstream>
#include <iomanip>
#include <iostream>

#define DEBUG_5POINT 1

#ifdef _OPENMP
#include <omp.h>
#endif

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_sfm/sfm_file_io/matches_saver.hpp"
#include "hs_sfm/fundamental/linear_8_points_ransac_refiner.hpp"
#if DEBUG_5POINT
#include "hs_sfm/sfm_utility/matches_tracks_convertor.hpp"
#endif

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
void FeatureMatchConfig::set_number_of_threads(int number_of_threads)
{
  number_of_threads_ = number_of_threads;
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
int FeatureMatchConfig::number_of_threads() const
{
  return number_of_threads_;
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
    for (size_t j = 0; j < number_of_keys; j++)
    {
      key_file<<keys[j].pt.x<<" "<<keys[j].pt.y<<"\n";
      keyset[j] << keys[j].pt.x,
                   keys[j].pt.y;
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
    std::cout<<"number_of_threads:"
             <<feature_match_config->number_of_threads()<<"\n";
#ifdef _OPENMP
    omp_set_num_threads(feature_match_config->number_of_threads());
    omp_lock_t lock;
    omp_init_lock(&lock);
#pragma omp parallel for
#endif

    for (int j = 0; j < i; j++)
    {
      if (!progress_manager_.CheckKeepWorking())
      {
        continue;
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

      hs::sfm::ImagePair image_pair(i, size_t(j));
      hs::sfm::KeyPairContainer key_pairs;
      for (size_t k = 0; k < descriptors_match.rows; k++)
      {
        if (distances.at<float>(int(k), 0) <
            distances.at<float>(int(k), 1) * match_threshold)
        {
          //Found Match!
          key_pairs.push_back(
            hs::sfm::KeyPair(size_t(indices.at<int>(int(k), 0)), k));
        }
      }
      if (key_pairs.size() > 16)
      {
#ifdef _OPENMP
        omp_set_lock(&lock);
#endif
        matches[image_pair] = key_pairs;
#ifdef _OPENMP
        omp_unset_lock(&lock);
#endif

      }

      number_of_matched++;
      progress_manager_.SetCurrentSubProgressCompleteRatio(
        float(number_of_matched) / float(number_of_matches));
    }

#ifdef _OPENMP
    omp_destroy_lock(&lock);
#endif
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
  typedef Refiner::Key Key;
  typedef Refiner::KeyPair RefinerKeyPair;
  typedef Refiner::KeyPairContainer RefinerKeyPairContainer;
  typedef Refiner::IndexSet IndexSet;

#if DEBUG_5POINT
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  std::string constraints_path = feature_match_config->matches_path();
  size_t pos = constraints_path.rfind('/');
  constraints_path.erase(constraints_path.begin() + pos + 1,
                         constraints_path.end());
  constraints_path += "constraints.txt";
  std::ofstream constraints_file(constraints_path.c_str());

  std::map<hs::sfm::ImagePair, float> inlier_ratios;
  std::map<hs::sfm::ImagePair, size_t> inlier_nums;
#endif

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
#if DEBUG_5POINT
      inlier_ratios[itr_key_pairs->first] =
        float(refiner_key_pairs_initial.size()) /
        float(refiner_key_pairs_refined.size());
      inlier_nums[itr_key_pairs->first] = inlier_indices.size();
#endif
    }
  }

#if DEBUG_5POINT
  size_t number_of_images = feature_match_config->image_paths().size();
  size_t number_of_image_pairs = matches_filtered.size();
  constraints_file<<number_of_images<<"\n";
  constraints_file<<number_of_image_pairs<<"\n";
  itr_key_pairs = matches_filtered.begin();
  itr_key_pairs_end = matches_filtered.end();
  for (; itr_key_pairs != itr_key_pairs_end; ++itr_key_pairs)
  {
    constraints_file<<itr_key_pairs->first.first<<" "
                    <<itr_key_pairs->first.second<<"\n";
    constraints_file<<"0 0 0 0 0 0 0 0 0\n0 0 0 0 0 0 0 0 0\n";
    constraints_file<<inlier_ratios[itr_key_pairs->first]<<"\n";
    constraints_file<<inlier_nums[itr_key_pairs->first]<<"\n";
    constraints_file<<"0\n";
  }

  hs::sfm::TrackContainer tracks;
  hs::sfm::MatchesTracksConvertor convertor;
  convertor(matches_filtered, tracks);
  constraints_file<<tracks.size()<<"\n";
  for (size_t i = 0; i < tracks.size(); i++)
  {
    constraints_file<<tracks[i].size();
    for (size_t j = 0; j < tracks[i].size(); j++)
    {
      constraints_file<<" "<<tracks[i][j].first<<" "<<tracks[i][j].second;
    }
    constraints_file<<"\n";
  }
#endif

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
    descriptors.create(int(number_of_keys), 128, CV_32FC1);
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
  progress_manager_.AddSubProgress(0.4f);
  KeysetContainer keysets;
  result = DetectFeature(config, keysets);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.4f);
  hs::sfm::MatchContainer matches_initial;
  result = MatchFeatures(config, matches_initial);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.19f);
  hs::sfm::MatchContainer matches_filtered;
  result = FilterMatches(config, keysets, matches_initial, matches_filtered);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.01f);
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
