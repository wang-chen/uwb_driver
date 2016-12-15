#include "ros/ros.h"
#include "uwb_driver/UwbRange.h"
#include "geometry_msgs/PoseStamped.h"
#include <sstream>
ros::Publisher uwb_pub;
std::vector<int> nodesId;
std::vector<double> nodesPos;
int node_num;
int count = 0;
std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> d(0,0.02);

void gazeboCallback(const geometry_msgs::PoseStamped pose)
{
    double distance = sqrt(
        pow(pose.pose.position.x - nodesPos[count*3  ], 2)+
        pow(pose.pose.position.y - nodesPos[count*3+1], 2)+
        pow(pose.pose.position.z - nodesPos[count*3+2], 2));

    uwb_driver::UwbRange msg;
    msg.header = std_msgs::Header();
    msg.header.frame_id = "uwb";
    msg.header.stamp = pose.header.stamp;
    msg.requester_id  = nodesId.back();
    msg.requester_idx =  nodesId.back()%100;
    msg.responder_id  = nodesId[count];
    msg.responder_idx = nodesId[count]%100;
    msg.distance = distance + d(gen);
    msg.distance_err = 0.02;
    msg.responder_location.x = nodesPos[count*3];
    msg.responder_location.y = nodesPos[count*3+1];
    msg.responder_location.z = nodesPos[count*3+2];
    uwb_pub.publish(msg);
    count = (count+1)%node_num;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "uwb_driver_gazebo");

    ros::NodeHandle n;

    if(n.getParam("/uwb/nodesId", nodesId))
        for (auto it:nodesId)
            ROS_WARN("Get node ID: %d", it);

    if(n.getParam("/uwb/nodesPos", nodesPos))
        for(auto it:nodesPos)
            ROS_WARN("Get node position: %4.2f", it);

    ros::Subscriber sub = n.subscribe("/ground_truth/pose", 1000, gazeboCallback);

    uwb_pub = n.advertise<uwb_driver::UwbRange>("/uwb_endorange_info", 0);
    
    node_num = nodesId.size() - 1;
    
    ROS_WARN("Publishing..");

    ros::spin();

    return 0;
}