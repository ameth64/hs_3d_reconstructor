#include "workflow_step.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

int WorkflowStepConfig::type()
{
  return type_;
}

WorkflowStep::WorkflowStep()
  : type_(STEP_NONE)
  , state_(STATE_READY)
  , result_code_(0)
{

}

WorkflowStep::~WorkflowStep()
{
  Stop();
}

int WorkflowStep::Start(WorkflowStepConfig* config)
{
  progress_manager_.StartWorking();
  working_thread_ = std::thread(&WorkflowStep::Run, this, config);
  return 0;
}

int WorkflowStep::Stop()
{
  progress_manager_.StopWorking();
  if (working_thread_.joinable())
  {
    working_thread_.join();
  }
  return 0;
}

float WorkflowStep::GetCompleteRatio()
{
  return progress_manager_.GetCompleteRatio();
}

int WorkflowStep::type() const
{
  return type_;
}

int WorkflowStep::state() const
{
  return state_;
}

int WorkflowStep::result_code() const
{
  return result_code_;
}

int WorkflowStep::Run(WorkflowStepConfig* config)
{
  state_ = STATE_WORKING;
  int result =  this->RunImplement(config);
  if (result != 0)
  {
    state_ = STATE_ERROR;
  }
  else
  {
    state_ = STATE_FINISHED;
  }
  return result;
}

}
}
}
