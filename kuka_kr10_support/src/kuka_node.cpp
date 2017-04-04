#include <moveit/move_group_interface/move_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>
#include <geometry_msgs/PoseArray.h>
#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <cmath>

static ros::Publisher display_publisher;
static ros::Publisher marker_pub;
static visualization_msgs::Marker link_list;
static ros::Rate * r;
static float f;

void frameCallback(geometry_msgs::PoseArray msg){
  ROS_INFO("frame subscriber callback");
  std::cout << msg << std::endl;
  link_list.color.b = 1.0;
  link_list.color.a = 1.0;
  link_list.scale.x = 1.0;
  link_list.scale.y = 1.0;
  link_list.scale.z = 1.0; 
  link_list.header.stamp = ros::Time::now();
  
  geometry_msgs::Point p;
  p.x = 1.0;
  p.y = 1.0;
  p.z = 0.0;
  link_list.points.push_back(p);
  link_list.points.push_back(p);  
  marker_pub.publish(link_list); 
  r->sleep();
}    
  
 
int main(int argc, char **argv)
{
  ros::init(argc, argv, "kuka_node");
  ros::NodeHandle node_handle;  
  ros::AsyncSpinner spinner(1);
  spinner.start();
  ros::Subscriber frame_sub = node_handle.subscribe("framelinks", 0 , &frameCallback);
  link_list.header.frame_id = "world";
  link_list.action = visualization_msgs::Marker::ADD;
  link_list.pose.orientation.w = 1.0;
  link_list.id = 0; 
  link_list.type = visualization_msgs::Marker::LINE_LIST;
  marker_pub = node_handle.advertise<visualization_msgs::Marker>("visualization_marker",0);
  /* This sleep is ONLY to allow Rviz to come up */
  // We will use the :planning_scene_interface:`PlanningSceneInterface`
  // class to deal directly with the world.
  r = new ros::Rate(20.0);
  r->sleep();
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;  

  // Publisher for visualizing plans in Rviz.
  display_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);

  while(ros::ok()) {
    ros::spinOnce();
  } 
  ros::shutdown();
  return 0;
}
