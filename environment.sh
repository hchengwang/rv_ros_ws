#!/bin/bash

echo "Setting up PYTHONPATH."
export PYTHONPATH=/home/ubuntu/code/rv_ros_ws/src:$PYTHONPATH

echo "Setup ROS_HOSTNAME."
export RV_ROS_ROOT=$HOME/code/rv_ros_ws

echo "Activating development."
source $RV_ROS_ROOT/devel/setup.bash

exec "$@" #Passes arguments. Need this for ROS remote launching to work.
