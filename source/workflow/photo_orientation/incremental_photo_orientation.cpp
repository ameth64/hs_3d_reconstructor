#include <fstream>
#include <iomanip>
#include <iostream>
#include <array>

#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp> 

#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "hs_sfm/sfm_file_io/keyset_loader.hpp"
#include "hs_sfm/sfm_file_io/matches_loader.hpp"
#include "hs_sfm/sfm_utility/similar_transform_estimator.hpp"
#include "hs_sfm/sfm_pipeline/incremental_sfm.hpp"
#include "hs_sfm/sfm_pipeline/reprojective_error_calculator.hpp"
#include "hs_sfm/sfm_pipeline/point_cloud_norm_calculator.hpp"
#include "hs_image_io/whole_io/image_data.hpp"
#include "hs_image_io/whole_io/image_io.hpp"
#include "hs_graphics/graphics_utility/pointcloud_data.hpp"

#include "workflow/photo_orientation/incremental_photo_orientation.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

PhotoOrientationConfig::PhotoOrientationConfig()
{
  type_ = STEP_PHOTO_ORIENTATION;
}

void PhotoOrientationConfig::set_image_intrinsic_map(
  const hs::sfm::ObjectIndexMap& image_intrinsic_map)
{
  image_intrinsic_map_ = image_intrinsic_map;
}
void PhotoOrientationConfig::set_matches_path(
  const std::string& matches_path)
{
  matches_path_ = matches_path;
}
void PhotoOrientationConfig::set_image_paths(
  const std::vector<std::string>& image_paths)
{
  image_paths_ = image_paths;
}
void PhotoOrientationConfig::set_keysets_path(
  const std::string& keysets_path)
{
  keysets_path_ = keysets_path;
}
void PhotoOrientationConfig::set_image_ids(
  const std::vector<int>& image_ids)
{
  image_ids_ = image_ids;
}
void PhotoOrientationConfig::set_intrinsic_params_set(
  const IntrinsicParamsContainer& intrinsic_params_set)
{
  intrinsic_params_set_ = intrinsic_params_set;
}
void PhotoOrientationConfig::set_intrinsic_ids(
  const std::vector<int>& intrinsic_ids)
{
  intrinsic_ids_ = intrinsic_ids;
}
void PhotoOrientationConfig::set_intrinsic_path(
  const std::string& intrinsic_path)
{
  intrinsic_path_ = intrinsic_path;
}
void PhotoOrientationConfig::set_extrinsic_path(
  const std::string& extrinsic_path)
{
  extrinsic_path_ = extrinsic_path;
}
void PhotoOrientationConfig::set_point_cloud_path(
  const std::string& point_cloud_path)
{
  point_cloud_path_ = point_cloud_path;
}
void PhotoOrientationConfig::set_tracks_path(
  const std::string& tracks_path)
{
  tracks_path_ = tracks_path;
}
void PhotoOrientationConfig::set_similar_transform_path(
  const std::string& similar_transform_path)
{
  similar_transform_path_ = similar_transform_path;
}
void PhotoOrientationConfig::set_workspace_path(
  const std::string& workspace_path)
{
  workspace_path_ = workspace_path;
}
void PhotoOrientationConfig::set_number_of_threads(int number_of_threads)
{
  number_of_threads_ = number_of_threads;
}

void PhotoOrientationConfig::set_pos_entries(
  const PosEntryContainer& pos_entries)
{
  pos_entries_ = pos_entries;
}

const hs::sfm::ObjectIndexMap&
PhotoOrientationConfig::image_intrinsic_map() const
{
  return image_intrinsic_map_;
}
const std::string& PhotoOrientationConfig::matches_path() const
{
  return matches_path_;
}
const std::vector<std::string>& PhotoOrientationConfig::image_paths() const
{
  return image_paths_;
}
const std::string& PhotoOrientationConfig::keysets_path() const
{
  return keysets_path_;
}
const std::vector<int>& PhotoOrientationConfig::image_ids() const
{
  return image_ids_;
}
const PhotoOrientationConfig::IntrinsicParamsContainer&
PhotoOrientationConfig::intrinsic_params_set() const
{
  return intrinsic_params_set_;
}
const std::vector<int>& PhotoOrientationConfig::intrinsic_ids() const
{
  return intrinsic_ids_;
}
const std::string& PhotoOrientationConfig::intrinsic_path() const
{
  return intrinsic_path_;
}
const std::string& PhotoOrientationConfig::extrinsic_path() const
{
  return extrinsic_path_;
}
const std::string& PhotoOrientationConfig::point_cloud_path() const
{
  return point_cloud_path_;
}
const std::string& PhotoOrientationConfig::tracks_path() const
{
  return tracks_path_;
}
const std::string& PhotoOrientationConfig::similar_transform_path() const
{
  return similar_transform_path_;
}
const std::string& PhotoOrientationConfig::workspace_path() const
{
  return workspace_path_;
}
int PhotoOrientationConfig::number_of_threads() const
{
  return number_of_threads_;
}
const PhotoOrientationConfig::PosEntryContainer&
PhotoOrientationConfig::pos_entries() const
{
  return pos_entries_;
}

IncrementalPhotoOrientation::IncrementalPhotoOrientation()
{
  type_ = STEP_PHOTO_ORIENTATION;
}

int IncrementalPhotoOrientation::LoadKeysets(
  WorkflowStepConfig* config, KeysetContainer& keysets)
{
  typedef EIGEN_STD_MAP(size_t, Keyset) KeysetMap;
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::string& keysets_path = photo_orientation_config->keysets_path();

  keysets.clear();
  KeysetMap keyset_map;
  {
    std::ifstream keysets_file(keysets_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(keysets_file);
    archive(keyset_map);
  }

  for (const auto& keyset : keyset_map)
  {
    keysets.push_back(keyset.second);
  }

  return 0;
}

int IncrementalPhotoOrientation::LoadMatches(
  WorkflowStepConfig* config, hs::sfm::MatchContainer& matches)
{
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::string& matches_path =
    photo_orientation_config->matches_path();
  const auto& image_ids = photo_orientation_config->image_ids();
  std::map<size_t, size_t> image_id_map;
  for (size_t i = 0; i < image_ids.size(); i++)
  {
    image_id_map[size_t(image_ids[i])] = i;
  }

  matches.clear();
  hs::sfm::MatchContainer matches_feature_match;
  {
    std::ifstream matches_file(matches_path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(matches_file);
    archive(matches_feature_match);
  }

  for (const auto& image_pair : matches_feature_match)
  {
    size_t image_id_first = image_id_map[image_pair.first.first];
    size_t image_id_second = image_id_map[image_pair.first.second];
    matches[std::make_pair(image_id_first, image_id_second)] =
      image_pair.second;
  }

  return 0;

}

int IncrementalPhotoOrientation::RunSFM(
  WorkflowStepConfig* config,
  const KeysetContainer& keysets,
  const hs::sfm::MatchContainer& matches,
  IntrinsicParamsContainer& intrinsic_params_set,
  ExtrinsicParamsContainer& extrinsic_params_set,
  hs::sfm::ObjectIndexMap& image_extrinsic_map,
  PointContainer& points,
  hs::sfm::TrackContainer& tracks,
  hs::sfm::ObjectIndexMap& track_point_map,
  hs::sfm::ViewInfoIndexer& view_info_indexer)
{
  typedef hs::sfm::pipeline::IncrementalSFM<Scalar> SFM;
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const hs::sfm::ObjectIndexMap& image_intrinsic_map =
    photo_orientation_config->image_intrinsic_map();
  intrinsic_params_set =
    photo_orientation_config->intrinsic_params_set();

  SFM sfm(100, 8, 2, size_t(photo_orientation_config->number_of_threads()));
  int result =  sfm(image_intrinsic_map,
                    matches,
                    keysets,
                    intrinsic_params_set,
                    extrinsic_params_set,
                    image_extrinsic_map,
                    points,
                    tracks,
                    track_point_map,
                    view_info_indexer,
                    &progress_manager_);

  return result;
}

int IncrementalPhotoOrientation::SimilarTransformByPosEntries(
  WorkflowStepConfig* config,
  const ExtrinsicParamsContainer& extrinsic_params_set,
  const hs::sfm::ObjectIndexMap& image_extrinsic_map)
{
  typedef PhotoOrientationConfig::PosEntry PosEntry;
  typedef PhotoOrientationConfig::PosEntryContainer PosEntryContainer;
  typedef hs::sfm::SimilarTransformEstimator<Scalar> Estimator;
  typedef Estimator::Point Point;
  typedef Estimator::PointContainer PointContainer;
  typedef Estimator::Rotation Rotation;
  typedef Estimator::Translate Translate;

  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);
    const PosEntryContainer& pos_entries =
      photo_orientation_config->pos_entries();

  PointContainer points_extrinsic;
  PointContainer points_pos;
  for (size_t image_id = 0; image_id < image_extrinsic_map.Size(); image_id++)
  {
    auto itr_pos_entry = pos_entries.find(image_id);
    if (image_extrinsic_map.IsValid(image_id) &&
        itr_pos_entry != pos_entries.end())
    {
      size_t extrinsic_id = image_extrinsic_map[image_id];
      const ExtrinsicParams& extrinsic_params =
        extrinsic_params_set[extrinsic_id];
      points_extrinsic.push_back(extrinsic_params.position());
      const PosEntry& pos_entry = itr_pos_entry->second;
      Point point_pos;
      point_pos<<pos_entry.x,
                 pos_entry.y,
                 pos_entry.z;
      points_pos.push_back(point_pos);
    }
  }

  Estimator estimator;
  Rotation similar_rotation;
  Translate similar_translate = Translate::Zero();
  Scalar similar_scale = Scalar(1);
  int result = estimator(points_extrinsic, points_pos,
                         similar_rotation,
                         similar_translate,
                         similar_scale);

  {
    std::string similar_path =
      photo_orientation_config->similar_transform_path();
    std::ofstream similar_file(similar_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(similar_file);
    archive(similar_scale, similar_rotation, similar_translate);
  }

  if (result == 0)
  {
    result_code_ |= RESULT_GEOREFERENCE;
  }

  return result;
}

int IncrementalPhotoOrientation::SaveIntrinsics(
  WorkflowStepConfig* config,
  const IntrinsicParamsContainer& intrinsic_params_set)
{
  typedef EIGEN_STD_MAP(size_t, IntrinsicParams) IntrinsicParamsMap;
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::string& intrinsic_path =
    photo_orientation_config->intrinsic_path();
  const std::vector<int>& intrinsic_ids =
    photo_orientation_config->intrinsic_ids();

  IntrinsicParamsMap intrinsic_map;
  for (size_t i = 0; i < intrinsic_params_set.size(); i++)
  {
    intrinsic_map.insert(std::make_pair(size_t(intrinsic_ids[i]),
                                        intrinsic_params_set[i]));
  }

  {
    std::ofstream intrinsic_file(intrinsic_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(intrinsic_file);
    archive(intrinsic_map);
  }

  return 0;
}

int IncrementalPhotoOrientation::SaveExtrinsics(
  WorkflowStepConfig* config,
  const ExtrinsicParamsContainer& extrinsic_params_set,
  const hs::sfm::ObjectIndexMap& image_extrinsic_map)
{
  typedef std::pair<size_t, size_t> ExtrinsicIndex;
  typedef EIGEN_STD_MAP(ExtrinsicIndex, ExtrinsicParams)
          ExtrinsicParamsMap;
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::vector<int>& image_ids =
    photo_orientation_config->image_ids();
  const std::string& extrinsic_path =
    photo_orientation_config->extrinsic_path();
  const hs::sfm::ObjectIndexMap& image_intrinsic_map =
    photo_orientation_config->image_intrinsic_map();
  const std::vector<int>& intrinsic_ids =
    photo_orientation_config->intrinsic_ids();

  ExtrinsicParamsMap extrinsic_map;

  for (size_t i = 0; i < image_ids.size(); i++)
  {
    int image_id = image_ids[i];
    if (image_extrinsic_map.IsValid(i))
    {
      size_t extrinsic_id = image_extrinsic_map[i];
      int intrinsic_id = intrinsic_ids[image_intrinsic_map[i]];
      extrinsic_map.insert(std::make_pair(std::make_pair(size_t(image_id),
                                                         size_t(intrinsic_id)),
                                          extrinsic_params_set[extrinsic_id]));
    }
  }

  {
    std::ofstream extrinsic_file(extrinsic_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(extrinsic_file);
    archive(extrinsic_map);
  }

  return 0;
}

int IncrementalPhotoOrientation::SavePointCloud(
  WorkflowStepConfig* config,
  const KeysetContainer& keysets,
  const hs::sfm::TrackContainer& tracks,
  const hs::sfm::ObjectIndexMap& track_point_map,
  const hs::sfm::ObjectIndexMap& image_extrinsic_map,
  const ExtrinsicParamsContainer& extrinsic_params_set,
  const IntrinsicParamsContainer& intrinsic_params_set,
  const PointContainer& points)
{
  typedef hs::imgio::whole::ImageData::Byte Byte;
  typedef std::array<Byte, 3> Color;
  typedef hs::graphics::PointCloudData<Scalar> PointCloudData;
  typedef hs::sfm::pipeline::PointCloudNormCalculator<Scalar> NormCalculator;
  typedef NormCalculator::CameraParams CameraParams;
  typedef NormCalculator::CameraParamsContainer CameraParamsContainer;

  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::vector<int> image_ids =
    photo_orientation_config->image_ids();
  const std::string point_cloud_path =
    photo_orientation_config->point_cloud_path();
  const std::vector<std::string> image_paths =
    photo_orientation_config->image_paths();
  const hs::sfm::ObjectIndexMap& image_intrinsic_map =
    photo_orientation_config->image_intrinsic_map();

  hs::sfm::CameraViewContainer camera_views(image_ids.size());
  for (size_t i = 0; i < tracks.size(); i++)
  {
    for (size_t j = 0; j < tracks[i].size(); j++)
    {
      size_t image_id = tracks[i][j].first;
      size_t key_id = tracks[i][j].second;
      camera_views[image_id].push_back(std::make_pair(i, key_id));
    }
  }

  //Set point cloud color.
  std::vector<bool> color_flags(points.size());
  Color color_default;
  color_default[0] = 0;
  color_default[1] = 0;
  color_default[2] = 0;
  std::vector<Color> colors(points.size());
  for (size_t i = 0; i < image_ids.size(); i++)
  {
    hs::imgio::whole::ImageData image_data;
    hs::imgio::whole::ImageIO image_io;
    if (image_io.LoadImage(image_paths[i], image_data) != 0)
    {
      return -1;
    }
    for (size_t j = 0; j < camera_views[i].size(); j++)
    {
      size_t track_id = camera_views[i][j].first;
      size_t key_id = camera_views[i][j].second;
      if (!track_point_map.IsValid(track_id)) continue;
      size_t point_id = track_point_map[track_id];
      if (!color_flags[point_id])
      {
        EIGEN_VECTOR(Scalar, 2) key = keysets[i][key_id];
        int row = int(key[1]);
        int col = int(key[0]);
        if (image_data.channel() == 1)
        {
          Byte byte = image_data.GetByte(row, col, 0);
          colors[point_id][0] = byte;
          colors[point_id][1] = byte;
          colors[point_id][2] = byte;
        }
        else
        {
          for (int k = 0; k < std::min(3, image_data.channel()); k++)
          {
            Byte byte = image_data.GetByte(row, col, k);
            colors[point_id][k] = byte;
          }
        }
        color_flags[point_id] = true;
      }
    }
  }

  //计算法向量
  PointContainer norms;
  {
    NormCalculator calculator;
    CameraParamsContainer camera_params_set;
    hs::imgio::whole::ImageIO image_io;
    for (size_t i = 0; i < image_intrinsic_map.Size(); i++)
    {
      if (image_intrinsic_map.IsValid(i) &&
          image_extrinsic_map.IsValid(i))
      {
        size_t intrinsic_id = image_intrinsic_map[i];
        size_t extrinsic_id = image_extrinsic_map[i];
        CameraParams camera_params;
        camera_params.intrinsic_params = intrinsic_params_set[intrinsic_id];
        camera_params.extrinsic_params = extrinsic_params_set[extrinsic_id];
        image_io.GetImageDimension(image_paths[i],
                                   camera_params.image_width,
                                   camera_params.image_height);
        camera_params_set.push_back(camera_params);
      }
    }
    calculator(camera_params_set, points, norms);
  }

  PointCloudData point_cloud_data;

  for (size_t i = 0; i < points.size(); i++)
  {
    PointCloudData::Vector3 point;
    point << (points[i][0]),
             (points[i][1]),
             (points[i][2]);
    point_cloud_data.VertexData().push_back(point);
    PointCloudData::Vector3 color;
    color << Scalar(colors[i][0]) / 255.0,
             Scalar(colors[i][1]) / 255.0,
             Scalar(colors[i][2]) / 255.0;
    point_cloud_data.ColorData().push_back(color);
    PointCloudData::Vector3 norm;
    norm << norms[i][0],
            norms[i][1],
            norms[i][2];
    point_cloud_data.NormalData().push_back(norm);
  }

  {
    std::ofstream point_cloud_file(point_cloud_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(point_cloud_file);
    archive(point_cloud_data);
  }

  return 0;
}

int IncrementalPhotoOrientation::SaveTracks(
  WorkflowStepConfig* config,
  const hs::sfm::TrackContainer& tracks,
  const hs::sfm::ObjectIndexMap& track_point_map,
  const hs::sfm::ViewInfoIndexer& view_info_indexer)
{
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::vector<int> image_ids =
    photo_orientation_config->image_ids();
  const std::string& tracks_path =
    photo_orientation_config->tracks_path();

  hs::sfm::TrackContainer tracks_wrap;
  for (size_t i = 0; i < tracks.size(); i++)
  {
    hs::sfm::Track track;
    for (size_t j = 0; j < tracks[i].size(); j++)
    {
      size_t image_id = tracks[i][j].first;
      size_t key_id = tracks[i][j].second;
      const hs::sfm::ViewInfo* view_info =
        view_info_indexer.GetViewInfoByTrackImage(i, image_id);
      if (view_info != nullptr &&
        !view_info->is_blunder)
      {
        track.push_back(std::make_pair(size_t(image_ids[image_id]),
                                       key_id));
      }
    }
    tracks_wrap.push_back(track);
  }

  {
    std::ofstream tracks_file(tracks_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(tracks_file);
    archive(tracks_wrap, track_point_map);
  }

  return 0;
}

int IncrementalPhotoOrientation::RunImplement(WorkflowStepConfig* config)
{
  int result = 0;
  while (1)
  {
    progress_manager_.AddSubProgress(0.01f);
    KeysetContainer keysets;
    result = LoadKeysets(config, keysets);
    if (result != 0)
    {
      std::cout<<"LoadKeysets Error!\n";
      break;
    }

    hs::sfm::MatchContainer matches;
    result = LoadMatches(config, matches);
    if (result != 0)
    {
      std::cout<<"LoadMatches Error!\n";
      break;
    }
    progress_manager_.FinishCurrentSubProgress();

    progress_manager_.AddSubProgress(0.98f);
    IntrinsicParamsContainer intrinsic_params_set;
    ExtrinsicParamsContainer extrinsic_params_set;
    hs::sfm::ObjectIndexMap image_extrinsic_map;
    PointContainer points;
    hs::sfm::TrackContainer tracks;
    hs::sfm::ObjectIndexMap track_point_map;
    hs::sfm::ViewInfoIndexer view_info_indexer;
    result = RunSFM(config,
                    keysets,
                    matches,
                    intrinsic_params_set,
                    extrinsic_params_set,
                    image_extrinsic_map,
                    points,
                    tracks,
                    track_point_map,
                    view_info_indexer);
    if (result != 0)
    {
      std::cout<<"RunSFM Error!\n";
      break;
    }
    progress_manager_.FinishCurrentSubProgress();

    progress_manager_.AddSubProgress(0.01f);
    SimilarTransformByPosEntries(
      config, extrinsic_params_set, image_extrinsic_map);

    result = SaveIntrinsics(config, intrinsic_params_set);
    if (result != 0)
    {
      std::cout<<"SaveIntrinsics Error!\n";
      break;
    }

    result = SaveExtrinsics(config, extrinsic_params_set, image_extrinsic_map);
    if (result != 0)
    {
      std::cout<<"SaveExtrinsics Error!\n";
      break;
    }

    result = SavePointCloud(config, keysets, tracks, track_point_map,
                            image_extrinsic_map, extrinsic_params_set,
                            intrinsic_params_set, points);
    if (result != 0)
    {
      std::cout<<"SavePointCloud Error!\n";
      break;
    }

    result = SaveTracks(config, tracks, track_point_map, view_info_indexer);
    if (result != 0)
    {
      std::cout<<"SaveTracks Error!\n";
    }

    progress_manager_.FinishCurrentSubProgress();

    break;
  }
  return result;
}

}
}
}
