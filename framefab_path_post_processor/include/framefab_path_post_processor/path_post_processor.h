//
// Created by yijiangh on 6/25/17.
//

#ifndef FRAMEFAB_PATH_POST_PROCESSOR_PATH_POST_PROCESSOR_H
#define FRAMEFAB_PATH_POST_PROCESSOR_PATH_POST_PROCESSOR_H

#include <framefab_msgs/ModelInputParameters.h>
#include <framefab_msgs/PathInputParameters.h>
#include <framefab_msgs/ElementCandidatePoses.h>

#include <framefab_path_post_processor/process_path.h>

namespace framefab_path_post_processing
{

class PathPostProcessor
{
  typedef std::vector<framefab_msgs::ElementCandidatePoses> ElementCandidatePosesArray;

 public:
  PathPostProcessor();
  virtual ~PathPostProcessor() {}

  bool createCandidatePoses();
  const ElementCandidatePosesArray& getCandidatePoses() const { return path_array_; }

  // data setting
  void setParams(framefab_msgs::ModelInputParameters model_params,
                 framefab_msgs::PathInputParameters path_params);

  // getter
  double getUnitScale() { return unit_scale_; }
  Eigen::Vector3d getTransfVec() { return transf_vec_; }

  framefab_utils::UnitProcessPath createScaledUnitProcessPath(int index,
                                                              Eigen::Vector3d st_pt, Eigen::Vector3d end_pt,
                                                              std::vector<Eigen::Vector3d> feasible_orients,
                                                              std::string type_str,
                                                              double shrink_length = 0.01);

 protected:
  void setTransfVec(Eigen::Vector3d& const ref_pt, Eigen::Vector3d& const base_center_pt, double& const scale)
  {
    transf_vec_ = (ref_pt - base_center_pt) * scale;
  }

  // add printing table
  // add printing table ref pt

 private:
  // params
  framefab_msgs::ModelInputParameters model_input_params_;
  framefab_msgs::PathInputParameters path_input_params_;

  std::vector<framefab_utils::UnitProcessPath> path_array_;

  double unit_scale_;
  Eigen::Vector3d ref_pt_;
  Eigen::Vector3d transf_vec_;
};
}
#endif //FRAMEFAB_PATH_POST_PROCESSOR_PATH_POST_PROCESSOR_H
