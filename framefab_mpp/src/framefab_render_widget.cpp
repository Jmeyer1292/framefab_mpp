//
// Created by yijiangh on 4/13/17.
//

#include <map>
#include <boost/shared_ptr.hpp>

// ROS msgs
#include <visualization_msgs/Marker.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/PoseArray.h>

//MoveIt
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene_monitor/planning_scene_monitor.h>

#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <tf_conversions/tf_eigen.h>
#include <eigen_conversions/eigen_msg.h>

// Qt
#include <QtCore>
#include <QFileDialog>
#include <QString>

// framefab
#include <framefab_rviz_panel.h>
#include <framefab_render_widget.h>

#include <framefab_msgs/AdvanceRobot.h>

namespace framefab
{

FrameFabRenderWidget::FrameFabRenderWidget( QWidget* parent )
    : parent_(parent),
      unit_conversion_scale_factor_(1),
      ref_pt_x_(0), ref_pt_y_(0), ref_pt_z_(0)
{
  ROS_INFO_NAMED("framefab_mpp", "[ff_render_widget] FrameFabPlanner Render Widget started.");

  node_handle_ = ros::NodeHandle("framefab_render_widget");
  rate_ = new ros::Rate(10.0);

  // readParameters
  readParameters();

  // init collision objects
  ROS_INFO_NAMED("framefab_mpp", "[ff_render_widget] init ptr_wf_collision");
  ptr_wire_frame_collision_objects_ = boost::make_shared<wire_frame::WireFrameCollisionObjects>();

  ptr_planning_scene_interface_ = boost::make_shared<moveit::planning_interface::PlanningSceneInterface>();

  ptr_move_group_ = boost::make_shared<move_group_interface::MoveGroup>("manipulator");

  adv_robot_srv_client_ = node_handle_.serviceClient<framefab_msgs::AdvanceRobot>(
      "/framefab_mpp_node/advance_robot");
}

FrameFabRenderWidget::~FrameFabRenderWidget()
{
}

bool FrameFabRenderWidget::readParameters()
{
  // render widget topic name parameters
  node_handle_.param("display_pose_topic", display_pose_topic_, std::string("/framelinks"));

  // rviz render parameters
  node_handle_.param("/framefab_mpp/wire_frame_collision_start_vertex_color_r", start_color_.r, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_start_vertex_color_g", start_color_.g, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_start_vertex_color_b", start_color_.b, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_start_vertex_color_a", start_color_.a, float(0.0));

  node_handle_.param("/framefab_mpp/wire_frame_collision_end_vertex_color_r", end_color_.r, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_end_vertex_color_g", end_color_.g, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_end_vertex_color_b", end_color_.b, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_end_vertex_color_a", end_color_.a, float(0.0));

  node_handle_.param("/framefab_mpp/wire_frame_collision_cylinder_color_r", cylinder_color_.r, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_cylinder_color_g", cylinder_color_.g, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_cylinder_color_b", cylinder_color_.b, float(0.0));
  node_handle_.param("/framefab_mpp/wire_frame_collision_cylinder_color_a", cylinder_color_.a, float(0.0));

  node_handle_.param("/framefab_mpp/wire_frame_collision_cylinder_radius", collision_cylinder_radius_, float(0.0001));
  return true;
}

void FrameFabRenderWidget::advanceRobot()
{
  // init main computation class - FrameFabPlanner here
  ROS_INFO_NAMED("framefab_mpp", "[FF_RenderWidget] advance robot called");

//  ptr_framefab_planner_ = boost::make_shared<FrameFabPlanner>(node_handle_);
//
//  ptr_framefab_planner_->setRobotHomePose();

//  moveit::planning_interface::MoveGroup::Plan my_plan;
//  const std::map<std::string, double> home_state = ptr_move_group_->getNamedTargetValues("home_pose");
//
//  std::vector<double> group_variable_values;
//  ptr_move_group_->getCurrentState()->copyJointGroupPositions(
//      ptr_move_group_->getCurrentState()->getRobotModel()->getJointModelGroup(ptr_move_group_->getName()),
//      group_variable_values);
//
//  for(std::map<std::string, double>::const_iterator it = home_state.begin(); it!=home_state.end(); ++it)
//  {
//    ROS_INFO("[ff_render_widget] home_pose: %s, %f", it->first.c_str(), it->second);
//  }

  framefab_msgs::AdvanceRobot adv_robot_srv;

  adv_robot_srv.request.is_advance = true;

  if(!adv_robot_srv_client_)
  {
    ROS_ERROR("[FF_RenderWidget] service connection FAILED");
  }

  if(adv_robot_srv_client_.call(adv_robot_srv))
  {
    ROS_INFO("[FF_RenderWidget] service result %d", adv_robot_srv.response.success);
  }
  else
  {
    ROS_ERROR("[FF_RenderWidget] failed to call service advance_robot");
  }

  rate_->sleep();
}

void FrameFabRenderWidget::setValue(int i)
{

}

void FrameFabRenderWidget::setScaleFactor(QString unit_info)
{
  Q_EMIT(sendLogInfo(QString("-----------MODEL UNIT-----------")));
  Q_EMIT(sendLogInfo(unit_info));

  if(QString("millimeter") == unit_info)
  {
    node_handle_.param("/framefab_mpp/unit_conversion_millimeter_to_meter", unit_conversion_scale_factor_, float(1));
  }

  if(QString("centimeter") == unit_info)
  {
    node_handle_.param("/framefab_mpp/unit_conversion_centimeter_to_meter", unit_conversion_scale_factor_, float(1));
  }

  if(QString("inch") == unit_info)
  {
    node_handle_.param("/framefab_mpp/unit_conversion_inch_to_meter", unit_conversion_scale_factor_, float(1));
  }

  if(QString("foot") == unit_info)
  {
    node_handle_.param("/framefab_mpp/unit_conversion_foot_to_meter", unit_conversion_scale_factor_, float(1));
  }

  Q_EMIT(sendLogInfo(QString("Convert to meter - scale factor %1").arg(unit_conversion_scale_factor_)));

  // if collision objects exists, rebuild
  if(0 != ptr_wire_frame_collision_objects_->sizeOfCollisionObjectsList())
  {
    ROS_INFO_NAMED("framefab_mpp", "[FF_RenderWidget] rebuilding model");
    Q_EMIT(sendLogInfo(QString("Model rebuilt for update on unit.")));
    this->constructCollisionObjects();
  }
}

void FrameFabRenderWidget::setRefPoint(double ref_pt_x, double ref_pt_y, double ref_pt_z)
{
  Q_EMIT(sendLogInfo(QString("-----------REF POINT-----------")));
  Q_EMIT(sendLogInfo(QString("(%1, %2, %3)").arg(ref_pt_x).arg(ref_pt_y).arg(ref_pt_z)));

  ref_pt_x_ = ref_pt_x;
  ref_pt_y_ = ref_pt_y;
  ref_pt_z_ = ref_pt_z;

  // if collision objects exists, rebuild
  if(0 != ptr_wire_frame_collision_objects_->sizeOfCollisionObjectsList())
  {
    ROS_INFO_NAMED("framefab_mpp", "rebuilding model");
    Q_EMIT(sendLogInfo(QString("Model rebuilt for update on Ref Point.")));
    this->constructCollisionObjects();
  }
}

void FrameFabRenderWidget::constructCollisionObjects()
{
  //--------------- load wireframe collision objects start -----------------------
  ptr_wire_frame_collision_objects_->constructCollisionObjects(
      ptr_move_group_->getPlanningFrame(),
      unit_conversion_scale_factor_, collision_cylinder_radius_,
      ref_pt_x_, ref_pt_y_, ref_pt_z_);

  wire_frame::MoveitLinearMemberCollisionObjectsListPtr ptr_collision_objects
      = ptr_wire_frame_collision_objects_->getCollisionObjectsList();

  std::vector<std::string> objects_id = ptr_planning_scene_interface_->getKnownObjectNames();
  ptr_planning_scene_interface_->removeCollisionObjects(objects_id);

  std::vector<moveit_msgs::CollisionObject> collision_objects;

  for (int i=0; i < ptr_collision_objects->size(); i++)
  {
    collision_objects.push_back((*ptr_collision_objects)[i]->edge_cylinder_collision);
    //scene.world.collision_objects.push_back((*ptr_collision_objects)[i]->edge_cylinder_collision);
    //scene.world.collision_objects.push_back((*ptr_collision_objects)[i]->start_vertex_collision);
    //scene.world.collision_objects.push_back((*ptr_collision_objects)[i]->end_vertex_collision);


    //moveit_msgs::ObjectColor cylinder_moveit_color;
    //cylinder_moveit_color.id = (*ptr_collision_objects)[i]->start_vertex_collision.id.c_str();
    //cylinder_moveit_color.color = cylinder_color_;

    //scene.object_colors.push_back(cylinder_moveit_color);
    //ptr_current_scene->setObjectColor((*ptr_collision_objects)[i]->start_vertex_collision.id.c_str(), start_color_);
    //ptr_current_scene->setObjectColor((*ptr_collision_objects)[i]->end_vertex_collision.id.c_str(), end_color_);
  }
  //--------------- load wireframe collision objects end -----------------------

  ptr_planning_scene_interface_->addCollisionObjects(collision_objects);

  rate_->sleep();
}

void FrameFabRenderWidget::readFile()
{
  QString filename = QFileDialog::getOpenFileName(
      this,
      tr("Open File"),
      "$HOME/Documents",
      tr("pwf Files (*.pwf)"));

  if(filename.isEmpty())
  {
    ROS_ERROR_NAMED("framefab_mpp", "[FF_RenderWidget] Read Model Failed!");
    return;
  }
  else
  {
    ROS_INFO_NAMED("framefab_mpp", "[FF_RenderWidget] Open Model: success.");
  }

  // compatible with paths in Chinese
  QTextCodec *code = QTextCodec::codecForName("gd18030");
  QTextCodec::setCodecForLocale(code);
  QByteArray byfilename = filename.toLocal8Bit();

  ptr_wire_frame_collision_objects_.reset();
  ptr_wire_frame_collision_objects_ = boost::make_shared<wire_frame::WireFrameCollisionObjects>();

  //--------------- load wireframe linegraph end -----------------------
  if (filename.contains(".obj") || filename.contains(".OBJ"))
  {
    ptr_wire_frame_collision_objects_->LoadFromOBJ(byfilename.data());
  }
  else
  {
    ptr_wire_frame_collision_objects_->LoadFromPWF(byfilename.data());
  }

  if (0 == ptr_wire_frame_collision_objects_->SizeOfVertList())
  {
    Q_EMIT(sendLogInfo(QString("Input frame empty, no links to draw.")));
    return;
  }

  ROS_INFO_NAMED("framefab_mpp", "[FF_RenderWidget] LineGraph Built: success.");
  //--------------- load wireframe linegraph end -----------------------

  this->constructCollisionObjects();

  QString parse_msg =
      "Nodes: "     + QString::number(ptr_wire_frame_collision_objects_->SizeOfVertList()) + "\n"
          + "Links: "    + QString::number(ptr_wire_frame_collision_objects_->SizeOfEdgeList()) + "\n"
          + "Pillars: "  + QString::number(ptr_wire_frame_collision_objects_->SizeOfPillar()) + "\n"
          + "Ceilings: " + QString::number(ptr_wire_frame_collision_objects_->SizeOfCeiling());

  Q_EMIT(sendLogInfo(QString("-----------MODEL INPUT-----------")));
  Q_EMIT(sendLogInfo(parse_msg));
  Q_EMIT(sendLogInfo(QString("factor scale: %1").arg(unit_conversion_scale_factor_)));

  ROS_INFO_NAMED("framefab_mpp", "[FF_RenderWidget] model loaded successfully");
}

} /* namespace framefab */