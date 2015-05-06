#include <iostream>

#if WIN32
#include <Windows.h>
#endif

#include "workflow/point_cloud/pmvs_point_cloud.hpp"

#include "hs_flowmodule/point_cloud/define/pc_define.hpp"
#include "hs_flowmodule/point_cloud/agent/pc_agent.hpp"
#include "hs_math/geometry/rotation.hpp"
#include "workflow/point_cloud/pmvs_point_cloud_generate_xml.hpp"

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

std::string PointCloudConfig::workspace_path() const
{
  return workspace_path_;
}
std::string PointCloudConfig::photo_orientation_path() const
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

int PointCloud::ExportPointCloudInputXML(WorkflowStepConfig* config)
  {
//   typedef hs::math::geometry::Rotation3D<Scalar>  Rotation3D;
//   typedef Rotation3D::Matrix33 Matrix33;
//   typedef Rotation3D::Vector3 Vector3;

  PointCloudConfig* point_cloud_config =
  static_cast<PointCloudConfig*>(config);

  const std::string& workspace_path =
  point_cloud_config->workspace_path();

//   std::string intrinsic_file = workspace_path
// 
//   std::string point_cloud_input_path = workspace_path + "point_cloud_input.xml";
// 
//   boost::property_tree::ptree root; 
//   root.put("module_list.module_item.module_name","point_cloud");
//   root.put("module_list.module_item.module_version","1.0");
//   //share_section
//   root.put("module_list.module_item.share_section.workspace",workspace_path);
//   root.put("module_list.module_item.share_section.parallel_number",
//   point_cloud_config->number_of_threads());
//   root.put("module_list.module_item.share_section.pyramid_level",3);
//   root.put("module_list.module_item.share_section.patch_density",2);
//   root.put("module_list.module_item.share_section.patch_range",2);
//   //share_section.photo_list
// 
//   const std::vector<std::string>& image_paths = point_cloud_config->image_paths();
//   const std::vector<int>& image_ids = point_cloud_config->image_ids();
//   size_t num_photo = image_paths.size();
//   std::string str;
//   for (size_t i = 0; i < num_photo; ++i)
//   {
//   str.clear();
//   boost::property_tree::ptree photo_item; 
//   photo_item.put("id",image_ids.at(i));
//   photo_item.put("file",image_paths.at(i));
// 
//   //intrinsic_matrix
//   const IntrinsicParams& intrinsic_param = intrinsic_params_set[0];
//   str = std::to_string(intrinsic_param.focal_length()) + "\t" +
//     std::to_string(intrinsic_param.skew()) +
//     std::to_string(intrinsic_param.principal_point_x());
//   photo_item.put("camera_parameters.intrinsic_matrix.row", str);
//   str = "0\t" +
//     std::to_string(intrinsic_param.focal_length() * 
//     intrinsic_param.pixel_ratio()) + "\t" +
//     std::to_string(intrinsic_param.principal_point_y());
//   photo_item.add("camera_parameters.intrinsic_matrix.row", str);
//   str = "0\t0\t1";
//   photo_item.add("camera_parameters.intrinsic_matrix.row", str);
//   //radial_distortion
//   str = std::to_string(intrinsic_param.k1()) + "\t" +
//     std::to_string(intrinsic_param.k2()) + "\t" +
//     std::to_string(intrinsic_param.k3());
//   photo_item.put("camera_parameters.distortion_params.radial_distortion", str);
//   //decentering_distortion
//   str = std::to_string(intrinsic_param.d1()) + "\t" +
//     std::to_string(intrinsic_param.d2());
//   photo_item.put("camera_parameters.distortion_params.decentering_distortion", str);
// 
//   //rotation_matrix
//   const ExtrinsicParams& extrinsic_params =
//     extrinsic_params_set[i];
//   const ExtrinsicParams::Position& position = extrinsic_params.position();
//   const ExtrinsicParams::Rotation& rotation = extrinsic_params.rotation();
//   Matrix33 rmat(rotation);
// 
//   str = std::to_string(rmat(0,0)) + "\t" +
//     std::to_string(rmat(0,1)) + "\t" +
//     std::to_string(rmat(0,2));
//   photo_item.put("camera_parameters.rotation_matrix.row", str);
//   str = std::to_string(rmat(1,0)) + "\t" +
//     std::to_string(rmat(1,1)) + "\t" +
//     std::to_string(rmat(1,2));
//   photo_item.add("camera_parameters.rotation_matrix.row", str);
//   str = std::to_string(rmat(2,0)) + "\t" +
//     std::to_string(rmat(2,1)) + "\t" +
//     std::to_string(rmat(2,2));
//   photo_item.add("camera_parameters.rotation_matrix.row", str);
//   //optical_center
//   str = std::to_string(position[0]) + "\t" +
//     std::to_string(position[1]) + "\t" +
//     std::to_string(position[2]);
//   photo_item.put("camera_parameters.optical_center", str);
// 
//   root.add_child("module_list.module_item.share_section.photo_list.photo_item",photo_item);
//   }
//   //cluster_section
//   root.put("module_list.module_item.cluster_section.accuracy_threshold",0.7);
//   root.put("module_list.module_item.cluster_section.coverage_threshold",0.7);
//   root.put("module_list.module_item.cluster_section.cluster_size",50);
//   std::string sparse_cloud_path = workspace_path + "sparse_point_cloud.txt";
//   root.put("module_list.module_item.cluster_section.sparse_cloud",sparse_cloud_path);
//   //pmvs_section
//   root.put("module_list.module_item.pmvs_section.visibility_threshold",2);
//   root.put("module_list.module_item.pmvs_section.consistency_threshold",0.7);
//   root.put("module_list.module_item.pmvs_section.group_threshold",0.001);
//   //merge_section
//   root.put("module_list.module_item.merge_section.quality_threshold",1);
//   root.put("module_list.module_item.merge_section.visibility_threshold",2);
// 
//   boost::property_tree::xml_parser::xml_writer_settings<char> settings(' ', 2);
//   write_xml(point_cloud_input_path,root,std::locale(),settings);

  return 0;
  }

int PointCloud::RunImplement(WorkflowStepConfig* config)
{
  PointCloudConfig* point_cloud_config =
    static_cast<PointCloudConfig*>(config);

  std::cout << "Runing...PointCloud\n";

//   bm::CBaseAgent *agent = pc::CPCAgent::create();
//   agent->init();
//   agent->input(point_cloud_config->get_point_cloud_input_xml_path().c_str());
//   agent->start(bm::CBaseDefine::E_MX_ASYNC);
// 
//   while(agent->getState() == bm::CBaseDefine::E_MS_BUSY)
//   {
// #if WIN32
//     Sleep(1000);
// #endif
//   }
// 
//   agent->stop();
//   agent->output(point_cloud_config->get_point_cloud_output_xml_path().c_str());
//   agent->uninit();
//   pc::CPCAgent::destroy((pc::CPCAgent*)agent);


  return 0;
}


}//namespace workflow
}
}
