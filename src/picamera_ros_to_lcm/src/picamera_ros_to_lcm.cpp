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
#include <jpeg-utils/jpeg-utils.h>
#include <jpeg-utils/jpeg-utils-ijg.h>

using namespace cv;
using namespace std;

cvBridgeLCM* cv_bridge_lcm;

// deal with parameters
std::string veh;
int width;
int height;

void callback(const sensor_msgs::ImageConstPtr& imageMap)
{
    ROS_INFO("Received image %dx%d", imageMap->width, imageMap->height);
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr = cv_bridge::toCvCopy(imageMap, imageMap->encoding); //copy image
    //cv_ptr = cv_bridge::toCvCopy(depthMap, sensor_msgs::image_encodings::TYPE_32FC1); //copy image
    
    cv::Mat im_pi = cv::Mat::zeros(height, width, CV_8UC3);
    im_pi = cv_ptr->image.clone();
    cv_bridge_lcm->publish_mjpg(im_pi, (char*)"IMAGE_PICAMERA");
}

void callback_compressed(const sensor_msgs::CompressedImageConstPtr& imageMap)
{
    ROS_INFO("Received image ");

    std::stringstream ss;
    ss << "IMAGE_PICAMERA_" << veh;
    cv_bridge_lcm->publish_mjpg(imageMap->data, width, height, (char*)ss.str().c_str());

    // try decompress here
//	double t_tot = (double)cvGetTickCount();
//
//    cv::Mat im_pi = cv::Mat::zeros(480, 640, CV_8UC3);
//    im_pi = cv::imdecode(cv::Mat(imageMap->data), CV_LOAD_IMAGE_COLOR); //copy image
//    cv::cvtColor(im_pi, im_pi, CV_BGR2RGB);
//
//	t_tot = cvGetTickCount() - t_tot;
//	ROS_INFO("Total Decompressing Time %f ms", t_tot / ((double)cvGetTickFrequency()*1000.) );
//	//cv_bridge_lcm->publish_mjpg(im_pi, (char*)"IMAGE_PICAMERA");
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "republisher");

    // init nh as "~", and set up ~xxx in launch file
    ros::NodeHandle nh("~");

    // two ways to setup params
    nh.param<std::string>("veh", veh, "trabant");
    nh.param<int>("width", width, 640);
    nh.param<int>("height", height, 480);
    //ros::param::param<std::string>("~veh", veh, "trabant");
    std::stringstream ss;
    ss << "/" << veh << "/camera_node/image/compressed";
    ROS_INFO("Subscribe Topic: %s", ss.str().c_str());

    // lcm_t* lcm = lcm_create("udpm://239.255.76.67:7667?recv_buf_size=100000&ttl=1");
    lcm_t* lcm = lcm_create(NULL);

    cv_bridge_lcm = new cvBridgeLCM(lcm, lcm);

    cout << "init lcm ok..." << endl;
    ros::Subscriber sub = nh.subscribe(ss.str().c_str(), 1000, callback_compressed);
    ros::spin();
    return 0;
}
