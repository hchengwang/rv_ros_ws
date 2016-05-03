#include "ros/ros.h"
#include "mit_msgs/MocapPosition.h"
#include <lcm/lcm.h>
#include <bot_core/bot_core.h>

lcm_t* lcm;

void chatterCallback(mit_msgs::MocapPosition msg)
{
    float x = msg.translational.x;
    float y = msg.translational.y;
    float z = msg.translational.z;

    float ax = msg.axisangle.x;
    float ay = msg.axisangle.y;
    float az = msg.axisangle.z;
    //ROS_INFO_STRING("HI GUYS " << x << "\t" << y << "\t" << z);
    //std::cout << "translation:\t" << x << "\t" << y << "\t" << z << std::endl;
    //std::cout << "axisangle:\t" << ax << "\t" << ay << "\t" << az << std::endl;

    bot_core_pose_t p;
    p.utime = bot_timestamp_now();
    p.pos[0] = x * 0.001;
    p.pos[1] = y * 0.001;
    p.pos[2] = 0; // ignore height

    double rpy_t[3];
    rpy_t[0] = 0; // ignore pitch and roll
    rpy_t[1] = 0; //
    rpy_t[2] = az;

    double quat_t[4];
    bot_roll_pitch_yaw_to_quat(rpy_t, quat_t);

    p.orientation[0] = quat_t[0];
    p.orientation[1] = quat_t[1];
    p.orientation[2] = quat_t[2];
    p.orientation[3] = quat_t[3];

    if (p.pos[0] == 0 && p.pos[1] == 0 && p.pos[2] == 0){
        std::cout << "missing vicon data" << std::endl;
    }else{
        bot_core_pose_t_publish(lcm, "POSE", &p);
    }
}

int main(int argc, char **argv)
{

	lcm = lcm_create(NULL);
	std::cout << "init lcm ok..." << std::endl;
	
	ros::init(argc, argv, "mocap_ros_to_lcm");
	ros::NodeHandle n;

	char* channel_name = "hardhat4";

	if(argc == 2){
		channel_name = argv[1];
	}

    ros::Subscriber sub = n.subscribe(channel_name, 1000, chatterCallback);
	ros::spin();
	return 0;
}
