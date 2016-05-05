#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "sensor_msgs/CompressedImage.h"

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

void callback(const sensor_msgs::ImageConstPtr& imageMap)
{
    ROS_INFO("Received image %dx%d", imageMap->width, imageMap->height);
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr = cv_bridge::toCvCopy(imageMap, imageMap->encoding); //copy image
    //cv_ptr = cv_bridge::toCvCopy(depthMap, sensor_msgs::image_encodings::TYPE_32FC1); //copy image
    
    cv::Mat im_pi = cv::Mat::zeros(480, 640, CV_8UC3);
    im_pi = cv_ptr->image.clone();
    cv_bridge_lcm->publish_mjpg(im_pi, (char*)"IMAGE_PICAMERA");
}

void callback_compressed(const sensor_msgs::CompressedImageConstPtr& imageMap)
{
    ROS_INFO("Received image ");
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "depth_converter");
    ros::NodeHandle n;

    lcm_t* lcm = lcm_create(NULL);
    cv_bridge_lcm = new cvBridgeLCM(lcm, lcm);

    cout << "init lcm ok..." << endl;
    ros::Subscriber sub = n.subscribe("/trabant/camera_node/image/compressed", 1000, callback_compressed);
    ros::spin();
    return 0;
}
