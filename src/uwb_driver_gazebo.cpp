#include "ros/ros.h"
#include "uwb_driver/UwbRange.h"
#include "geometry_msgs/PoseStamped.h"
#include <Eigen/Dense>
#include "eigen_conversions/eigen_msg.h"
#include <sstream>
#include <string.h>
#include <tf/transform_broadcaster.h>
#include <tf_conversions/tf_eigen.h>
using namespace Eigen;
// using namespace std;
using namespace tf;
ros::Publisher uwb_pub;
std::vector<int> nodesId;
std::vector<double> nodesPos;
std::vector<double> antennaOffset;
int node_num;
int antenna_num;
int count = 0;
int count_antenna = 0;
std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> d(0,0.02);

void gazeboCallback(const geometry_msgs::PoseStamped pose)
{
    uwb_driver::UwbRange msg;
    msg.header = std_msgs::Header();
    msg.header.frame_id = "uwb";
    msg.header.stamp = pose.header.stamp;
    msg.requester_id  = nodesId.back();
    msg.requester_idx = nodesId.back()%100;
    msg.responder_id  = nodesId[count];
    msg.responder_idx = nodesId[count]%100;

    Eigen::Affine3d pose_eigen;
    poseMsgToEigen(pose.pose, pose_eigen);
    static tf::TransformBroadcaster br;
    tf::Transform transform;

    if(antenna_num == 0)
    {
        msg.antenna = 0;
        tf::transformEigenToTF (pose_eigen, transform);
        br.sendTransform(tf::StampedTransform(transform, pose.header.stamp, pose.header.frame_id, "antenna"));
    }
    else
    {
        int antenna_id = count_antenna%antenna_num;
        msg.antenna = antenna_id + 1;
        double x = antennaOffset[antenna_id*3];
        double y = antennaOffset[antenna_id*3+1];
        double z = antennaOffset[antenna_id*3+2];
        Eigen::Affine3d trans = Eigen::Affine3d::Identity();
        trans.translate(Vector3d(x,y,z));
        pose_eigen = pose_eigen * trans;
        tf::transformEigenToTF(pose_eigen, transform);
        br.sendTransform(tf::StampedTransform(transform, pose.header.stamp, pose.header.frame_id, "antenna"+std::to_string(msg.antenna)));
        std::cout << " " <<"antenna"+std::to_string(msg.antenna)<< " " << std::endl;
    }

    count_antenna = count_antenna+1;

    double distance = sqrt(
        pow(pose_eigen(0,3) - nodesPos[count*3  ], 2)+
        pow(pose_eigen(1,3) - nodesPos[count*3+1], 2)+
        pow(pose_eigen(2,3) - nodesPos[count*3+2], 2));

    transform.setOrigin( tf::Vector3(nodesPos[count*3  ],nodesPos[count*3+1], nodesPos[count*3+2]));
    br.sendTransform(tf::StampedTransform(transform, pose.header.stamp, pose.header.frame_id, std::to_string(count)+"anchor"));

    msg.distance = distance + d(gen);
    msg.distance_err = 0.02;
    msg.responder_location.x = nodesPos[count*3];
    msg.responder_location.y = nodesPos[count*3+1];
    msg.responder_location.z = nodesPos[count*3+2];
    uwb_pub.publish(msg);

    if (antenna_num==0 || count_antenna%antenna_num == 0)
        count = (count+1)%node_num;

    ros::Duration(0.025).sleep();
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

    if(n.getParam("/uwb/antennaOffset", antennaOffset))
        for(auto it:antennaOffset)
            ROS_WARN("Get antenna Offset: %4.2f", it);

    antenna_num = antennaOffset.size()/3;
    ROS_WARN("Using %d antennas", antenna_num);

    ros::Subscriber sub = n.subscribe("/ground_truth_to_tf/pose", 1, gazeboCallback);

    uwb_pub = n.advertise<uwb_driver::UwbRange>("/uwb_endorange_info", 0);
    
    node_num = nodesId.size() - 1;
    
    ROS_WARN("Publishing..");

    ros::spin();

    return 0;
}