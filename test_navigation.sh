#!/bin/sh
export TURTLEBOT3_MODEL="waffle_pi"
sleep 2
xterm -e roslaunch turtlebot3_gazebo turtlebot3_world.launch &
sleep 5
xterm -e roslaunch turtlebot3_navigation turtlebot3_navigation.launch