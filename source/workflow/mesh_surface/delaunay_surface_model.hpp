#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_DELAUNAY_SURFACE_MODEL_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_DELAUNAY_SURFACE_MODEL_HPP_

#include <array>

#include "hs_math/linear_algebra/eigen_macro.hpp"
#include "hs_graphics/graphics_utility/pointcloud_data.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"

#include "workflow/common/workflow_step.hpp"
#include "workflow/mesh_surface/surface_model_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

class HS_EXPORT DelaunaySurfaceModel : public WorkflowStep
{
public:
  typedef double Scalar;
  typedef EIGEN_VECTOR(Scalar, 3) Vertex;
  typedef EIGEN_STD_VECTOR(Vertex) VertexContainer;
  typedef std::array<size_t, 3> Triangle;
  typedef std::vector<Triangle> TriangleContainer;

  typedef hs::graphics::PointCloudData<Scalar> PointCloudData;

public:
  DelaunaySurfaceModel();
protected:
  int LoadPointCloudData(WorkflowStepConfig* config,
                         PointCloudData& point_cloud_data);
  int DelaunayTriangulate(WorkflowStepConfig* config,
                          const PointCloudData& point_cloud_data,
                          VertexContainer& vertices,
                          TriangleContainer& triangles);
  int SaveSurfaceModel(WorkflowStepConfig* config,
                       const VertexContainer& vertices,
                       const TriangleContainer& triangles);
  virtual int RunImplement(WorkflowStepConfig* config);
};

}
}
}

#endif