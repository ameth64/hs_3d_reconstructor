#ifndef _HS_3D_RECONSTRUCTOR_WORKFLOW_COMMON_WORKFLOW_STEP_HPP_
#define _HS_3D_RECONSTRUCTOR_WORKFLOW_COMMON_WORKFLOW_STEP_HPP_

#include <thread>
#include <memory>

#include "hs_progress/progress_utility/progress_manager.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

enum WorkflowStepType
{
  STEP_NONE = 0,
  STEP_FEATURE_MATCH,
  STEP_PHOTO_ORIENTATION,
  STEP_POINT_CLOUD,
  STEP_SURFACE_MODEL,
  STEP_TEXTURE
};

class HS_EXPORT WorkflowStepConfig
{
public:
  int type ();

protected:
  int type_;
};

typedef std::shared_ptr<WorkflowStepConfig> WorkflowStepConfigPtr;

class HS_EXPORT WorkflowStep
{
public:
  enum WorkflowStepState
  {
    STATE_READY = 0,
    STATE_WORKING,
    STATE_ERROR,
    STATE_FINISHED
  };
public:
  WorkflowStep();
  virtual ~WorkflowStep();
  int Start(WorkflowStepConfig* config);
  int Stop();
  float GetCompleteRatio();
  int type() const;
  int state() const;

protected:
  int Run(WorkflowStepConfig* config);
  virtual int RunImplement(WorkflowStepConfig* config) = 0;

protected:
  int type_;
  int state_;
  hs::progress::ProgressManager progress_manager_;
  std::thread working_thread_;
};

}
}
}

#endif
