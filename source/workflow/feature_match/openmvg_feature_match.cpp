#include "openmvg_feature_match.hpp"

#include "hs_sfm/fundamental/linear_8_points_ransac_refiner.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <openMVG/image/image.hpp>
#include <openMVG/features/features.hpp>
#include <openMVG/matching/matcher_kdtree_flann.hpp>
#include <openMVG/matching/indMatchDecoratorXY.hpp>
#include <openMVG/matching/matching_filters.hpp>
#include <nonFree/sift/SIFT_describer.hpp>

#include "workflow/feature_match/feature_match_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

OpenMVGFeatureMatch::OpenMVGFeatureMatch()
  : FeatureMatchStep()
{
}

int OpenMVGFeatureMatch::DetectFeature(WorkflowStepConfig* config,
                                       KeysetContainer& keysets)
{
  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);
  size_t number_of_images = feature_match_config->image_paths().size();
  if (feature_match_config->descriptor_paths().size() != number_of_images)
  {
    return -1;
  }
  auto itr_image_path = feature_match_config->image_paths().begin();
  auto itr_image_path_end = feature_match_config->image_paths().end();
  auto itr_descriptor_path = feature_match_config->descriptor_paths().begin();
  auto itr_descriptor_path_end =
    feature_match_config->descriptor_paths().end();
  for (size_t i = 0; itr_image_path != itr_image_path_end;
       ++itr_image_path, ++itr_descriptor_path, i++)
  {
    if (!progress_manager_.CheckKeepWorking())
    {
      break;
    }

    std::string feature_path = *itr_descriptor_path;
    size_t pos = feature_path.rfind('.');
    if (pos != std::string::npos)
    {
      feature_path.erase(feature_path.begin() + pos, feature_path.end());
    }
    feature_path += ".feat";

    openMVG::Image<unsigned char> image_gray;
    if (!ReadImage(itr_image_path->c_str(), &image_gray)) return -1;
    openMVG::features::SIFT_Image_describer sift_describer;
    std::unique_ptr<openMVG::features::Regions> regions;
    sift_describer.Describe(image_gray, regions);
    sift_describer.Save(regions.get(), feature_path, *itr_descriptor_path);

    size_t number_of_keys = regions->RegionCount();
    Keyset keyset(number_of_keys);
    for (size_t j = 0; j < number_of_keys; j++)
    {
      openMVG::Vec2 key = regions->GetRegionPosition(j);
      keyset[j] << double(key[0]),
                   double(key[1]);
    }

    keysets.push_back(keyset);

    progress_manager_.SetCurrentSubProgressCompleteRatio(
      float(i + 1) / float(number_of_images));
  }

  {
    std::ofstream keysets_file(feature_match_config->keysets_path(),
                               std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(keysets_file);
    archive(keysets);
  }

  return 0;
}

int OpenMVGFeatureMatch::MatchFeatures(WorkflowStepConfig* config,
                                       const MatchGuide& match_guide,
                                       hs::sfm::MatchContainer& matches)
{
  typedef flann::L2<unsigned char> MetricT;
  typedef openMVG::matching::ArrayMatcher_Kdtree_Flann<unsigned char, MetricT>
          MatcherT;

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
    //cv::Mat descriptors_index = LoadDescriptors(
    //  feature_match_config->key_paths()[i],
    //  feature_match_config->descriptor_paths()[i]);
    std::string descriptor_path_i =
      feature_match_config->descriptor_paths()[i];
    std::string feature_path_i = descriptor_path_i;
    size_t pos = feature_path_i.rfind('.');
    if (pos != std::string::npos)
    {
      feature_path_i.erase(feature_path_i.begin() + pos,
                           feature_path_i.end());
    }
    feature_path_i += ".feat";
    openMVG::features::SIFT_Regions regions_i;
    regions_i.Load(feature_path_i, descriptor_path_i);
    
    //cv::flann::Index index(descriptors_index,
    //                       cv::flann::KDTreeIndexParams(4));
    const size_t regions_count_i = regions_i.RegionCount();
    const std::vector<openMVG::PointFeature> point_features_i =
      regions_i.GetRegionsPositions();
    const MatcherT::ScalarT* tab_i =
      reinterpret_cast<const MatcherT::ScalarT*>(
        regions_i.DescriptorRawData());
    MatcherT matcher10;
    matcher10.Build(tab_i, regions_count_i, regions_i.DescriptorLength());
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
      //cv::Mat descriptors_match = LoadDescriptors(
      //  feature_match_config->key_paths()[j],
      //  feature_match_config->descriptor_paths()[j]);
      std::string descriptor_path_j =
        feature_match_config->descriptor_paths()[j];
      std::string feature_path_j = descriptor_path_j;
      size_t pos = feature_path_j.rfind('.');
      if (pos != std::string::npos)
      {
        feature_path_j.erase(feature_path_j.begin() + pos,
                             feature_path_j.end());
      }
      feature_path_j += ".feat";
      openMVG::features::SIFT_Regions regions_j;
      regions_j.Load(feature_path_j, descriptor_path_j);

      //cv::Mat indices(descriptors_match.rows, 2, CV_32SC1);
      //cv::Mat distances(descriptors_match.rows, 2, CV_32FC1);
      //index.knnSearch(descriptors_match, indices, distances,
      //                2, cv::flann::SearchParams(128));
      const size_t regions_count_j = regions_j.RegionCount();
      const MatcherT::ScalarT* tab_j =
        reinterpret_cast<const MatcherT::ScalarT*>(
          regions_j.DescriptorRawData());

      const size_t nn = 2;
      std::vector<int> indices;
      std::vector<MatcherT::DistanceType> distances;
      matcher10.SearchNeighbours(tab_j, regions_count_j,
                                 &indices, &distances, nn);

      const float distance_ratio = 0.6f;
      std::vector<openMVG::matching::IndMatch> filtered_matches;
      std::vector<int> nn_ratio_indices;
      openMVG::matching::NNdistanceRatio(
        distances.begin(), distances.end(), nn, nn_ratio_indices,
        distance_ratio);

      for (size_t k = 0; k < nn_ratio_indices.size(); k++)
      {
        const size_t index = nn_ratio_indices[k];
        filtered_matches.push_back(
          openMVG::matching::IndMatch(indices[index * nn], index));
      }

      openMVG::matching::IndMatch::getDeduplicated(filtered_matches);
      const std::vector<openMVG::PointFeature>& point_features_j =
        regions_j.GetRegionsPositions();
      openMVG::matching::IndMatchDecorator<float> match_deduplicator(
        filtered_matches, point_features_i, point_features_j);
      match_deduplicator.getDeduplicated(filtered_matches);

      //const float match_threshold = 0.6f;

      //hs::sfm::ImagePair image_pair(i, j);
      //hs::sfm::KeyPairContainer key_pairs;
      //for (size_t k = 0; k < descriptors_match.rows; k++)
      //{
      //  if (distances.at<float>(int(k), 0) <
      //      distances.at<float>(int(k), 1) * match_threshold)
      //  {
      //    //Found Match!
      //    key_pairs.push_back(
      //      hs::sfm::KeyPair(size_t(indices.at<int>(int(k), 0)), k));
      //  }
      //}
#ifdef _OPENMP
        omp_set_lock(&lock);
#endif
      if (filtered_matches.size() > 16)
      {
        hs::sfm::ImagePair image_pair(i, j);
        hs::sfm::KeyPairContainer key_pairs;
        for (size_t k = 0; k < filtered_matches.size(); k++)
        {
          key_pairs.push_back(std::make_pair(size_t(filtered_matches[k]._i),
                                             size_t(filtered_matches[k]._j)));
        }
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

int OpenMVGFeatureMatch::FilterMatches(
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

int OpenMVGFeatureMatch::RunImplement(WorkflowStepConfig* config)
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