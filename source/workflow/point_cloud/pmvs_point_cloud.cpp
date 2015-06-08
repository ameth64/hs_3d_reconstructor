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
void PointCloudConfig::set_s_number_of_threads(int s_number_of_threads)
{
  s_number_of_threads_ = s_number_of_threads;
}
void PointCloudConfig::set_s_pyramid_level(int s_pyramid_level)
{
  s_pyramid_level_ = s_pyramid_level; 
}
void PointCloudConfig::set_s_patch_density(int s_patch_density)
{
	s_patch_density_ = s_patch_density;
}
void PointCloudConfig::set_s_patch_range(int s_patch_range)
{
  s_patch_range_ = s_patch_range;
}
void PointCloudConfig::set_c_accuracy_threshold(float c_accuracy_threshold)
{
  c_accuracy_threshold_ = c_accuracy_threshold;
}
void PointCloudConfig::set_c_coverage_threshold(float c_coverage_threshold)
{
  c_coverage_threshold_ = c_coverage_threshold;
}
void PointCloudConfig::set_c_cluster_size(int c_cluster_size)
{
  c_cluster_size_ = c_cluster_size;
}
void PointCloudConfig::set_p_visibility_threshold(int p_visibility_threshold)
{
  p_visibility_threshold_ = p_visibility_threshold;
}
void PointCloudConfig::set_p_consistency_threshold(float p_consistency_threshold)
{
  p_consistency_threshold_ = p_consistency_threshold;
}
void PointCloudConfig::set_p_group_threshold(float p_group_threshold)
{
  p_group_threshold_ = p_group_threshold;
}
void PointCloudConfig::set_m_quality_threshold(int m_quality_threshold)
{
  m_quality_threshold_ = m_quality_threshold;
}
void PointCloudConfig::set_m_visibility_threshold(int m_visibility_threshold)
{
  m_visibility_threshold_ = m_visibility_threshold;
}

const std::string PointCloudConfig::workspace_path() const
{
  return workspace_path_;
}
const std::string PointCloudConfig::photo_orientation_path() const
{
  return photo_orientation_path_;
}
int PointCloudConfig::s_number_of_threads() const
{
  return s_number_of_threads_;
}
int PointCloudConfig::s_pyramid_level() const
{
  return s_pyramid_level_;
}
int PointCloudConfig::s_patch_density() const
{
  return s_patch_density_;
}
int PointCloudConfig::s_patch_range() const
{
  return s_patch_range_;
}
float PointCloudConfig::c_accuracy_threshold() const
{
  c_accuracy_threshold_;
}
float PointCloudConfig::c_coverage_threshold() const
{
  c_coverage_threshold_;
}
int PointCloudConfig::c_cluster_size() const
{
  c_cluster_size_;
}
float PointCloudConfig::p_consistency_threshold() const
{
  p_consistency_threshold_;
}
float PointCloudConfig::p_group_threshold() const
{
  p_group_threshold_;
}
int PointCloudConfig::p_visibility_threshold() const
{
  p_visibility_threshold_;
}
int PointCloudConfig::m_quality_threshold() const
{
  m_quality_threshold_;
}
int PointCloudConfig::m_visibility_threshold() const
{
  m_visibility_threshold_;
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
  root.put("module_list.module_item.share_section.pyramid_level",
           point_cloud_config->s_pyramid_level());
  root.put("module_list.module_item.share_section.patch_density",
           point_cloud_config->s_patch_density());
  root.put("module_list.module_item.share_section.patch_range",
           point_cloud_config->s_patch_range());
  root.put("module_list.module_item.cluster_section.accuracy_threshold",
           point_cloud_config->c_accuracy_threshold());
  root.put("module_list.module_item.cluster_section.coverage_threshold",
           point_cloud_config->c_coverage_threshold());
  root.put("module_list.module_item.cluster_section.cluster_size",
           point_cloud_config->c_cluster_size());
  root.put("module_list.module_item.pmvs_section.visibility_threshold",
           point_cloud_config->p_visibility_threshold());
  root.put("module_list.module_item.pmvs_section.consistency_threshold",
           point_cloud_config->p_consistency_threshold());
  root.put("module_list.module_item.pmvs_section.group_threshold",
           point_cloud_config->p_group_threshold());
  root.put("module_list.module_item.merge_section.quality_threshold",
           point_cloud_config->m_quality_threshold());
  root.put("module_list.module_item.merge_section.visibility_threshold",
           point_cloud_config->m_visibility_threshold());
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
#else
    sleep(1);
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
