#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_TEXTURE_ROUGH_TEXTURE_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_TEXTURE_ROUGH_TEXTURE_HPP_

#include <array>

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "workflow/common/workflow_step.hpp"

#include "hs_math/linear_algebra/eigen_macro.hpp"
#include "hs_math/geometry/rotation.hpp"

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
  void set_surface_model_path(const std::string& surface_model_path);
  void set_similar_transform(const SimilarTransform& similar_transform);

  double dem_x_scale() const;
  double dem_y_scale() const;
  const std::string& dem_path() const;
  int dem_tile_x_size() const;
  int dem_tile_y_size() const;
  const std::string& surface_model_path() const;
  const SimilarTransform& similar_transform() const;

private:
  double dem_x_scale_;
  double dem_y_scale_;
  std::string dem_path_;
  int dem_tile_x_size_;
  int dem_tile_y_size_;
  std::string surface_model_math_;
  SimilarTransform similar_transform_;
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

protected:
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif