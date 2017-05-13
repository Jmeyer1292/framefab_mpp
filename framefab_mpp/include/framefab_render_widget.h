//
// Created by yijiangh on 4/13/17.
//

#ifndef FRAMEFABRENDERWIDGET_H
#define FRAMEFABRENDERWIDGET_H

// Qt
#include <QObject>
#include <QWidget>
#include <QSlider>
#include <QString>

// ROS
#include <ros/ros.h>
#include <std_msgs/ColorRGBA.h>

// MoveIt
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene_monitor/planning_scene_monitor.h>
#include <moveit/move_group_interface/move_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/CollisionObject.h>

// framefab - wireframe
#include <wire_frame/Vec.h>
#include <wire_frame/wire_frame_line_graph.h>
#include <wire_frame/wire_frame_collision_objects.h>

#include <framefab_planner.h>

namespace framefab
{

// TODO: this class should be an interface class, share the function of
// scene generation, but keep independent of specific visualizer platform
// (Qt in Rviz for now)

//! @class FrameFabRenderWidget
/*!
 * @brief input model, generate geometry for visualization & computation class
 */
class FrameFabRenderWidget : public QWidget
{
 Q_OBJECT
 public:
  /*!
   * @brief constructor
   * @param[in] ros node_handle
   */
  FrameFabRenderWidget( QWidget* parent = 0 );

  /*!
   * @brief destructor
   */
  ~FrameFabRenderWidget();

  /*!
   * Reads and verifies the ROS parameters.
   * @return true if successful
   */
  bool readParameters();

 Q_SIGNALS:
 void sendLogInfo(QString);

 public Q_SLOTS:

  /*!
   * @brief (Qt slot function) Read the file name from the dialog and parse results
   */
  void readFile();

  // TODO: this should not be Poses (just collision object visualization)
  // TODO: the orientation for robot should leave to be determined later
  /*!
   * @brief (Qt slot function) publish ros message "draw links"
   */
  void displayPoses();

  /**
   * @brief (Qt slot function) Advances robot one step along current trajectory
   *
   * Current a descartes planning testbed.
   */
  void advanceRobot();

  /**
   *
   * @brief sets value of slider
   */
  void setValue(int i);

  void setScaleFactor(QString unit_info);
  void setRefPoint(double ref_pt_x, double ref_pt_y, double ref_pt_z);

  void constructCollisionObjects();

 private:
  //! Model constants
  float collision_cylinder_radius_;
  float unit_conversion_scale_factor_;

  double ref_pt_x_;
  double ref_pt_y_;
  double ref_pt_z_;

  //! Model rendering constants
  std_msgs::ColorRGBA start_color_;
  std_msgs::ColorRGBA end_color_;
  std_msgs::ColorRGBA cylinder_color_;

  //! Parent pointer for ui updates
  QWidget* parent_;

  //! ROS NodeHandle
  ros::NodeHandle node_handle_;

  //! ROS Rate to refresh Rviz
  ros::Rate*      rate_;

  //! MoveIt! interfaces
  std::string planning_group_name_;

  moveit::planning_interface::PlanningSceneInterfacePtr ptr_planning_scene_interface_;
  move_group_interface::MoveGroupPtr ptr_move_group_;

  //! ROS publisher
  ros::Publisher display_pose_publisher_;

  //! ROS topics
  std::string display_pose_topic_;

  //! FrameFab computation class
  FrameFabPlannerPtr ptr_framefab_planner_;

  //! wireframe data structure
  wire_frame::WireFrameCollisionObjectsPtr ptr_wire_frame_collision_objects_;
};
}/* namespace */

#endif //FRAMEFABRENDERWIDGET_H