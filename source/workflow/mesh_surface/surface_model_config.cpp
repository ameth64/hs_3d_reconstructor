#include "workflow/mesh_surface/surface_model_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

MeshSurfaceConfig::MeshSurfaceConfig()
{
  type_ = STEP_SURFACE_MODEL;
}
void MeshSurfaceConfig::set_xml_path(const std::string& xml_path){
  xml_path_ = xml_path;
}
void MeshSurfaceConfig::set_point_cloud_path(const std::string& pointcloud_path){
  pointcloud_path_ = pointcloud_path;
}
void MeshSurfaceConfig::set_octree_depth(const int& octree_depth){ 
  octree_depth_ = octree_depth; 
}
void MeshSurfaceConfig::set_core_use(const int& core_use){
  core_use_ = core_use;
}
void MeshSurfaceConfig::set_min_depth(const int& min_depth){
  min_depth_ = min_depth;
}
void MeshSurfaceConfig::set_point_weight(const float& point_weight){
  point_weight_ = point_weight;
}
void MeshSurfaceConfig::set_cube_ratio(const float& cube_ratio){
  cube_ratio_ = cube_ratio;
}
void MeshSurfaceConfig::set_solver_divide(const int& solver_divide){
  solver_divide_ = solver_divide;
}
void MeshSurfaceConfig::set_iso_divide(const int& iso_divide){
  iso_divide_ = iso_divide;
}
void MeshSurfaceConfig::set_samples_per_node(const float& samples_per_node){
  samples_per_node_ = samples_per_node;
}
void MeshSurfaceConfig::set_min_iters(const int& min_iters){
  min_iters_ = min_iters;
}
void MeshSurfaceConfig::set_solver_accuracy(const float& solver_accuracy){
  solver_accuracy_ = solver_accuracy;
}
void MeshSurfaceConfig::set_confidence(const int& confidence){
  confidence_ = confidence;
}
void MeshSurfaceConfig::set_polygon_mesh(const int& polygon_mesh){
  polygon_mesh_ = polygon_mesh;
}
void MeshSurfaceConfig::set_output_dir(const std::string& output_dir){
  output_dir_ = output_dir;
}

const std::string& MeshSurfaceConfig::xml_path()const{
  return xml_path_;
}
const std::string& MeshSurfaceConfig::pointcloud_path()const{
  return pointcloud_path_;
}
const std::string& MeshSurfaceConfig::output_dir()const{
  return output_dir_;
}
const int& MeshSurfaceConfig::octree_depth()const{
  return octree_depth_;
}
const int& MeshSurfaceConfig::core_use()const{
  return core_use_;
}
const int& MeshSurfaceConfig::min_depth()const{
  return min_depth_;
}
const int& MeshSurfaceConfig::solver_divide()const{
  return solver_divide_;
}
const int& MeshSurfaceConfig::iso_divide()const{
  return iso_divide_;
}
const int& MeshSurfaceConfig::min_iters()const{
  return min_iters_;
}
const int& MeshSurfaceConfig::confidence()const{
  return confidence_;
}
const int& MeshSurfaceConfig::polygon_mesh()const{
  return polygon_mesh_;
}
const float& MeshSurfaceConfig::point_weight()const{
  return point_weight_;
}
const float& MeshSurfaceConfig::cube_ratio()const{
  return cube_ratio_;
}
const float& MeshSurfaceConfig::solver_accuracy()const{
  return solver_accuracy_;
}
const float& MeshSurfaceConfig::samples_per_node()const{
  return samples_per_node_;
}

}
}
}