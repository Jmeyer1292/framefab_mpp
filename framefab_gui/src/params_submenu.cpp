#include <framefab_gui/params_submenu.h>
#include <ui_params_submenu.h>

framefab_gui::ParamsSubmenu::ParamsSubmenu(QWidget* parent) : QWidget(parent)
{
  ui_ = new Ui::ParamsSubmenu();
  ui_->setupUi(this);

  // Set up option menus
  // ModelInput
  model_input_widget_ = new ModelInputConfigWidget(framefab_msgs::ModelInputParameters());
  connect(ui_->pushbutton_model_input, SIGNAL(clicked()), model_input_widget_, SLOT(show()));

  // Path Input
  path_input_widget_ = new PathInputConfigWidget(framefab_msgs::PathInputParameters());
  connect(ui_->pushbutton_path_input, SIGNAL(clicked()), path_input_widget_, SLOT(show()));

  // Save Request Connection
  connect(model_input_widget_, SIGNAL(parameters_save_requested()), this, SIGNAL(saveRequested()));
  connect(path_input_widget_, SIGNAL(parameters_save_requested()), this, SIGNAL(saveRequested()));

  // Accept Request Connection
  connect(model_input_widget_, SIGNAL(parameters_changed()), this, SIGNAL(acceptRequested()));
  connect(path_input_widget_, SIGNAL(parameters_changed()), this, SIGNAL(acceptRequested()));
}

const framefab_msgs::ModelInputParameters& framefab_gui::ParamsSubmenu::modelInputParams() const
{
  return model_input_widget_->params();
}

void framefab_gui::ParamsSubmenu::setModelInputParams(
    const framefab_msgs::ModelInputParameters& params)
{
  model_input_widget_->params() = params;
  model_input_widget_->update_display_fields();
}

const framefab_msgs::PathInputParameters& framefab_gui::ParamsSubmenu::pathInputParams() const
{
  return path_input_widget_->params();
}

void framefab_gui::ParamsSubmenu::setPathInputParams(
    const framefab_msgs::PathInputParameters& params)
{
  path_input_widget_->params() = params;
  path_input_widget_->update_display_fields();
}

//const framefab_msgs::PathPlanningParameters& framefab_gui::ParamsSubmenu::pathPlanningParams() const
//{
//  return path_planning_params_->params();
//}

//void framefab_gui::ParamsSubmenu::setPathPlanningParams(const framefab_msgs::PathPlanningParameters& params)
//{
//  path_planning_params_->params() = params;
//  path_planning_params_->update_display_fields();
//}

//const framefab_msgs::ScanPlanParameters& framefab_gui::ParamsSubmenu::scanParams() const
//{
//  return scan_params_->params();
//}
//
//void framefab_gui::ParamsSubmenu::setScanParams(const framefab_msgs::ScanPlanParameters& params)
//{
//  scan_params_->params() = params;
//  scan_params_->update_display_fields();
//}