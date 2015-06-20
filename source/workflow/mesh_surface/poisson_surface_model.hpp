#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_POISSON_SURFACE_MODEL_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_POISSON_SURFACE_MODEL_HPP_

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "workflow/common/workflow_step.hpp"
#include "workflow/mesh_surface/surface_model_config.hpp"
#include "hs_flowmodule/mesh_surface/agent/ms_agent.hpp"

namespace hs
{
  namespace recon
  {
    namespace workflow
    {
      class HS_EXPORT PoissonSurface : public WorkflowStep
      {
      public:
        PoissonSurface();
      private:
        int RunPoissonSurface(WorkflowStepConfig* config);
        int CreateConfigXml(MeshSurfaceConfig* config);
      protected:
        virtual int RunImplement(WorkflowStepConfig* config);
      };

    }
  }
}



#endif //_HS_3D_RECONSTRUCTOR_WORKFLOW_POISSON_SURFACE_MODEL_HPP_