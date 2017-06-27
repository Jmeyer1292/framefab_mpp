//
// Created by yijiangh on 6/25/17.
//
#include <ros/ros.h>
#include <ros/console.h>

#include <framefab_path_post_processor/path_post_processor.h>

#include <rapidjson/document.h>
#include "rapidjson/filereadstream.h"


Eigen::Vector3d transformPoint(const Eigen::Vector3d pt, double scale, Eigen::Vector3d ref_transf)
{
  return (pt * scale + ref_transf);
}

framefab_path_post_planning::PathPostProcessor()
{
  unit_scale= 1;
  ref_pt_ = Eigen::Vector3d(0, 0, 0);
  transf_vec_ = Eigen::Vector3d(0, 0, 0);
}

void framefab_path_post_processing::PathPostProcessor::setParams(
    framefab_msgs::ModelInputParameters model_params,
    framefab_msgs::PathInputParameters path_params)
{
  model_input_params_ = model_params;
  path_input_params_ = path_params;

  // set unit scale
  switch (model_input_params_.unit_type)
  {
    case framefab_msgs::ModelInputParameters::MILLIMETER:
    {
      unit_scale_ = 0.001;
    }
    case framefab_msgs::ModelInputParameters::CENTIMETER:
    {
      unit_scale_ = 0.01;
    }
    case framefab_msgs::ModelInputParameters::INCH:
    {
      unit_scale_ = 0.0254;
    }
    case framefab_msgs::ModelInputParameters::FOOT:
    {
      unit_scale_ = 0.3048;
    }
    default:
    {
      ROS_ERROR("Unrecognized Unit type in Model Input Parameters!");
    }
  }

  // set ref point
  ref_pt = Eigen::Vector3d(model_input_params_.ref_pt_x, model_input_params_.ref_pt_y, model_input_params_.ref_pt_z);
}

bool framefab_path_post_processing::PathPostProcessor::createCandidatePoses()
{
  using namespace rapidjson;

  /* --- 1. Parse the input JSON file into a document --- */
  std::string fpath = path_input_params_.file_path;

  FILE* fp = fopen(fpath.c_str(), "r");

  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));

  Document document;

  if(document.ParseStream(is).HasParseError())
  {
    ROS_ERROR_STREAM("PathPostProcessor has ERROR parsing the input json file!");
    return false;
  }

  fclose(fp);
  ROS_INFO_STREAM("path json Parsing succeeded.");

  int m = document["element_number"].GetInt();
  ROS_INFO_STREAM("model element member: " << m);

  const Value& bcp = document["base_center_pt"];
  Eigen::Vector3d base_center_pt(bcp[0].GetDouble(), bcp[1].GetDouble(), bcp[2].GetDouble());

  setTransfVec(ref_pt_, base_center_pt, unit_scale_);

  const Value& process_path_array = document["sequenced_elements"];
  assert(process_path_array.IsArray());

  path_array_.clear();

  for (SizeType i = 0; i < process_path_array.Size(); i++)
  {
    const Value& element_path = process_path_array[i];
    Eigen::Vector3d st_pt(element_path["start_pt"][0].GetDouble(),
                          element_path["start_pt"][1].GetDouble(),
                          element_path["start_pt"][2].GetDouble());

    Eigen::Vector3d end_pt(element_path["end_pt"][0].GetDouble(),
                           element_path["end_pt"][1].GetDouble(),
                           element_path["end_pt"][2].GetDouble());

    // TODO: fetch the feasible orients
    std::vector<Eigen::Vector3d> feasible_orients;


    // create UnitProcessPath & Add UnitProcessPath into ProcessPath
    path_array_.push_back(createScaledUnitProcessPath(i, st_pt, end_pt, feasible_orients, type_str, 0.01));
  }

  return true;
}

framefab_utils::UnitProcessPath framefab_path_post_processing::PathPostProcessor::createScaledUnitProcessPath(
    int index, Eigen::Vector3d st_pt, Eigen::Vector3d end_pt,
    std::vector<Eigen::Vector3d> feasible_orients,
    std::string type_str,
    double shrink_length)
{
  framefab_utils::UnitProcessPath upp(
      index,
      transformPoint(st_pt, unit_scale_, transf_vec_),
      transformPoint(end_pt, unit_scale_, transf_vec_),
      feasible_orients, type_str, shrink_length);

  return
}