#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

double marker_pick_x = -2.5;
double marker_pick_y = 0.0;
double marker_drop_x = 2.5;
double marker_drop_y = 0.0;

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "pick_objects");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  // set up the frame parameters
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  // Define a position and orientation for the robot to reach
  goal.target_pose.pose.position.x = marker_pick_x;
  goal.target_pose.pose.position.y = marker_pick_y;
  goal.target_pose.pose.orientation.w = 1.0;

   // Send the goal position and orientation for the robot to reach
  ROS_INFO("travelling to the pickup zone!");
  ac.sendGoal(goal);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {
    ROS_INFO("turtlebot sucessfully reached pickup zone!");

    // picking up object
    ros::Duration(5).sleep();

    // move to Dropoff location
    goal.target_pose.pose.position.x = marker_drop_x;
    goal.target_pose.pose.position.y = marker_drop_y;
    goal.target_pose.pose.orientation.w = 1.0;

    // Send the goal position and orientation for the robot to reach
    ROS_INFO("travelling to the dropoff zone!");
    ac.sendGoal(goal);

    // Wait an infinite time for the results
    ac.waitForResult();

    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {
      // Robot reached dropoff zone
      ROS_INFO("turtlebot sucessfully reached dropoff zone!");
    }
    else
    {
      ROS_INFO("turtlebot fails to reach dropoff zone!");
    }
    }
  else
    {
    ROS_INFO("turtlebot fails to reach pickup zone!");
    }
  return 0;
}