#include <fstream>
#include <iomanip>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_sfm/sfm_file_io/matches_saver.hpp"
#include "hs_sfm/sfm_file_io/keyset_saver.hpp"
#include "hs_sfm/fundamental/linear_8_points_ransac_refiner.hpp"

#include "opencv_feature_match.hpp"
#include "workflow/feature_match/feature_match_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

OpenCVFeatureMatch::OpenCVFeatureMatch()
  : FeatureMatchStep()
{
}

int OpenCVFeatureMatch::DetectFeature(WorkflowStepConfig* config,
                                      KeysetMap& keysets)
{
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  std::string keysets_path = feature_match_config->keysets_path();
  size_t number_of_images = feature_match_config->image_paths().size();
  if (feature_match_config->descriptor_paths().size() != number_of_images)
  {
    return -1;
  }
  cv::SIFT sift(feature_match_config->keys_limits());
  auto itr_image_path = feature_match_config->image_paths().begin();
  auto itr_image_path_end = feature_match_config->image_paths().end();
  auto itr_descriptor_path = feature_match_config->descriptor_paths().begin();
  auto itr_descriptor_path_end =
    feature_match_config->descriptor_paths().end();
  hs::imgio::whole::ImageIO image_io;
  for (size_t i = 0; itr_image_path != itr_image_path_end;
       ++itr_image_path, ++itr_descriptor_path, i++)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }
    //imread doesn't work!And I don't know why!
    //cv::Mat image = cv::imread(*itr_image_path, cv::IMREAD_GRAYSCALE);
    hs::imgio::whole::ImageData image_data;
    if (image_io.LoadImage(itr_image_path->second, image_data) != 0) return -1;
    cv::Mat image(image_data.height(), image_data.width(), CV_8UC3,
                  image_data.GetBuffer());
    cv::Mat image_gray;
    cv::cvtColor(image, image_gray, cv::COLOR_RGB2GRAY);
    //TODO:Use pyramid now.
    cv::Mat image_pyramid;
    cv::resize(image_gray, image_pyramid,
               cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);

    cv::Mat mask;
    std::vector<cv::KeyPoint> keys;
    cv::Mat descriptors;
    sift(image_pyramid, mask, keys, descriptors);

    size_t number_of_keys = keys.size();
    Keyset keyset(number_of_keys);
    for (size_t j = 0; j < number_of_keys; j++)
    {
      keyset[j] << double(keys[j].pt.x) * 2.0,
                   double(keys[j].pt.y) * 2.0;
    }
    keysets.insert(std::make_pair(itr_image_path->first, keyset));

    std::ofstream descriptor_file(itr_descriptor_path->second.c_str(),
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
    std::cout<<"Detect features success.\n";
    std::ofstream keysets_file(keysets_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(keysets_file);
    archive(keysets);
    return 0;
  }
  else
  {
    std::cout<<"Detect features failed.\n";
    return -1;
  }
}

int OpenCVFeatureMatch::MatchFeatures(WorkflowStepConfig* config,
                                      const KeysetMap& keysets,
                                      const MatchGuide& match_guide,
                                      hs::sfm::MatchContainer& matches)
{
  matches.clear();
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  size_t number_of_matches = 0;
  for (const auto& guide : match_guide)
  {
    for (const auto& match : guide.second)
    {
      number_of_matches++;
    }
  }

  size_t number_of_matched = 0;
  for (const auto& match_guide_i : match_guide)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }
    size_t image_id_i = match_guide_i.first;
    std::cout<<"Matching train image "<<image_id_i<<"\n";
    auto itr_descriptor_path_i =
      feature_match_config->descriptor_paths().find(image_id_i);
    auto itr_keyset_i = keysets.find(image_id_i);
    cv::Mat descriptors_index = LoadDescriptors(
      itr_keyset_i->second.size(),
      itr_descriptor_path_i->second);
    cv::flann::Index index(descriptors_index,
                           cv::flann::KDTreeIndexParams(4));
    std::cout<<"number_of_threads:"
             <<feature_match_config->number_of_threads()<<"\n";
    std::vector<size_t> guided_matches(match_guide_i.second.begin(),
                                       match_guide_i.second.end());
#ifdef _OPENMP
    omp_set_num_threads(feature_match_config->number_of_threads());
    omp_lock_t lock;
    omp_init_lock(&lock);
#pragma omp parallel for
#endif

    for (int i = 0; i < int(guided_matches.size()); i++)
    {
      if (!progress_manager_.CheckKeepWorking())
      {
        continue;
      }
      size_t image_id_j = guided_matches[i];
      auto itr_descriptor_path_j =
        feature_match_config->descriptor_paths().find(image_id_j);
      auto itr_keyset_j = keysets.find(image_id_j);
      cv::Mat descriptors_match = LoadDescriptors(
        itr_keyset_j->second.size(),
        itr_descriptor_path_j->second);
      cv::Mat indices(descriptors_match.rows, 2, CV_32SC1);
      cv::Mat distances(descriptors_match.rows, 2, CV_32FC1);
      index.knnSearch(descriptors_match, indices, distances,
                      2, cv::flann::SearchParams(128));

      const float match_threshold = 0.6f;

      hs::sfm::ImagePair image_pair(itr_descriptor_path_i->first,
                                    itr_descriptor_path_j->first);
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
      ++itr_descriptor_path_j;
#ifdef _OPENMP
        omp_set_lock(&lock);
#endif
      if (key_pairs.size() > 16)
      {
        matches[image_pair] = key_pairs;
      }

      number_of_matched++;
      progress_manager_.SetCurrentSubProgressCompleteRatio(
        float(number_of_matched) / float(number_of_matches));

#ifdef _OPENMP
        omp_unset_lock(&lock);
#endif

    }

#ifdef _OPENMP
    omp_destroy_lock(&lock);
#endif
  }

  return 0;
}

int OpenCVFeatureMatch::FilterMatches(
  WorkflowStepConfig* config,
  const KeysetMap& keysets,
  const hs::sfm::MatchContainer& matches_initial,
  hs::sfm::MatchContainer& matches_filtered)
{
  typedef hs::sfm::fundamental::Linear8PointsRansacRefiner<double> Refiner;
  typedef Refiner::Key Key;
  typedef Refiner::KeyPair RefinerKeyPair;
  typedef Refiner::KeyPairContainer RefinerKeyPairContainer;
  typedef Refiner::IndexSet IndexSet;

  size_t number_of_image_pairs = 0;
  for (const auto& image_pair : matches_initial)
  {
    number_of_image_pairs++;
  }

  auto itr_key_pairs = matches_initial.begin();
  auto itr_key_pairs_end = matches_initial.end();
  double distance_threshold = 32.0;
  Refiner refiner;
  size_t number_of_finished_image_pairs = 0;
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
    auto itr_keyset_first = keysets.find(itr_key_pairs->first.first);
    auto itr_keyset_second = keysets.find(itr_key_pairs->first.second);
    for (; itr_key_pair != itr_key_pair_end; ++itr_key_pair)
    {
      RefinerKeyPair refiner_key_pair;
      refiner_key_pair.first =
        itr_keyset_first->second[itr_key_pair->first];
      refiner_key_pair.second =
        itr_keyset_second->second[itr_key_pair->second];
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

    number_of_finished_image_pairs++;
    progress_manager_.SetCurrentSubProgressCompleteRatio(
      float(number_of_finished_image_pairs) /
      float(number_of_image_pairs));
  }

  return 0;
}

#if 0
int OpenCVFeatureMatch::FilterMatchesOpenMVG(
  WorkflowStepConfig* config,
  const KeysetMap& keysets,
  const hs::sfm::MatchContainer& matches_initial,
  hs::sfm::MatchContainer& matches_filtered)
{
  size_t number_of_image_pairs = 0;
  for (const auto& image_pair : matches_initial)
  {
    number_of_image_pairs++;
  }

  auto itr_key_pairs = matches_initial.begin();
  auto itr_key_pairs_end = matches_initial.end();
  double distance_threshold = 32.0;
  size_t number_of_finished_image_pairs = 0;
  for (; itr_key_pairs != itr_key_pairs_end; ++itr_key_pairs)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }
    std::cout<<"Filtering image pair "<<itr_key_pairs->first.first<<" "
                                      <<itr_key_pairs->first.second<<"\n";
    auto itr_key_pair = itr_key_pairs->second.begin();
    auto itr_key_pair_end = itr_key_pairs->second.end();
    auto itr_keyset_first = keysets.find(itr_key_pairs->first.first);
    auto itr_keyset_second = keysets.find(itr_key_pairs->first.second);
    for (; itr_key_pair != itr_key_pair_end; ++itr_key_pair)
    {
    }
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

    number_of_finished_image_pairs++;
    progress_manager_.SetCurrentSubProgressCompleteRatio(
      float(number_of_finished_image_pairs) /
      float(number_of_image_pairs));
  }

  return 0;
}
#endif

cv::Mat OpenCVFeatureMatch::LoadDescriptors(size_t number_of_keys,
                                            const std::string& descriptor_path)
{
  cv::Mat descriptors;
  while (1)
  {
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
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  int result = 0;
  progress_manager_.AddSubProgress(0.4f);
  KeysetMap keysets;
  result = DetectFeature(config, keysets);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.4f);
  MatchGuide match_guide;
  result = GuideMatchesByPos(config, match_guide);
  if (result != 0) return result;
  hs::sfm::MatchContainer matches_initial;
  result = MatchFeatures(config, keysets, match_guide, matches_initial);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.19f);
  hs::sfm::MatchContainer matches_filtered;
  result = FilterMatches(config, keysets, matches_initial, matches_filtered);
  progress_manager_.FinishCurrentSubProgress();
  if (result != 0) return result;

  progress_manager_.AddSubProgress(0.01f);
  {
    std::ofstream matches_file(feature_match_config->matches_path(),
                               std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(matches_file);
    archive(matches_filtered);
  }

  progress_manager_.FinishCurrentSubProgress();

  return result;
}

}
}
}
