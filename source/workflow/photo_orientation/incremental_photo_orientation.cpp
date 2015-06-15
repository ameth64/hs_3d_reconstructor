#include <fstream>
#include <iomanip>
#include <iostream>
#include <array>

#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp> 

#include <cereal/archives/json.hpp>

#include "hs_sfm/sfm_file_io/keyset_loader.hpp"
#include "hs_sfm/sfm_file_io/matches_loader.hpp"
#include "hs_sfm/sfm_utility/similar_transform_estimator.hpp"
#include "hs_image_io/whole_io/image_data.hpp"
#include "hs_image_io/whole_io/image_io.hpp"

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
void PhotoOrientationConfig::set_key_paths(
  const std::vector<std::string>& key_paths)
{
  key_paths_ = key_paths;
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
const std::vector<std::string>& PhotoOrientationConfig::key_paths() const
{
  return key_paths_;
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
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::vector<std::string>& key_paths =
    photo_orientation_config->key_paths();

  keysets.clear();
  hs::sfm::fileio::KeysetLoader<Scalar> loader;
  for (size_t i = 0; i < key_paths.size(); i++)
  {
    Keyset keyset;
    if (loader(key_paths[i], keyset) != 0)
    {
      return -1;
    }
    keysets.push_back(keyset);
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

  hs::sfm::fileio::MatchesLoader loader;
  return loader(matches_path, matches);

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
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const hs::sfm::ObjectIndexMap& image_intrinsic_map =
    photo_orientation_config->image_intrinsic_map();
  intrinsic_params_set =
    photo_orientation_config->intrinsic_params_set();

  SFM sfm(100, 8, 2, size_t(photo_orientation_config->number_of_threads()));
  return sfm(image_intrinsic_map,
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
  Translate similar_translate;
  Scalar similar_scale;
  int result = estimator(points_extrinsic, points_pos,
                         similar_rotation,
                         similar_translate,
                         similar_scale);

  if (result == 0)
  {
    std::string similar_path =
      photo_orientation_config->similar_transform_path();
    std::ofstream similar_file(similar_path);
    if (!similar_file)
    {
      result = -1;
    }
    else
    {
      similar_file.setf(std::ios::fixed);
      similar_file<<std::setprecision(10);
      similar_file<<similar_scale<<"\n";
      similar_file<<similar_rotation[0]<<"\n";
      similar_file<<similar_rotation[1]<<"\n";
      similar_file<<similar_rotation[2]<<"\n";
      similar_file<<similar_translate[0]<<"\n";
      similar_file<<similar_translate[1]<<"\n";
      similar_file<<similar_translate[2];
      similar_file.close();
    }
  }

  return result;
}

int IncrementalPhotoOrientation::SaveIntrinsics(
  WorkflowStepConfig* config,
  const IntrinsicParamsContainer& intrinsic_params_set)
{
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::string& intrinsic_path =
    photo_orientation_config->intrinsic_path();
  const std::vector<int>& intrinsic_ids =
    photo_orientation_config->intrinsic_ids();

  std::ofstream intrinsic_file(intrinsic_path.c_str());
  if (!intrinsic_file)
  {
    return -1;
  }
  intrinsic_file.setf(std::ios::fixed);
  intrinsic_file<<std::setprecision(8);

  for (size_t i = 0; i < intrinsic_params_set.size(); i++)
  {
    const IntrinsicParams& intrinsic_param = intrinsic_params_set[i];
    int intrinsic_id = intrinsic_ids[i];
    intrinsic_file << intrinsic_id << " "
                   << intrinsic_param.focal_length() << " "
                   << intrinsic_param.skew() << " "
                   << intrinsic_param.principal_point_x() << " "
                   << intrinsic_param.principal_point_y() << " "
                   << intrinsic_param.pixel_ratio() << " "
                   << intrinsic_param.k1() << " "
                   << intrinsic_param.k2() << " "
                   << intrinsic_param.k3() << " "
                   << intrinsic_param.d1() << " "
                   << intrinsic_param.d2() << "\n";
  }

  return 0;
}

int IncrementalPhotoOrientation::SaveExtrinsics(
  WorkflowStepConfig* config,
  const ExtrinsicParamsContainer& extrinsic_params_set,
  const hs::sfm::ObjectIndexMap& image_extrinsic_map)
{
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

  std::ofstream extrinsic_file(extrinsic_path.c_str());
  if (!extrinsic_file)
  {
    return -1;
  }
  extrinsic_file.setf(std::ios::fixed);
  extrinsic_file<<std::setprecision(8);

  for (size_t i = 0; i < image_ids.size(); i++)
  {
    int image_id = image_ids[i];
    int intrinsic_id = intrinsic_ids[image_intrinsic_map[i]];
    if (image_extrinsic_map.IsValid(i))
    {
      int extrinsic_id = int(image_extrinsic_map[i]);
      const ExtrinsicParams& extrinsic_params =
        extrinsic_params_set[extrinsic_id];;
      const ExtrinsicParams::Position& position = extrinsic_params.position();
      const ExtrinsicParams::Rotation& rotation = extrinsic_params.rotation();

      extrinsic_file<<image_id<<" "<<intrinsic_id<<" "
                    <<position[0]<<" "<<position[1]<<" "<<position[2]<<" "
                    <<rotation[0]<<" "<<rotation[1]<<" "<<rotation[2]<<"\n";
    }
    else
    {
      extrinsic_file<<image_id<<" "<<intrinsic_id<<" 0 0 0 0 0 0\n";
    }
  }

  return 0;
}

int IncrementalPhotoOrientation::SavePointCloud(
  WorkflowStepConfig* config,
  const KeysetContainer& keysets,
  const hs::sfm::TrackContainer& tracks,
  const hs::sfm::ObjectIndexMap& track_point_map,
  const PointContainer& points)
{
  typedef hs::imgio::whole::ImageData::Byte Byte;
  typedef std::array<Byte, 3> Color;

  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::vector<int> image_ids =
    photo_orientation_config->image_ids();
  const std::string point_cloud_path =
    photo_orientation_config->point_cloud_path();
  const std::vector<std::string> image_paths =
    photo_orientation_config->image_paths();

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

  //Save ply file.
  std::ofstream ply_file(point_cloud_path.c_str());
  if (!ply_file)
  {
    return -1;
  }
  ply_file.setf(std::ios::fixed);
  ply_file<<std::setprecision(8);

  ply_file<<"ply\n";
  ply_file<<"format ascii 1.0\n";
  ply_file<<"element vertex "<<points.size()<<"\n";
  ply_file<<"property float64 x\n";
  ply_file<<"property float64 y\n";
  ply_file<<"property float64 z\n";
  ply_file<<"property float64 nx\n";
  ply_file<<"property float64 ny\n";
  ply_file<<"property float64 nz\n";
  ply_file<<"property uint8 diffuse_red\n";
  ply_file<<"property uint8 diffuse_green\n";
  ply_file<<"property uint8 diffuse_blue\n";
  ply_file<<"end_header\n";

  for (size_t i = 0; i < points.size(); i++)
  {
    //TODO:Norm calculation needed.
    ply_file
      <<points[i][0]<<" "<<points[i][1]<<" "<<points[i][2]<<" 1 0 0 "
      <<int(colors[i][0])<<" "<<int(colors[i][1])<<" "<<int(colors[i][2])<<"\n";
  }

  return 0;
}

int IncrementalPhotoOrientation::SaveTracks(
  WorkflowStepConfig* config,
  const hs::sfm::TrackContainer& tracks,
  const hs::sfm::ObjectIndexMap& track_point_map)
{
  PhotoOrientationConfig* photo_orientation_config =
    static_cast<PhotoOrientationConfig*>(config);

  const std::vector<int> image_ids =
    photo_orientation_config->image_ids();
  const std::string& tracks_path =
    photo_orientation_config->tracks_path();

  hs::sfm::TrackContainer tracks_wrap = tracks;
  for (size_t i = 0; i < tracks_wrap.size(); i++)
  {
    for (size_t j = 0; j < tracks_wrap[i].size(); j++)
    {
      tracks_wrap[i][j].first = size_t(image_ids[tracks_wrap[i][j].first]);
    }
  }

  {
    std::ofstream tracks_file(tracks_path);
    cereal::JSONOutputArchive archive(tracks_file);
    archive(tracks_wrap, track_point_map);
  }


  return 0;
}

int IncrementalPhotoOrientation::RunImplement(WorkflowStepConfig* config)
{
  int result = 0;
  while (1)
  {
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

    result = SavePointCloud(config, keysets, tracks, track_point_map, points);
    if (result != 0)
    {
      std::cout<<"SavePointCloud Error!\n";
      break;
    }

    result = SaveTracks(config, tracks, track_point_map);
    if (result != 0)
    {
      std::cout<<"SaveTracks Error!\n";
    }

    break;
  }
  return result;
}

}
}
}
