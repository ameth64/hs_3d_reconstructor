#include <iostream>

#if WIN32
#include <Windows.h>
#endif

#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp> 

#include "workflow/point_cloud/pmvs_point_cloud.hpp"

#include "hs_flowmodule/point_cloud/define/pc_define.hpp"
#include "hs_flowmodule/point_cloud/agent/pc_agent.hpp"
#include "hs_math/geometry/rotation.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

PointCloudConfig::PointCloudConfig()
{
  type_ = STEP_POINT_CLOUD;
}

void PointCloudConfig::set_workspace_path(
  const std::string& workspace_path)
{
  workspace_path_ = workspace_path;
}
void PointCloudConfig::set_photo_orientation_path(
  const std::string& photo_orientation_path)
{
  photo_orientation_path_ = photo_orientation_path;
}
void PointCloudConfig::set_number_of_threads(int number_of_threads)
{
  number_of_threads_ = number_of_threads;
}

const std::string PointCloudConfig::workspace_path() const
{
  return workspace_path_;
}
const std::string PointCloudConfig::photo_orientation_path() const
{
  return photo_orientation_path_;
}
int PointCloudConfig::number_of_threads() const
{
  return number_of_threads_; 
}

PointCloud::PointCloud()
{
  type_ = STEP_POINT_CLOUD;
}

int PointCloud::RunImplement(WorkflowStepConfig* config)
{
  PointCloudConfig* point_cloud_config =
    static_cast<PointCloudConfig*>(config);
  std::string workspace_path = point_cloud_config->workspace_path();
  std::string photo_orientation_xml = 
    point_cloud_config->photo_orientation_path() + "point_cloud_input.xml";
  std::string point_cloud_xml = workspace_path + "point_cloud_input.xml";
  std::string point_cloud_xml_out = workspace_path + "point_cloud_output.xml";

  boost::property_tree::ptree root; 
  boost::property_tree::xml_parser::xml_writer_settings<boost::property_tree::ptree::key_type> settings(' ', 2);
  //boost::property_tree::xml_parser::xml_writer_settings<char> settings(' ', 2);
  read_xml(photo_orientation_xml,root,boost::property_tree::xml_parser::trim_whitespace);
  root.put("module_list.module_item.share_section.workspace",workspace_path);
  write_xml(point_cloud_xml, root, std::locale(), settings);

  bm::CBaseAgent *agent = pc::CPCAgent::create();
  agent->init();
  agent->input(point_cloud_xml.c_str());
  agent->start(bm::CBaseDefine::E_MX_ASYNC);

  int pc_progress = 0;
  while(agent->getState() == bm::CBaseDefine::E_MS_BUSY)
  {
#if WIN32
    Sleep(1000);
#endif
    pc_progress = agent->getProgress();
    progress_manager_.SetCurrentSubProgressCompleteRatio(
      float(pc_progress)/float(100));
  }

  agent->stop();
  agent->output(point_cloud_xml_out.c_str());
  agent->uninit();
  pc::CPCAgent::destroy((pc::CPCAgent*)agent);

  return 0;
}


}//namespace workflow
}
}
