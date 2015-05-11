#include "workflow/mesh_surface/poisson_surface_model.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#ifdef WIN32
#include <Windows.h>
#endif
#include <string>

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

      int PoissonSurface::CreateConfigXml(MeshSurfaceConfig* config)
      {
        boost::property_tree::ptree config_tree;
        config_tree.put("module_list.module_item.module_name",
          "mesh_surface");
        config_tree.put("module_list.module_item.module_version",
          "1.0");
        config_tree.put("module_list.module_item.pointcloud",
          config->pointcloud_path());
        config_tree.put("module_list.module_item.option_list.octree_depth",
          config->octree_depth());
        config_tree.put("module_list.module_item.option_list.core_use",
          config->core_use());
        config_tree.put("module_list.module_item.option_list.min_depth",
          config->min_depth());
        config_tree.put("module_list.module_item.option_list.point_weight",
          config->point_weight());
        config_tree.put("module_list.module_item.option_list.cube_ratio",
          config->cube_ratio());
        config_tree.put("module_list.module_item.option_list.solver_divide",
          config->solver_divide());
        config_tree.put("module_list.module_item.option_list.iso_divide",
          config->iso_divide());
        config_tree.put("module_list.module_item.option_list.samples_per_node",
          config->samples_per_node());
        config_tree.put("module_list.module_item.option_list.min_iters",
          config->min_iters());
        config_tree.put("module_list.module_item.option_list.solver_accuracy",
          config->solver_accuracy());
        config_tree.put("module_list.module_item.option_list.confidence",
          config->confidence());
        config_tree.put("module_list.module_item.option_list.polygon_mesh",
          config->polygon_mesh());
        config_tree.put("module_list.module_item.option_list.output_dir",
          config->output_dir());
        //boost::property_tree::xml_parser::xml_writer_settings<boost::property_tree::ptree::key_type> settings(' ', 2);
        boost::property_tree::xml_parser::xml_writer_settings<char> settings(' ', 2);
        write_xml(config->xml_path(), config_tree, std::locale(), settings);
        return 0;
        
      }

      PoissonSurface::PoissonSurface()
      {
        type_ = STEP_SURFACE_MODEL;
      }

      int PoissonSurface::RunPoissonSurface(WorkflowStepConfig* config)
      {
        MeshSurfaceConfig* surface_model_config =
          static_cast<MeshSurfaceConfig*>(config);
        if (CreateConfigXml(surface_model_config) != 0)
        {
          return -1;
        }
        bm::CBaseAgent* agent = ms::CMSAgent::create();
        agent->init();
        agent->input(surface_model_config->xml_path().c_str());
        agent->start(bm::CBaseDefine::E_MX_ASYNC);

        int ms_progress = 0;
        while (ms_progress != 100)
        {
          if (!progress_manager_.CheckKeepWorking())
          {
            break;
          }
#ifdef WIN32
          Sleep(100);
#else
          //something like Sleep
#endif
          ms_progress = agent->getProgress();
          progress_manager_.SetCurrentSubProgressCompleteRatio(
            float(ms_progress)/float(100));

        }

        agent->stop();

        std::string outputPath = surface_model_config->output_dir() + "output.xml";
        agent->output(outputPath.c_str());

        agent->clear();

        agent->uninit();

        ms::CMSAgent::destroy((ms::CMSAgent*)agent);
        return 0;

      }

      int PoissonSurface::RunImplement(WorkflowStepConfig *config)
      {
        return RunPoissonSurface(config);
      }
    }
  }
}