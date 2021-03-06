//
// Created by yijiangh on 7/5/17.
//

#ifndef FRAMEFAB_PROCESS_PLANNING_PATH_TRANSITIONS_H
#define FRAMEFAB_PROCESS_PLANNING_PATH_TRANSITIONS_H

#include "common_utils.h"

#include <Eigen/Geometry>

// msgs
//#include <framefab_msgs/BlendingPlanParameters.h>
#include <framefab_msgs/ElementCandidatePoses.h>
#include "eigen_conversions/eigen_msg.h"

#include <descartes_planner/graph_builder.h>

namespace framefab_process_planning
{


struct ConstrainedSegParameters
{
  double linear_vel;
  double linear_disc;
  double angular_disc;
  double retract_dist;
};

std::vector<descartes_planner::ConstrainedSegment>
toDescartesConstrainedPath(
    const std::vector<framefab_msgs::ElementCandidatePoses>& process_path,
    const int selected_path_id,
    const double process_speed, const ConstrainedSegParameters& transition_params);

}

#endif //FRAMEFAB_PROCESS_PLANNING_PATH_TRANSITIONS_H