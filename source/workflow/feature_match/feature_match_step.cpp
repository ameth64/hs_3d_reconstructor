#include <opencv2/flann/miniflann.hpp>

#include "workflow/feature_match/feature_match_config.hpp"
#include "feature_match_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{ 

FeatureMatchStep::FeatureMatchStep()
{
  type_ = STEP_FEATURE_MATCH;
}

int FeatureMatchStep::GuideMatchesByPos(WorkflowStepConfig* config,
                                        MatchGuide& match_guide)
{
  const size_t KNN = 40;

  FeatureMatchConfig* feature_match_config =
    static_cast<FeatureMatchConfig*>(config);

  std::map<size_t, FeatureMatchConfig::PosEntry> pos_entries =
    feature_match_config->pos_entries();

  auto itr_pos = pos_entries.begin();
  auto itr_pos_end = pos_entries.end();
  cv::Mat index_data(int(pos_entries.size()), 3, CV_32FC1);
  double mean_x = 0.0;
  double mean_y = 0.0;
  double mean_z = 0.0;
  std::cout<<"Converting pos entries.\n";
  std::map<size_t, size_t> pos_index_map;
  for (int i = 0; itr_pos != itr_pos_end; ++itr_pos, i++)
  {
    mean_x += itr_pos->second.x;
    mean_y += itr_pos->second.y;
    mean_z += itr_pos->second.z;
    pos_index_map[size_t(i)] = itr_pos->first;
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

  const auto& image_paths = feature_match_config->image_paths();
  size_t number_of_images = image_paths.size();

  std::set<size_t> images_no_pos;
  std::cout<<"Finding images have no pos.\n";
  for (const auto& image_path : image_paths)
  {
    if (pos_entries.find(image_path.first) == pos_entries.end())
    {
      images_no_pos.insert(image_path.first);
    }
  }
  std::cout<<images_no_pos.size()<<" images have no pos.\n";

  std::cout<<"Adding base matches!\n";
  auto itr_image_path = image_paths.begin();
  auto itr_image_path_end = image_paths.end();
  for (; itr_image_path != itr_image_path_end; ++itr_image_path)
  {
    size_t image_id = itr_image_path->first;
    if (number_of_images < KNN + 2 ||
        pos_entries.find(image_id) == pos_entries.end())
    {
      auto itr_image_path_j = image_paths.begin();
      for (; itr_image_path_j != itr_image_path; ++itr_image_path_j)
      {
        size_t image_id_j = itr_image_path_j->first;
        match_guide[image_id].insert(image_id_j);
      }
    }
    else
    {
      //将没有pos的照片全部加入匹配
      auto itr_image_no_pos = images_no_pos.begin();
      auto itr_image_no_pos_end = images_no_pos.end();
      for (; itr_image_no_pos != itr_image_no_pos_end; ++itr_image_no_pos)
      {
        if (*itr_image_no_pos < image_id)
        {
          match_guide[image_id].insert(*itr_image_no_pos);
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

    auto itr_pos_entry = pos_entries.begin();
    for (size_t i = 0; itr_pos_entry != pos_entries.end(); ++itr_pos_entry, ++i)
    {
      size_t match = itr_pos_entry->first;
      for (size_t j = 0; j < KNN + 1; j++)
      {
        size_t neighbor = pos_index_map[indices.at<int>(int(i), int(j))];
        if (neighbor != match)
        {
          match_guide[match].insert(neighbor);
        }
      }
    }

    std::cout<<"Eliminate duplicate matches.\n";
    //剔除重复的匹配
    for (auto& matches : match_guide)
    {
      size_t image_id_i = matches.first;
      auto itr_match = matches.second.begin();
      auto itr_match_end = matches.second.end();
      for (; itr_match != itr_match_end;)
      {
        size_t image_id_j = *itr_match;
        if (image_id_j > image_id_i)
        {
          if (match_guide[image_id_j].find(image_id_i) ==
              match_guide[image_id_j].end())
          {
            match_guide[image_id_j].insert(image_id_i);
          }
          itr_match = match_guide[image_id_i].erase(itr_match);
        }
        else
        {
          itr_match++;
        }
      }
    }

  }

  return 0;
}


}
}
}
