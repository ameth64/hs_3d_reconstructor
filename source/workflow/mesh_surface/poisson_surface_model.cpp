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
        boost::property_tree::xml_parser::xml_writer_settings<boost::property_tree::ptree::key_type> settings(' ', 2);
        //boost::property_tree::xml_parser::xml_writer_settings<char> settings(' ', 2);
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
