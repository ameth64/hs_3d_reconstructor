#include <fstream>
#include <vector>

#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include "workflow/mesh_surface/delaunay_surface_model.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

DelaunaySurfaceModel::DelaunaySurfaceModel()
{
  type_ = STEP_SURFACE_MODEL;
}

int DelaunaySurfaceModel::LoadPointCloudData(WorkflowStepConfig* config,
                                             PointCloudData& point_cloud_data)
{
  MeshSurfaceConfig* surface_model_config =
    static_cast<MeshSurfaceConfig*>(config);

  {
    std::ifstream point_cloud_file(surface_model_config->pointcloud_path(),
                                   std::ios::binary);
    if (!point_cloud_file) return -1;
    cereal::PortableBinaryInputArchive archive(point_cloud_file);
    archive(point_cloud_data);
  }
  
  return 0;
}

int DelaunaySurfaceModel::DelaunayTriangulate(
  WorkflowStepConfig* config,
  const PointCloudData& point_cloud_data,
  VertexContainer& vertices,
  TriangleContainer& triangles)
{
  typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
  typedef CGAL::Triangulation_vertex_base_with_info_2<size_t, Kernel> Vb;
  typedef CGAL::Triangulation_data_structure_2<Vb> Tds;
  typedef CGAL::Delaunay_triangulation_2<Kernel, Tds> Delaunay;
  typedef Kernel::Point_2 Point;

  std::vector<std::pair<Point, size_t> > points;
  vertices.resize(point_cloud_data.PointCloudSize());
  for (size_t i = 0; i < point_cloud_data.PointCloudSize(); i++)
  {
    Vertex point = point_cloud_data.VertexData()[i];
    points.push_back(std::make_pair(Point(point[0], point[1]), i));
    vertices[i] = point;
  }

  Delaunay triangulation;
  triangulation.insert(points.begin(), points.end());

  for (Delaunay::Finite_faces_iterator itr_face =
         triangulation.finite_faces_begin();
       itr_face != triangulation.finite_faces_end(); ++itr_face)
  {
    Delaunay::Face_handle face = itr_face;
    Triangle triangle;
    
    triangle[0] = face->vertex(0)->info();
    triangle[1] = face->vertex(1)->info();
    triangle[2] = face->vertex(2)->info();

    triangles.push_back(triangle);
  }

  return 0;
}

int DelaunaySurfaceModel::SaveSurfaceModel(WorkflowStepConfig* config,
  const VertexContainer& vertices,
  const TriangleContainer& triangles)
{
  MeshSurfaceConfig* surface_model_config =
    static_cast<MeshSurfaceConfig*>(config);

  {
    std::string mesh_path =
      surface_model_config->output_dir() + "/mesh.bin";
    std::ofstream mesh_file(mesh_path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(mesh_file);
    archive(vertices, triangles);
  }

  return 0;
}

int DelaunaySurfaceModel::RunImplement(WorkflowStepConfig* config)
{
  int result = -1;
  while (1)
  {
    progress_manager_.AddSubProgress(0.1f);
    PointCloudData point_cloud_data;
    result = LoadPointCloudData(config, point_cloud_data);
    if (result != 0) break;
    progress_manager_.FinishCurrentSubProgress();

    progress_manager_.AddSubProgress(0.8f);
    VertexContainer vertices;
    TriangleContainer triangles;
    result = DelaunayTriangulate(config, point_cloud_data, vertices, triangles);
    if (result != 0) break;
    progress_manager_.FinishCurrentSubProgress();

    progress_manager_.AddSubProgress(0.1f);
    result = SaveSurfaceModel(config, vertices, triangles);
    if (result != 0) break;
    progress_manager_.FinishCurrentSubProgress();

    break;
  }

  return result;
}

}
}
}