//
// Created by yijiangh on 7/8/17.
//
#include <ros/console.h>

#include <Eigen/Geometry>

#include "generate_motion_plan.h"
#include "trajectory_utils.h"
#include "common_utils.h"
#include <descartes_trajectory/axial_symmetric_pt.h>
#include <descartes_trajectory/joint_trajectory_pt.h>

#include <descartes_planner/ladder_graph_dag_search.h>
#include <descartes_planner/dense_planner.h>

// msg
#include <geometry_msgs/Pose.h>

#include <eigen_conversions/eigen_msg.h>

const static bool validateTrajectory(const trajectory_msgs::JointTrajectory& pts,
                                     const descartes_core::RobotModel& model,
                                     const double min_segment_size)
{
  for (std::size_t i = 1; i < pts.points.size(); ++i)
  {
    const auto& pt_a = pts.points[i - 1].positions;
    const auto& pt_b = pts.points[i].positions;

    auto interpolate = framefab_process_planning::interpolateJoint(pt_a, pt_b, min_segment_size);

    // The thought here is that the graph building process already checks the waypoints in the
    // trajectory for collisions. What we want to do is check between these waypoints when they
    // move a lot. 'interpolateJoint()' returns a list of positions where the maximum joint motion
    // between them is no more 'than min_segment_size' and its inclusive. So if there are only two
    // solutions, then we just have the start & end which are already checked.
    if (interpolate.size() > 2)
    {
      for (std::size_t j = 1; j < interpolate.size() - 1; ++j)
      {
        if (!model.isValid(interpolate[j]))
        {
          return false;
        }
      }
    }
  }
  return true;
}

static framefab_process_planning::DescartesTraj generateUnitProcessMotionPlan(
    const descartes_core::RobotModelPtr model,
    const framefab_process_planning::DescartesTraj traj,
    const std::vector<double>& start_state,
    moveit::core::RobotModelConstPtr moveit_model,
    const std::string& move_group_name)
{
  using framefab_process_planning::DescartesTraj;
  using framefab_process_planning::freeSpaceCostFunction;

  DescartesTraj null_solution;

  // Generate a graph of the process path joint solutions
  descartes_planner::PlanningGraph planning_graph (model);
  if (!planning_graph.insertGraph(traj)) // builds the graph out
  {
    ROS_ERROR("%s: Failed to build graph. One or more points may have no valid IK solutions", __FUNCTION__);
    return null_solution;
  }

  // Using the valid starting configurations, let's compute an estimate
  // of the cost to move to these configurations from our starting pose
  const auto& graph = planning_graph.graph();
  const auto dof = graph.dof();

  std::vector<std::vector<double>> process_start_poses;
  const auto& joint_data = graph.getRung(0).data; // This is a flat vector of doubles w/ all the solutions
  const auto n_start_poses = joint_data.size() / dof;

  for (std::size_t i = 0; i < n_start_poses; ++i) // This builds a list of starting poses
  {
    std::vector<double> sol (&joint_data[i * dof], &joint_data[i*dof + dof]);
    process_start_poses.push_back(sol);
  }

  std::vector<double> process_start_costs (process_start_poses.size());
  for (std::size_t i = 0; i < process_start_costs.size(); ++i) // This computes the list of configurations
  {
    process_start_costs[i] = freeSpaceCostFunction(start_state, process_start_poses[i]);
  }

  // Now we perform the search using the starting costs from our estimation above
  descartes_planner::DAGSearch search (graph);
  double cost = search.run(process_start_costs);
  if (cost == std::numeric_limits<double>::max())
  {
    ROS_ERROR("%s: Failed to search graph. All points have IK, but process constraints (e.g velocity) "
                  "prevent a solution", __FUNCTION__);
    return null_solution;
  }

  // Here we search the graph for the shortest path and build a descartes trajectory of it!
  auto path_idxs = search.shortestPath();
  ROS_INFO("%s: Descartes computed path with cost %lf", __FUNCTION__, cost);
  DescartesTraj solution;
  for (size_t i = 0; i < path_idxs.size(); ++i)
  {
    const auto idx = path_idxs[i];
    const auto* data = graph.vertex(i, idx);
    const auto& tm = graph.getRung(i).timing;
    auto pt = descartes_core::TrajectoryPtPtr(new descartes_trajectory::JointTrajectoryPt(std::vector<double>(data, data + dof), tm));
    solution.push_back(pt);
  }

  ROS_INFO_STREAM("-----------------------------");
  ROS_INFO_STREAM("input traj size: " << traj.size());
  ROS_INFO_STREAM("solution size: " << solution.size());

  return solution;
}

bool framefab_process_planning::generateMotionPlan(
    const descartes_core::RobotModelPtr model,
    const std::vector<DescartesUnitProcess>& trajs,
    const std::vector<moveit_msgs::CollisionObject>& collision_objs,
    moveit::core::RobotModelConstPtr moveit_model,
    ros::ServiceClient& planning_scene_diff_client,
    const std::string& move_group_name,
    const std::vector<double>& start_state,
    std::vector<framefab_msgs::UnitProcessPlan>& plan)
{
  plan.resize(trajs.size());

  for(std::size_t i = 0; i < trajs.size(); i++)
  {
    // update collision objects
    addCollisionObject(planning_scene_diff_client, collision_objs[i]);

    // try descartes planning with connect + unit process
    DescartesTraj traj;
    traj.insert(traj.end(), trajs[i].connect_path.begin(), trajs[i].connect_path.end());
    traj.insert(traj.end(), trajs[i].unit_process_path.begin(), trajs[i].unit_process_path.end());

    DescartesTraj solution = generateUnitProcessMotionPlan(model, traj, start_state, moveit_model, move_group_name);

    // if no solution found, descartes planning with unit process
    // and get free plan for connect path

    // add generated traj into plan output
//    plan[i].trajectory_connection =

    // update last pose (joint)

  }

  return true;
}