#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_TEXTURE_ROUGH_TEXTURE_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_TEXTURE_ROUGH_TEXTURE_HPP_

#include <array>

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "workflow/common/workflow_step.hpp"

#include "hs_math/linear_algebra/eigen_macro.hpp"
#include "hs_math/geometry/rotation.hpp"
#include "hs_sfm/sfm_utility/camera_type.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT TextureConfig : public WorkflowStepConfig
{
public:
  TextureConfig();

  typedef double Scalar;
  typedef hs::math::geometry::Rotation3D<Scalar> Rotation;
  typedef EIGEN_VECTOR(Scalar, 3) Translate;

  typedef hs::sfm::CameraExtrinsicParams<Scalar> ExtrinsicParams;
  typedef hs::sfm::CameraIntrinsicParams<Scalar> IntrinsicParams;
  struct ImageParams
  {
    ExtrinsicParams extrinsic_params;
    IntrinsicParams intrinsic_params;
    std::string image_path;
    size_t image_width;
    size_t image_height;
  };
  typedef EIGEN_STD_VECTOR(ImageParams) ImageParamsContainer;

  struct SimilarTransform
  {
    Rotation rotation;
    Translate translate;
    Scalar scale;
  };

  void set_dem_x_scale(double dem_x_scale);
  void set_dem_y_scale(double dem_y_scale);
  void set_dem_path(const std::string& dem_path);
  void set_dem_tile_x_size(int dem_tile_x_size);
  void set_dem_tile_y_size(int dem_tile_y_size);
  void set_dom_x_scale(double dom_x_scale);
  void set_dom_y_scale(double dom_y_scale);
  void set_dom_path(const std::string& dom_path);
  void set_dom_tile_x_size(int dom_tile_x_size);
  void set_dom_tile_y_size(int dom_tile_y_size);
  void set_surface_model_path(const std::string& surface_model_path);
  void set_similar_transform(const SimilarTransform& similar_transform);
  void set_images(const ImageParamsContainer& images);

  double dem_x_scale() const;
  double dem_y_scale() const;
  const std::string& dem_path() const;
  int dem_tile_x_size() const;
  int dem_tile_y_size() const;
  double dom_x_scale() const;
  double dom_y_scale() const;
  const std::string& dom_path() const;
  int dom_tile_x_size() const;
  int dom_tile_y_size() const;
  const std::string& surface_model_path() const;
  const SimilarTransform& similar_transform() const;
  const ImageParamsContainer& images() const;

private:
  double dem_x_scale_;
  double dem_y_scale_;
  std::string dem_path_;
  int dem_tile_x_size_;
  int dem_tile_y_size_;
  double dom_x_scale_;
  double dom_y_scale_;
  std::string dom_path_;
  int dom_tile_x_size_;
  int dom_tile_y_size_;
  std::string surface_model_math_;
  SimilarTransform similar_transform_;
  ImageParamsContainer images_;
};
typedef std::shared_ptr<TextureConfig> TextureConfigPtr;

class HS_EXPORT RoughTexture : public WorkflowStep
{
public:
  RoughTexture();

private:
  typedef double Scalar;
  typedef EIGEN_VECTOR(Scalar, 3) Vertex;
  typedef EIGEN_STD_VECTOR(Vertex) VertexContainer;
  typedef std::array<size_t, 3> Triangle;
  typedef std::vector<Triangle> TriangleContainer;
  typedef EIGEN_VECTOR(Scalar, 2) PlanarPoint;

  int LoadSurfaceModel(WorkflowStepConfig* config,
                       VertexContainer& vertices,
                       TriangleContainer& triangles);

  int GenerateDEM(WorkflowStepConfig* config,
                  const VertexContainer& vertices,
                  const TriangleContainer& triangles);

  int GenerateDOM(WorkflowStepConfig* config,
                  const VertexContainer& vertices,
                  const TriangleContainer& triangles);

protected:
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif