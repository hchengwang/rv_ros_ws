#include "ros/ros.h"
#include "sensor_msgs/Image.h"

#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <lcm/lcm.h>
#include <bot_core/bot_core.h>
#include <cv_bridge_lcm/rv-cv-bridge-lcm.h>

using namespace cv;
using namespace std;

cvBridgeLCM* cv_bridge_lcm;

void callback(const sensor_msgs::ImageConstPtr& depthMap)
{
    cv_bridge::CvImagePtr cv_ptr;
    // cv_ptr = cv_bridge::toCvCopy(depthMap, depthMap->encoding); //copy image
    cv_ptr = cv_bridge::toCvCopy(depthMap, sensor_msgs::image_encodings::TYPE_32FC1); //copy image
    
    cv::Mat im_kinect_rgb = cv::Mat::zeros(480, 640, CV_8UC3);
    cv::Mat im_kinect_depth = cv::Mat::zeros(480, 640, CV_16UC1);
    
    cv::Mat im_depth = cv_ptr->image.clone();

//    // for ZED
//    int scale = 1;
//    int d_scale = 1;
//    if(depthMap->width == 640 && depthMap->height == 480) //ZED
//    {
//        ROS_INFO("ZED Camera 640x480");
//    }
//    else if( depthMap->width == 320 && depthMap->height == 240) //Structure
//    {
//        ROS_INFO("Structure Sensor 320x240");
//        scale = 2;
//        d_scale = 1000; //structure is in meters apparantly
//    }
//    else
//    {
//        ROS_INFO("UNDEFINED RESOLUTION DETECTED %dx%d",depthMap->width, depthMap->height);
//        scale = 0;
//    }

    // [y, x]: [240, 320] -> [480, 640]
    int scale = 2; // structure sensor

    for(int i = 0; i < im_depth.cols - 2; i++){
        for(int j = 0; j < im_depth.rows - 2; j++){
            int x = i * scale;
            int y = j * scale;
            // float m -> int mm
            //int16_t d = (int16_t)(im_depth.at<int16_t>(j, i)) * d_scale; 
            int16_t d = (int16_t)(im_depth.at<float>(j, i) * 1000);

            im_kinect_depth.at<int16_t>(y, x) =     d;
            if(scale == 2)
            {
                im_kinect_depth.at<int16_t>(y+1, x) =   d;
                im_kinect_depth.at<int16_t>(y, x+1) =   d;
                im_kinect_depth.at<int16_t>(y+1, x+1) = d;
            }
        }
    }

    cv_bridge_lcm->publish_kinect_frame(im_kinect_rgb, im_kinect_depth, (char*)"KINECT_FRAME");

    std::cout << im_kinect_depth(cv::Rect(300, 300, 10, 10)) 
        << std::endl << std::endl;    
    //cv::imshow("window", cv_ptr->image);
    //cv::waitKey(3);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "depth_converter");
    ros::NodeHandle n;
    //cv::namedWindow( "window" ); //this fails, I think its odroid specific though.

    lcm_t* lcm = lcm_create(NULL);
    cv_bridge_lcm = new cvBridgeLCM(lcm, lcm);

    cout << "init lcm ok..." << endl;
    ros::Subscriber sub = n.subscribe("/camera/depth/image", 1000, callback);
    ros::spin();
    return 0;
}
