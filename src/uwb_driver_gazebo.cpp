#include "ros/ros.h"
#include "uwb_driver/UwbRange.h"
#include "geometry_msgs/PoseStamped.h"
#include <Eigen/Dense>
#include "eigen_conversions/eigen_msg.h"
#include <sstream>

using namespace Eigen;
using namespace tf;
ros::Publisher uwb_pub;
std::vector<int> nodesId;
std::vector<double> nodesPos;
int node_num;
int count = 0;
int count_antenna = 0;
std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> d(0,0.02);
Eigen::Affine3d offset_left, offset_right;

void gazeboCallback(const geometry_msgs::PoseStamped pose)
{
    uwb_driver::UwbRange msg;
    msg.header = std_msgs::Header();
    msg.header.frame_id = "uwb";
    msg.header.stamp = pose.header.stamp;
    msg.requester_id  = nodesId.back();
    msg.requester_idx =  nodesId.back()%100;
    msg.responder_id  = nodesId[count];
    msg.responder_idx = nodesId[count]%100;

    Eigen::Affine3d pose_eigen;
    poseMsgToEigen(pose.pose, pose_eigen);
    if (count_antenna%2)
    {
        msg.antenna = 0;
        pose_eigen = pose_eigen * offset_left;
        count = (count+1)%node_num;
    }
    else
    {
        msg.antenna = 255;
        pose_eigen = pose_eigen * offset_right;
    }
    count_antenna = count_antenna+1;


    double distance = sqrt(
        pow(pose_eigen(0,3) - nodesPos[count*3  ], 2)+
        pow(pose_eigen(1,3) - nodesPos[count*3+1], 2)+
        pow(pose_eigen(2,3) - nodesPos[count*3+2], 2));


    msg.distance = distance + d(gen);
    msg.distance_err = 0.02;
    msg.responder_location.x = nodesPos[count*3];
    msg.responder_location.y = nodesPos[count*3+1];
    msg.responder_location.z = nodesPos[count*3+2];
    uwb_pub.publish(msg);
    

    ros::Duration(0.0666).sleep();
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

    ros::Subscriber sub = n.subscribe("/ground_truth/pose", 1, gazeboCallback);

    uwb_pub = n.advertise<uwb_driver::UwbRange>("/uwb_endorange_info", 0);
    
    node_num = nodesId.size() - 1;
    
    ROS_WARN("Publishing..");

    offset_left.setIdentity();
    offset_right.setIdentity();
    offset_left.translate(Vector3d(-0.2, 0, 0));
    offset_right.translate(Vector3d(0, 0.2, 0));

    ros::spin();

    return 0;
}