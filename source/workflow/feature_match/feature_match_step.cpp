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


}
}
}
