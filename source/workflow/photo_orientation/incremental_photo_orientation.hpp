#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_INCREMENTAL_PHOTO_ORIENTATION_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_INCREMENTAL_PHOTO_ORIENTATION_HPP_

#include "hs_sfm/sfm_utility/camera_type.hpp"
#include "hs_sfm/sfm_utility/match_type.hpp"
#include "hs_sfm/incremental/incremental.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/common/workflow_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT PhotoOrientationConfig : public WorkflowStepConfig
{
public:
  typedef double Scalar;
  typedef hs::sfm::CameraIntrinsicParams<Scalar> IntrinsicParams;
  typedef EIGEN_STD_VECTOR(IntrinsicParams) IntrinsicParamsContainer;

public:
  PhotoOrientationConfig();

  void set_image_intrinsic_map(
    const hs::sfm::ObjectIndexMap& image_intrinsic_map);
  void set_matches_path(const std::string& matches_path);
  void set_image_paths(const std::vector<std::string>& image_paths);
  void set_key_paths(const std::vector<std::string>& key_paths);
  void set_image_ids(const std::vector<int>& image_ids);
  void set_intrinsic_params_set(
    const IntrinsicParamsContainer& intrinsic_params_set);
  void set_intrinsic_ids(const std::vector<int>& intrinsic_ids);
  void set_intrinsic_path(const std::string& intrinsic_path);
  void set_extrinsic_path(const std::string& extrinsic_path);
  void set_point_cloud_path(const std::string& point_cloud_path);
  void set_number_of_threads(int number_of_threads);

  const hs::sfm::ObjectIndexMap& image_intrinsic_map() const;
  const std::string& matches_path() const;
  const std::vector<std::string>& image_paths() const;
  const std::vector<std::string>& key_paths() const;
  const std::vector<int>& image_ids() const;
  const IntrinsicParamsContainer& intrinsic_params_set() const;
  const std::vector<int>& intrinsic_ids() const;
  const std::string& intrinsic_path() const;
  const std::string& extrinsic_path() const;
  const std::string& point_cloud_path() const;
  int number_of_threads() const;

private:
  hs::sfm::ObjectIndexMap image_intrinsic_map_;
  std::string matches_path_;
  std::vector<std::string> image_paths_;
  std::vector<std::string> key_paths_;
  std::vector<int> image_ids_;
  IntrinsicParamsContainer intrinsic_params_set_;
  std::vector<int> intrinsic_ids_;
  std::string intrinsic_path_;
  std::string extrinsic_path_;
  std::string point_cloud_path_;
  int number_of_threads_;
};

typedef std::shared_ptr<PhotoOrientationConfig> PhotoOrientationConfigPtr;

class HS_EXPORT IncrementalPhotoOrientation : public WorkflowStep
{
public:
  IncrementalPhotoOrientation();

private:
  typedef PhotoOrientationConfig::Scalar Scalar;
  typedef hs::sfm::incremental::IncrementalSFM<Scalar> SFM;
  typedef SFM::Keyset Keyset;
  typedef SFM::KeysetContainer KeysetContainer;
  typedef SFM::ExtrinsicParams ExtrinsicParams;
  typedef SFM::ExtrinsicParamsContainer ExtrinsicParamsContainer;
  typedef SFM::IntrinsicParams IntrinsicParams;
  typedef SFM::IntrinsicParamsContainer IntrinsicParamsContainer;
  typedef SFM::Point Point;
  typedef SFM::PointContainer PointContainer;

  int LoadKeysets(WorkflowStepConfig* config, KeysetContainer& keysets);
  int LoadMatches(WorkflowStepConfig* config,
                  hs::sfm::MatchContainer& matches);
  int RunSFM(WorkflowStepConfig* config,
             const KeysetContainer& keysets,
             const hs::sfm::MatchContainer& matches,
             IntrinsicParamsContainer& intrinsic_params_set,
             ExtrinsicParamsContainer& extrinsic_params_set,
             hs::sfm::ObjectIndexMap& image_extrinsic_map,
             PointContainer& points,
             hs::sfm::TrackContainer& tracks,
             hs::sfm::ObjectIndexMap& track_point_map,
             hs::sfm::ViewInfoIndexer& view_info_indexer);
  int SaveIntrinsics(WorkflowStepConfig* config,
                     const IntrinsicParamsContainer& intrinsic_params_set);
  int SaveExtrinsics(WorkflowStepConfig* config,
                     const ExtrinsicParamsContainer& extrinsic_params_set,
                     const hs::sfm::ObjectIndexMap& image_extrinsic_map);
  int SavePointCloud(WorkflowStepConfig* config,
                     const KeysetContainer& keysets,
                     const hs::sfm::TrackContainer& tracks,
                     const hs::sfm::ObjectIndexMap& track_point_map,
                     const PointContainer& points);

protected:
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif
