#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <nav_msgs/Odometry.h>
#include <sstream>
using namespace std;


double ROBOT_X;
double ROBOT_Y;
double dist_margin = 0.22;

double marker_pick_x = -2.5;
double marker_pick_y = 0.0;
double marker_drop_x = 2.5;
double marker_drop_y = 0.0;

void robotPoseCallback(const nav_msgs::Odometry::ConstPtr &odom)
{
  // Update robot position
  ROBOT_X = odom->pose.pose.position.x;
  ROBOT_Y = odom->pose.pose.position.y;
}

double getDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}

int main( int argc, char** argv )
{
    ros::init(argc, argv, "add_markers");
    ros::NodeHandle n;
    ros::Rate r(1);
    ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

    bool obj_picked = false;

    // Set our initial shape type to be a cube
    uint32_t shape = visualization_msgs::Marker::CUBE;

    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.3;
    marker.scale.y = 0.3;
    marker.scale.z = 0.3;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = 0.0f;
    marker.color.b = 1.0f;
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();

    // Subscribe to /odom
    ros::Subscriber sub1 = n.subscribe("/odom", 10, robotPoseCallback);

    while (ros::ok())
    {   
        if (obj_picked)
        {
            // calculate dist
            double dropoff_act_dist = getDistance(ROBOT_X,ROBOT_Y,marker_drop_x,marker_drop_y);
            ROS_INFO("distance to dropoff: [%f]",dropoff_act_dist);
            if (dropoff_act_dist < dist_margin)
            {
                marker.action = visualization_msgs::Marker::ADD;
                marker.pose.position.x = marker_drop_x;
                marker.pose.position.y = marker_drop_y;
            }    
        }
        else
        {
            // calculate dist
            double pickup_act_dist = getDistance(ROBOT_X,ROBOT_Y,marker_pick_x,marker_pick_y);
            if (pickup_act_dist < dist_margin)
            {
                // picking up object
                marker.action = visualization_msgs::Marker::DELETE;
                ROS_INFO("marker picked!"); 
                obj_picked = true;
            }
            else
            {
                // calculate dist
                ROS_INFO("distance to pickup: [%f]",pickup_act_dist);
                marker.action = visualization_msgs::Marker::ADD;
                marker.pose.position.x = marker_pick_x;
                marker.pose.position.y = marker_pick_y;
                obj_picked = false;
            }
        }

        // Publish the Marker
        marker_pub.publish(marker);
        // Sleep for 1 seconds
        sleep(1);

        // Handle ROS communication events
        ros::spinOnce();
    }
}
