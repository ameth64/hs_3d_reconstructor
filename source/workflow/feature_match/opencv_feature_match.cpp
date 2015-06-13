#include <fstream>
#include <iomanip>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_sfm/sfm_file_io/matches_saver.hpp"
#include "hs_sfm/sfm_file_io/keyset_saver.hpp"
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
  const std::map<size_t, PosEntry>& pos_entries)
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
const std::map<size_t, FeatureMatchConfig::PosEntry>&
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
    hs::sfm::fileio::KeysetSaver<double> keyset_saver;
    keyset_saver(*itr_key_path, keyset);
    keysets.push_back(keyset);

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
    std::cout<<"Detect features success.\n";
    return 0;
  }
  else
  {
    std::cout<<"Detect features failed.\n";
    return -1;
  }
}

int OpenCVFeatureMatch::GuideMatchesByPos(WorkflowStepConfig* config,
                                          MatchGuide& match_guide)
{
  const size_t KNN = 40;

  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);

  std::map<size_t, FeatureMatchConfig::PosEntry> pos_entries =
    feature_match_config->pos_entries();

  size_t number_of_images = feature_match_config->image_paths().size();
  match_guide.resize(number_of_images);

  auto itr_pos = pos_entries.begin();
  auto itr_pos_end = pos_entries.end();
  std::vector<size_t> index_map(number_of_images);
  cv::Mat index_data(int(pos_entries.size()), 3, CV_32FC1);
  double mean_x = 0.0;
  double mean_y = 0.0;
  double mean_z = 0.0;
  std::cout<<"Converting pos entries.\n";
  for (int i = 0; itr_pos != itr_pos_end; ++itr_pos, i++)
  {
    mean_x += itr_pos->second.x;
    mean_y += itr_pos->second.y;
    mean_z += itr_pos->second.z;
    index_map[size_t(i)] = itr_pos->first;
  }
  mean_x /= double(pos_entries.size());
  mean_y /= double(pos_entries.size());
  mean_z /= double(pos_entries.size());
  itr_pos = pos_entries.begin();
  itr_pos_end = pos_entries.end();
  std::cout<<"Translate pos entries.\n";
  for (int i = 0; itr_pos != itr_pos_end; ++itr_pos, ++i)
  {
    itr_pos->second.x -= mean_x;
    itr_pos->second.y -= mean_y;
    itr_pos->second.z -= mean_z;
    index_data.at<float>(i, 0) = float(itr_pos->second.x);
    index_data.at<float>(i, 1) = float(itr_pos->second.y);
    index_data.at<float>(i, 2) = float(itr_pos->second.z);
  }

  cv::flann::Index index;
  if (pos_entries.size() > 2)
  {
    index.build(index_data, cv::flann::KDTreeIndexParams(4));
  }

  std::vector<size_t> images_no_pos;
  std::cout<<"Finding images have no pos.\n";
  for (size_t i = 0; i < number_of_images; i++)
  {
    if (pos_entries.find(i) == pos_entries.end())
    {
      images_no_pos.push_back(i);
    }
  }
  std::cout<<images_no_pos.size()<<" images have no pos.\n";

  std::cout<<"Adding base matches!\n";
  for (size_t i = 0; i < number_of_images; i++)
  {
    if (number_of_images < KNN + 2 || pos_entries.find(i) == pos_entries.end())
    {
      for (size_t j = 0; j < i; j++)
      {
        match_guide[i].insert(j);
      }
    }
    else
    {
      //将没有pos的照片全部加入匹配
      auto itr_image_no_pos = images_no_pos.begin();
      auto itr_image_no_pos_end = images_no_pos.end();
      for (; itr_image_no_pos != itr_image_no_pos_end; ++itr_image_no_pos)
      {
        if (*itr_image_no_pos < i)
        {
          match_guide[i].insert(*itr_image_no_pos);
        }
      }
    }
  }

  std::cout<<"Add pos knn matches.\n";
  if (pos_entries.size() > KNN + 1)
  {
    cv::Mat pos_match = index_data.clone();
    cv::Mat indices(int(pos_entries.size()), int(KNN + 1), CV_32SC1);
    cv::Mat distances(int(pos_entries.size()), int(KNN + 1), CV_32FC1);
    std::cout<<"KNN search neighbors.\n";
    index.knnSearch(pos_match, indices, distances, int(KNN + 1),
                    cv::flann::SearchParams(128));
    std::cout<<"Add knn neighbors.\n";

    for (size_t i = 0; i < pos_entries.size(); i++)
    {
      size_t match = index_map[i];
      for (size_t j = 0; j < KNN + 1; j++)
      {
        size_t neighbor = index_map[indices.at<int>(int(i), int(j))];
        if (neighbor != match)
        {
          match_guide[match].insert(neighbor);
        }
      }
    }

    std::cout<<"Eliminate duplicate matches.\n";
    //剔除重复的匹配
    for (size_t i = 0; i < number_of_images; i++)
    {
      auto itr_match = match_guide[i].begin();
      auto itr_match_end = match_guide[i].end();
      for (; itr_match != itr_match_end;)
      {
        if (*itr_match > i)
        {
          if (match_guide[*itr_match].find(i) == match_guide[*itr_match].end())
          {
            match_guide[*itr_match].insert(i);
          }
          itr_match = match_guide[i].erase(itr_match);
        }
        else
        {
          itr_match++;
        }
      }
    }

#if 1
    std::cout<<"Dumping match guide.\n";
    MatchGuide match_guide_out = match_guide;
    for (size_t i = 0; i < number_of_images; i++)
    {
      for (size_t j = i + 1; j < number_of_images; j++)
      {
        if (match_guide_out[j].find(i) != match_guide_out[j].end())
        {
          match_guide_out[i].insert(j);
        }
      }
      std::cout<<"match_guide_out["<<i<<"]:\n";
      auto itr_guide = match_guide_out[i].begin();
      auto itr_guide_end = match_guide_out[i].end();
      for (; itr_guide != itr_guide_end; ++itr_guide)
      {
        std::cout<<*itr_guide<<" ";
      }
      std::cout<<"\n";
    }
#endif
  }

  return 0;
}

int OpenCVFeatureMatch::MatchFeatures(WorkflowStepConfig* config,
                                      const MatchGuide& match_guide,
                                      hs::sfm::MatchContainer& matches)
{
  matches.clear();
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  size_t number_of_images = feature_match_config->image_paths().size();
  size_t number_of_matches = 0;
  RandomAccessMatchGuide random_access_match_guide(number_of_images);
  for (size_t i = 0; i < number_of_images; i++)
  {
    auto itr_guide = match_guide[i].begin();
    auto itr_guide_end = match_guide[i].end();
    for (; itr_guide != itr_guide_end; ++itr_guide)
    {
      random_access_match_guide[i].push_back(*itr_guide);
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

    for (int k = 0; k < int(random_access_match_guide[i].size()); k++)
    {
      if (!progress_manager_.CheckKeepWorking())
      {
        continue;
      }
      size_t j = random_access_match_guide[i][k];
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
        if (distances.at<float>(int(k), 0) <
            distances.at<float>(int(k), 1) * match_threshold)
        {
          //Found Match!
          key_pairs.push_back(
            hs::sfm::KeyPair(size_t(indices.at<int>(int(k), 0)), k));
        }
      }
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
  const KeysetContainer& keysets,
  const hs::sfm::MatchContainer& matches_initial,
  hs::sfm::MatchContainer& matches_filtered)
{
  typedef hs::sfm::fundamental::Linear8PointsRansacRefiner<double> Refiner;
  typedef Refiner::Key Key;
  typedef Refiner::KeyPair RefinerKeyPair;
  typedef Refiner::KeyPairContainer RefinerKeyPairContainer;
  typedef Refiner::IndexSet IndexSet;

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
  MatchGuide match_guide;
  result = GuideMatchesByPos(config, match_guide);
  if (result != 0) return result;
  hs::sfm::MatchContainer matches_initial;
  result = MatchFeatures(config, match_guide, matches_initial);
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
