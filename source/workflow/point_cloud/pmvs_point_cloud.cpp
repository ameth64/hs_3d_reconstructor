#include <iostream>
#include <fstream>

#if WIN32
#include <Windows.h>
#endif

#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp> 

#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/archives/portable_binary.hpp>

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
void PointCloudConfig::set_intrinsic_path(
  const std::string& intrinsic_path)
{
  intrinsic_path_ = intrinsic_path;
}
void PointCloudConfig::set_extrinsic_path(
  const std::string& extrinsic_path)
{
  extrinsic_path_ = extrinsic_path;
}
void PointCloudConfig::set_sparse_point_cloud_path(
  const std::string& sparse_point_cloud_path)
{
  sparse_point_cloud_path_ = sparse_point_cloud_path;
}
void PointCloudConfig::set_intermediate_path(
  const std::string& intermediate_path)
{
  intermediate_path_ = intermediate_path;
}void PointCloudConfig::set_s_number_of_threads(int s_number_of_threads)
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

const std::string& PointCloudConfig::photo_orientation_path() const
{
  return photo_orientation_path_;
}
const std::string& PointCloudConfig::workspace_path() const
{
  return workspace_path_;
}
const std::string& PointCloudConfig::intrinsic_path() const
{
  return intrinsic_path_;
}
const std::string& PointCloudConfig::extrinsic_path() const
{
  return extrinsic_path_;
}
const std::string& PointCloudConfig::sparse_point_cloud_path() const
{
  return sparse_point_cloud_path_;
}
const std::string& PointCloudConfig::intermediate_path() const
{
  return intermediate_path_;
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
  return c_accuracy_threshold_;
}
float PointCloudConfig::c_coverage_threshold() const
{
  return c_coverage_threshold_;
}
int PointCloudConfig::c_cluster_size() const
{
  return c_cluster_size_;
}
float PointCloudConfig::p_consistency_threshold() const
{
  return p_consistency_threshold_;
}
float PointCloudConfig::p_group_threshold() const
{
  return p_group_threshold_;
}
int PointCloudConfig::p_visibility_threshold() const
{
  return p_visibility_threshold_;
}
int PointCloudConfig::m_quality_threshold() const
{
  return m_quality_threshold_;
}
int PointCloudConfig::m_visibility_threshold() const
{
  return m_visibility_threshold_;
}

std::map<int, std::string>& PointCloudConfig::photo_paths()
{
  return photo_paths_;
}
const std::map<int, std::string>& PointCloudConfig::photo_paths() const
{
  return photo_paths_;
}

PointCloud::PointCloud()
{
  type_ = STEP_POINT_CLOUD;
}

int PointCloud::CreateConfigXml(
  PointCloudConfig* config)
{
  const std::string intrinsic_path = config->intrinsic_path();
  const std::string extrinsic_path = config->extrinsic_path();

  //读取内参数文件
  IntrinsicParamsMap intrinsic_params_map;
  ReadIntrinsicFile(intrinsic_path, intrinsic_params_map);

  //读取外参数文件
  ExtrinsicParamsMap extrinsic_params_map;
  ReadExtrinsicFile(extrinsic_path, extrinsic_params_map);

  boost::property_tree::ptree root;
  root.put("module_list.module_item.module_name", "point_cloud");
  root.put("module_list.module_item.module_version", "1.0");
  //share_section
  root.put("module_list.module_item.share_section.workspace", 
    config->workspace_path());
  root.put("module_list.module_item.share_section.intermediate",
    config->intermediate_path());
  root.put("module_list.module_item.share_section.parallel_number",
    config->s_number_of_threads());
  root.put("module_list.module_item.share_section.pyramid_level",
    config->s_pyramid_level());
  root.put("module_list.module_item.share_section.patch_density",
    config->s_patch_density());
  root.put("module_list.module_item.share_section.patch_range",
    config->s_patch_range());
  //share_section.photo_list

  std::string str;
  for (auto itr_photo_id = extrinsic_params_map.begin();
    itr_photo_id != extrinsic_params_map.end(); ++itr_photo_id)
  {
    str.clear();
    boost::property_tree::ptree photo_item;
    photo_item.put("id", itr_photo_id->first.first);
    auto itr_photo_paht = config->photo_paths().find(
      int(itr_photo_id->first.first));
    if (itr_photo_paht != config->photo_paths().end())
    {
      photo_item.put("file", itr_photo_paht->second);
    }
    else
    {
      std::cout << "读取图片数据失败!\n";
      return -1;
    }
    //intrinsic_matrix
    IntrinsicParams intrinsic_param;
    auto itr_intrinsic_params =
      intrinsic_params_map.find(itr_photo_id->first.second);
    if (itr_intrinsic_params != intrinsic_params_map.end())
    {
      intrinsic_param = itr_intrinsic_params->second;
    }
    else
    {
      std::cout << "读取内参数失败!\n";
      return -1;
    }

    str = std::to_string(intrinsic_param.focal_length()) + " " +
      std::to_string(intrinsic_param.skew()) + " " +
      std::to_string(intrinsic_param.principal_point_x());
    photo_item.put("camera_parameters.intrinsic_matrix.row", str);
    str = "0 " +
      std::to_string(intrinsic_param.focal_length() *
      intrinsic_param.pixel_ratio()) + " " +
      std::to_string(intrinsic_param.principal_point_y());
    photo_item.add("camera_parameters.intrinsic_matrix.row", str);
    str = "0 0 1";
    photo_item.add("camera_parameters.intrinsic_matrix.row", str);
    //radial_distortion
    str = std::to_string(intrinsic_param.k1()) + " " +
      std::to_string(intrinsic_param.k2()) + " " +
      std::to_string(intrinsic_param.k3());
    photo_item.put("camera_parameters.distortion_params.radial_distortion", str);
    //decentering_distortion
    str = std::to_string(intrinsic_param.d1()) + " " +
      std::to_string(intrinsic_param.d2());
    photo_item.put("camera_parameters.distortion_params.decentering_distortion", str);

   //rotation_matrix
    Matrix33 rmat(itr_photo_id->second.rotation());
    Position position = itr_photo_id->second.position();

    str = std::to_string(rmat(0, 0)) + " " +
      std::to_string(rmat(0, 1)) + " " +
      std::to_string(rmat(0, 2));
    photo_item.put("camera_parameters.rotation_matrix.row", str);
    str = std::to_string(rmat(1, 0)) + " " +
      std::to_string(rmat(1, 1)) + " " +
      std::to_string(rmat(1, 2));
    photo_item.add("camera_parameters.rotation_matrix.row", str);
    str = std::to_string(rmat(2, 0)) + " " +
      std::to_string(rmat(2, 1)) + " " +
      std::to_string(rmat(2, 2));
    photo_item.add("camera_parameters.rotation_matrix.row", str);
    //optical_center
    str = std::to_string(position[0]) + " " +
      std::to_string(position[1]) + " " +
      std::to_string(position[2]);
    photo_item.put("camera_parameters.optical_center", str);

     root.add_child("module_list.module_item.share_section.photo_list.photo_item", photo_item);
   }
  //cluster_section
  root.put("module_list.module_item.cluster_section.accuracy_threshold",
    config->c_accuracy_threshold());
  root.put("module_list.module_item.cluster_section.coverage_threshold",
    config->c_coverage_threshold());
  root.put("module_list.module_item.cluster_section.cluster_size",
    config->c_cluster_size());
  std::string sparse_cloud_path =
    config->sparse_point_cloud_path();
  root.put("module_list.module_item.cluster_section.sparse_cloud",
    sparse_cloud_path);
  //pmvs_section
  root.put("module_list.module_item.pmvs_section.visibility_threshold",
    config->p_visibility_threshold());
  root.put("module_list.module_item.pmvs_section.consistency_threshold",
    config->p_consistency_threshold());
  root.put("module_list.module_item.pmvs_section.group_threshold",
    config->p_group_threshold());
  //merge_section
  root.put("module_list.module_item.merge_section.quality_threshold",
    config->m_quality_threshold());
  root.put("module_list.module_item.merge_section.visibility_threshold",
    config->m_visibility_threshold());

  boost::property_tree::xml_parser::xml_writer_settings<boost::property_tree::ptree::key_type> settings(' ', 2);
  //boost::property_tree::xml_parser::xml_writer_settings<char> settings(' ', 2);
  write_xml(config->intermediate_path()+"point_cloud_input.xml", root, std::locale(), settings);
  return 0;
}

//读取内参数文件
int PointCloud::ReadIntrinsicFile(
  const std::string& file_path, IntrinsicParamsMap& ipm)
{
  std::ifstream intrinsic_file(file_path, std::ios::binary);
  if (!intrinsic_file) return -1;
  cereal::PortableBinaryInputArchive archive(intrinsic_file);
  archive(ipm);
  return 0;
}
//读取外参数文件
int PointCloud::ReadExtrinsicFile(
  const std::string& file_path, ExtrinsicParamsMap& epm)
{
  std::ifstream extrinsic_file(file_path, std::ios::binary);
  if (!extrinsic_file) return -1;
  cereal::PortableBinaryInputArchive archive(extrinsic_file);
  archive(epm);
  return 0;
}

int PointCloud::RunImplement(WorkflowStepConfig* config)
{
  PointCloudConfig* point_cloud_config =
    static_cast<PointCloudConfig*>(config);
  if (CreateConfigXml(point_cloud_config) != 0)
  {
    return -1;
  }

  std::string point_cloud_xml =
    point_cloud_config->intermediate_path() + "point_cloud_input.xml";
  std::string point_cloud_xml_out = 
    point_cloud_config->intermediate_path() + "point_cloud_output.xml";

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
