//
// Created by yijiangh on 7/5/17.
//

#ifndef FRAMEFAB_PROCESS_PLANNING_PATH_TRANSITIONS_H
#define FRAMEFAB_PROCESS_PLANNING_PATH_TRANSITIONS_H

//#include "common_utils.h"
//#include <framefab_msgs/BlendingPlanParameters.h>
#include "eigen_conversions/eigen_msg.h"

namespace framefab_process_planning
{

struct ConnectingPath
{
  EigenSTL::vector_Affine3d depart;
  EigenSTL::vector_Affine3d approach;
};

struct TransitionParameters
{
  double linear_disc;
  double angular_disc;
  double retract_dist;
};

std::vector<ConnectingPath> generateTransitions(const std::vector<geometry_msgs::PoseArray>& segments,
                                                const TransitionParameters& params);

framefab_process_planning::DescartesTraj
toDescartesTraj(const std::vector<geometry_msgs::PoseArray>& segments,
                const double process_speed, const TransitionParameters& transition_params,
                boost::function<descartes_core::TrajectoryPtPtr(const Eigen::Affine3d&, const double)> conversion_fn);

}

#endif //FRAMEFAB_PROCESS_PLANNING_PATH_TRANSITIONS_H
