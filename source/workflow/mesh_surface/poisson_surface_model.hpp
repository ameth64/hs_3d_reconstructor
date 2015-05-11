#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_POISSON_SURFACE_MODEL_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_POISSON_SURFACE_MODEL_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "workflow/common/workflow_step.hpp"
#include "hs_flowmodule/mesh_surface/agent/ms_agent.hpp"

namespace hs
{
  namespace recon
  {
    namespace workflow
    {
      class HS_EXPORT MeshSurfaceConfig : public WorkflowStepConfig
      {
      public:
        MeshSurfaceConfig();
        void set_xml_path(const std::string& xml_path);
        void set_point_cloud_path(const std::string& pointcloud_path);
        void set_octree_depth(const int& octree_depth);
        void set_core_use(const int& core_use);
        void set_min_depth(const int& min_depth);
        void set_point_weight(const float& point_weight);          
        void set_cube_ratio(const float& cube_ratio);
        void set_solver_divide(const int& solver_divide);
        void set_iso_divide(const int& iso_divide);
        void set_samples_per_node(const float& samples_per_node);
        void set_min_iters(const int& min_iters);
        void set_solver_accuracy(const float& solver_accuracy);
        void set_confidence(const int& confidence);
        void set_polygon_mesh(const int& polygon_mesh);
        void set_output_dir(const std::string& output_dir);

        void set_dem_x_scale(double dem_x_scale);
        void set_dem_y_scale(double dem_y_scale);
        void set_dem_path(const std::string& dem_path);
        void set_dem_tile_x_size(int dem_tile_x_size);
        void set_dem_tile_y_size(int dem_tile_y_size);

        const std::string& xml_path()const;
        const std::string& pointcloud_path()const;
        const std::string& output_dir()const;
        const int& octree_depth()const;
        const int& core_use()const;
        const int& min_depth()const;
        const int& solver_divide()const;
        const int& iso_divide()const;
        const int& min_iters()const;
        const int& confidence()const;
        const int& polygon_mesh()const;
        const float& point_weight()const;
        const float& cube_ratio()const;
        const float& solver_accuracy()const;
        const float& samples_per_node()const;

        double dem_x_scale() const;
        double dem_y_scale() const;
        const std::string& dem_path() const;
        int dem_tile_x_size() const;
        int dem_tile_y_size() const;

      private:
          std::string xml_path_;
          std::string pointcloud_path_;
          std::string output_dir_;
          int octree_depth_;
          int core_use_;
          int min_depth_;
          int solver_divide_;
          int iso_divide_;
          int min_iters_;
          int confidence_;
          int polygon_mesh_;
          float point_weight_;
          float cube_ratio_;
          float solver_accuracy_;
          float samples_per_node_;

          double dem_x_scale_;
          double dem_y_scale_;
          std::string dem_path_;
          int dem_tile_x_size_;
          int dem_tile_y_size_;
      };
      typedef std::shared_ptr<MeshSurfaceConfig> MeshSurfaceConfigPtr;
      class HS_EXPORT PoissonSurface : public WorkflowStep
      {
      public:
        PoissonSurface();
      private:
        int RunPoissonSurface(WorkflowStepConfig* config);
        int CreateConfigXml(MeshSurfaceConfig* config);
        int GenerateDEM(MeshSurfaceConfig* config);
      protected:
        virtual int RunImplement(WorkflowStepConfig* config);
      };

    }
  }
}



#endif //_HS_3D_RECONSTRUCTOR_WORKFLOW_POISSON_SURFACE_MODEL_HPP_