#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <iostream>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Chase ball");

    // Request to set robot vel to lin_x and ang_z
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x    = lin_x;
    srv.request.angular_z   = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    const int white_pixel = 255;
    bool trun_left = false;
    bool move_forward = false;
    bool turn_right = false;

    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int i = 0; i < img.height * img.step; i+=3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) 
            {
                int col_num =(i % img.step);
                if (col_num <= img.step/3){trun_left = true;break;}
                else if (col_num <= (2*img.step)/3){move_forward = true;break;}
                else {turn_right = true;break;}
            }       
    }

    // ball found
    if (move_forward){move_forward = false;drive_robot(0.4,0.0); } 
    else if (trun_left){trun_left = false;drive_robot(0,0.3); } 
    else if (turn_right){turn_right = false;drive_robot(0,-0.3); }     
    // no ball found 
    else {drive_robot(0.0,0.0);}

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}