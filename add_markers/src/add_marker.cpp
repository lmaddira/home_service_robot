#include <ros/ros.h>
#include <iostream>
#include <visualization_msgs/Marker.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseActionGoal.h>
#include <move_base_msgs/MoveBaseActionFeedback.h>


bool pick = false;
bool place = false;
visualization_msgs::Marker marker;
ros::Publisher marker_pub;
visualization_msgs::Marker marker2;
ros::Publisher marker_pub2;
/*
void Odom_CallBack(const nav_msgs::Odometry::ConstPtr& msg)
{
    ROS_INFO("Inside Odomcallback\n");
    if(first_run && got_goal)
    {
        start_x =  msg->pose.pose.position.x;
        start_y =  msg->pose.pose.position.y;
        first_run = false;
        marker.lifetime = ros::Duration();
        marker.action = visualization_msgs::Marker::ADD;
    }
    float x =  msg->pose.pose.position.x;
    float y =  msg->pose.pose.position.y;
    if(marker.pose.position.x <= abs(start_x - x) && marker.pose.position.y <= abs(start_y - y) && got_goal)
    {
        ROS_INFO("Reached the destination\n");
        reached = true;
        first_run = true;
        got_goal = false;
        marker.action = visualization_msgs::Marker::DELETE;
        ros::Duration(1,0).sleep();        
        // return true;
    }
    else
    {
        marker_pub.publish(marker);
        ROS_INFO(" dist_x  and dist_y now %f, %f \n",abs(start_x - x),abs(start_y - y));

    }
    ROS_INFO(" dist_x  and dist_y now %f, %f \n",abs(start_x - x),abs(start_y - y));
    ros::Duration(0.5,0).sleep();
    return;
}*/
void Goal_callback(const move_base_msgs::MoveBaseActionGoal::ConstPtr& Actiongoal)//(const geometry_msgs::PoseStamped::ConstPtr& goal)
{
    // if((marker.pose.position.x !=goal->pose.position.x) || (marker.pose.position.y != goal->pose.position.y)|| 
    //     (marker.pose.position.z != goal->pose.position.z) )


        // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    ROS_INFO("got the goal position\n");
    marker.pose.position.x = Actiongoal->goal.target_pose.pose.position.x;
    marker.pose.position.y = Actiongoal->goal.target_pose.pose.position.y;
    marker.pose.position.z = Actiongoal->goal.target_pose.pose.position.z;
    marker.pose.orientation.x = Actiongoal->goal.target_pose.pose.orientation.x;
    marker.pose.orientation.y = Actiongoal->goal.target_pose.pose.orientation.y;
    marker.pose.orientation.z = Actiongoal->goal.target_pose.pose.orientation.z;
    marker.pose.orientation.w = Actiongoal->goal.target_pose.pose.orientation.w;
    marker2.pose.position.x = Actiongoal->goal.target_pose.pose.position.x;
    marker2.pose.position.y = Actiongoal->goal.target_pose.pose.position.y;
    marker2.pose.position.z = Actiongoal->goal.target_pose.pose.position.z;
    marker2.pose.orientation.x = Actiongoal->goal.target_pose.pose.orientation.x;
    marker2.pose.orientation.y = Actiongoal->goal.target_pose.pose.orientation.y;
    marker2.pose.orientation.z = Actiongoal->goal.target_pose.pose.orientation.z;
    marker2.pose.orientation.w = Actiongoal->goal.target_pose.pose.orientation.w;
    marker.lifetime = ros::Duration();
    marker.action = visualization_msgs::Marker::ADD;
    marker2.lifetime = ros::Duration();
    marker2.action = visualization_msgs::Marker::ADD;
    if(!pick) // to loop between pick and place
    {
        marker2.text = "Pick object";
        pick = true; // for first goal would be to pick 
        place = false;
    }
    else
    {
        marker2.text = "object expected here";
        marker.action = visualization_msgs::Marker::DELETE;
        place = true; // second goal would be to place
        pick = false;
    }
}

void Result_callback(const move_base_msgs::MoveBaseActionResult::ConstPtr& result)
{
    if(pick)
    {
        ROS_INFO("Reached the destination\n");
        std::cout<<"result is :"<<result->result<<"\n";
        std::cout<<"result status is "<<result->status.status<<"\n";
        marker.action = visualization_msgs::Marker::DELETE;
        marker_pub.publish(marker);
        marker_pub2.publish(marker2);
        ros::Duration(1,0).sleep();
    }else
    {
        ROS_INFO("Reached the destination\n");
        marker.action = visualization_msgs::Marker::ADD;
        marker2.text = "object placed";
        marker_pub.publish(marker);
        marker_pub2.publish(marker2);
    }
    

}
void Feedback_callback(const move_base_msgs::MoveBaseActionFeedback::ConstPtr& feedback)//
{
    if(pick)
    {
        ROS_INFO("feedback currently is %u \n ",feedback->status.status);
        marker_pub.publish(marker);
        marker_pub2.publish(marker2);
    }else // assumed its for place
    {
        ROS_INFO("feedback currently is %u \n ",feedback->status.status);
        marker_pub.publish(marker);
        marker_pub2.publish(marker2);
\
    }
    
    ros::Duration(0.5,0).sleep();
}


int main( int argc, char** argv )
{
    ros::init(argc, argv, "add_markers");
    ros::NodeHandle n;
    ros::Rate r(1);
    marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);
    marker_pub2 = n.advertise<visualization_msgs::Marker>("visualization_marker2", 1);
    
    // Set our initial shape type to be a cube
    uint32_t shape = visualization_msgs::Marker::CUBE;

    
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();
    marker2.header.frame_id = "map";
    marker2.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;
    marker2.ns = "text";
    marker2.id = 1;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;
    marker2.type = visualization_msgs::Marker::TEXT_VIEW_FACING;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    // marker.action = visualization_msgs::Marker::ADD;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.3;
    marker.scale.y = 0.3;
    marker.scale.z = 0.3;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker2.scale.x = 0.3;
    marker2.scale.y = 0.3;
    marker2.scale.z = 0.1;

    marker2.color.r = 0.0f;
    marker2.color.g = 1.0f;
    marker2.color.b = 0.0f;
    marker2.color.a = 1.0;
    // // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    // marker.pose.position.x = 0;
    // marker.pose.position.y = 0;
    // marker.pose.position.z = 0;
    // marker.pose.orientation.x = 0.0;
    // marker.pose.orientation.y = 0.0;
    // marker.pose.orientation.z = 0.0;
    // marker.pose.orientation.w = 1.0;
    ros::Subscriber goal_sub = n.subscribe("/move_base/goal",1,Goal_callback);
    // Publish the marker
    while (marker_pub.getNumSubscribers() < 1)
    {
        if (!ros::ok())
        {
        return 0;
        }
        ROS_WARN_ONCE("Please create a subscriber to the marker");
        sleep(1);
    }
    ros::Subscriber goal_feed_sub = n.subscribe("/move_base/feedback",1,Feedback_callback);
    ros::Subscriber goal_res_sub = n.subscribe("/move_base/result",1,Result_callback);
        
    ros::Duration(1,0).sleep();
    ros::spin();
    return 0;
}