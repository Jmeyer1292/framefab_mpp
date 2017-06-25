//#include "framefab_gui/states/scanning_state.h"

#include <ros/console.h>
#include <framefab_gui/framefab_widget.h>
#include <framefab_gui/states/system_init_state.h>
#include <framefab_gui/states/path_planning_state.h>
#include <QtConcurrent/QtConcurrentRun>

// input params
#include <framefab_msgs/PathPlanning.h>
#include <framefab_msgs/ModelInputParameters.h>
#include <framefab_msgs/PathInputParameters.h>

// Class level constants
const static std::string PATH_PLANNING_SERVICE = "path_planning";

framefab_gui::PathPlanningState::PathPlanningState()
    : path_planning_action_client_(PATH_PLANNING_ACTION_SERVER_NAME, true)
{
//  ROS_INFO_STREAM("PathPlanningState Init.");
}

framefab_gui::PathPlanningState::~PathPlanningState()
{}

void framefab_gui::PathPlanningState::onStart(FrameFabWidget& gui)
{
  gui.setText("PathPlanning State.\n Please input data in parameter widget.\n Click 'Next' to continue after finished.");
//  gui.setButtonsEnabled(false);

  gui_ptr_ = &gui;

  QObject::connect(this, SIGNAL(feedbackReceived(QString)), this, SLOT(setFeedbackText(QString)));
  QtConcurrent::run(this, &PathPlanningState::makeRequest, gui.params().modelInputParams(),
                    gui.params().pathInputParams());
}

void framefab_gui::PathPlanningState::onExit(FrameFabWidget& gui) { gui.setButtonsEnabled(true); }

void framefab_gui::PathPlanningState::onNext(FrameFabWidget& gui)
{
//  Q_EMIT newStateAvailable(new SelectPathState());
}

void framefab_gui::PathPlanningState::onBack(FrameFabWidget& gui) {}

void framefab_gui::PathPlanningState::onReset(FrameFabWidget& gui) {}

void framefab_gui::PathPlanningState::makeRequest(
    framefab_msgs::ModelInputParameters model_params,
    framefab_msgs::PathInputParameters path_params)
{
  framefab_msgs::PathPlanning srv;
  srv.request.action = srv.request.FIND_ONLY;

  framefab_msgs::PathPlanningGoal goal;
  goal.action = framefab_msgs::PathPlanningGoal::FIND_ONLY;
  goal.use_default_parameters = false;
  goal.model_params = model_params;
  goal.path_params  = path_params;
  path_planning_action_client_.sendGoal(
      goal,
      boost::bind(&framefab_gui::PathPlanningState::pathPlanningDoneCallback, this, _1, _2),
      boost::bind(&framefab_gui::PathPlanningState::pathPlanningActiveCallback, this),
      boost::bind(&framefab_gui::PathPlanningState::pathPlanningFeedbackCallback, this, _1));
  ROS_INFO_STREAM("Goal sent from path planning state");
}

void framefab_gui::PathPlanningState::setFeedbackText(QString feedback)
{
  gui_ptr_->appendText("\n" + feedback.toStdString());
}

// Action Callbacks
void framefab_gui::PathPlanningState::pathPlanningDoneCallback(
    const actionlib::SimpleClientGoalState& state,
    const framefab_msgs::PathPlanningResultConstPtr& result)
{
  if(result->succeeded)
  {
//    Q_EMIT newStateAvailable(new SelectPlansState());
  }
  else
    Q_EMIT newStateAvailable(new SystemInitState());
}

void framefab_gui::PathPlanningState::pathPlanningActiveCallback()
{
  ROS_INFO_STREAM("Path Planning Goal is active");
}

void framefab_gui::PathPlanningState::pathPlanningFeedbackCallback(
    const framefab_msgs::PathPlanningFeedbackConstPtr& feedback)
{
//  Q_EMIT feedbackReceived(QString::fromStdString((feedback->last_completed).c_str()));
}