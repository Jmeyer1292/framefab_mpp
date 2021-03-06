//
// Created by yijiangh on 7/10/17.
//

//
// Created by yijiangh on 6/28/17.
//
//#include "framefab_gui/states/scanning_state.h"

#include <ros/console.h>
#include <framefab_gui/states/select_plan_state.h>
#include <framefab_gui/states/system_init_state.h>
#include <framefab_gui/states/simulating_state.h>
//#include <QtConcurrent/QtConcurrentRun>

void framefab_gui::SelectPlanState::onStart(FrameFabWidget& gui)
{
  gui.setText("Select Plan State.\nPlease select the desired plan to be simulated in selection window.\nClick <Simulate> to continue. ");
  gui.setButtonsEnabled(false);
  selected_plan_id_ = -1;

  selectPlan(gui);
}

void framefab_gui::SelectPlanState::onExit(FrameFabWidget& gui) {}

void framefab_gui::SelectPlanState::onNext(FrameFabWidget& gui)
{
  gui.setButtonsEnabled(true);
  selected_plan_id_ = gui.selection_widget().getSelectedValue();
  gui.selection_widget().close();

  gui.appendText("\nselect plan state finished! Selected Plan: #" + std::to_string(selected_plan_id_));

  std::vector<int> plan_ids;

  if(gui.selection_widget().getSimulateType())
  {
    // simulate single
    ROS_INFO_STREAM("[selection widget] Single Simulation!");
    plan_ids.push_back(selected_plan_id_);
  }
  else
  {
    ROS_INFO_STREAM("[selection widget] All Simulations Until Selected id!");
    // simulate until selected id
    for(std::size_t i = 0; i <= selected_plan_id_; i++)
    {
      plan_ids.push_back(i);
    }
  }

  Q_EMIT newStateAvailable(new SimulatingState(plan_ids));
}

void framefab_gui::SelectPlanState::onBack(FrameFabWidget& gui)
{
  gui.selection_widget().cleanUpVisual();
  Q_EMIT newStateAvailable(new SystemInitState());
}

void framefab_gui::SelectPlanState::onReset(FrameFabWidget& gui)
{
  gui.selection_widget().cleanUpVisual();
  Q_EMIT newStateAvailable(new SystemInitState());
}

void framefab_gui::SelectPlanState::selectPlan(FrameFabWidget& gui)
{
  gui.selection_widget().setMode(framefab_gui::SelectionWidget::PLAN_SELECTION);
  gui.selection_widget().show();
  gui.selection_widget().loadParameters();
}


