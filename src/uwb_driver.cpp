//_____________________________________________________________________________
//
// #includes
//_____________________________________________________________________________

//#define ROS_MASTER_URI		"http://localhost:11311"
//#define ROS_ROOT		"/opt/ros/indigo/share/ros"
#include <iostream>
#include "stdio.h"
#include <stdlib.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <math.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <signal.h>
#include <math.h>
#include <tf/transform_broadcaster.h>
#include <boost/assign/list_of.hpp>
#include <string.h>
#include "rcmIf.h"
#include "rcm.h"
#include "rn.h"
#include "hostInterfaceCommon.h"
#include "hostInterfaceRCM.h"
#include "hostInterfaceRN.h"
#include <sys/ioctl.h>
#include <rtwtypes.h>

//For log file use
#include <fstream>
#include <pwd.h>

#include "ros/ros.h"
#include "time.h"
#include <arpa/inet.h>

#include "uwb_driver/UwbRange.h"  //self-defined msg file to inform of a range info package from p4xx
#include "uwb_driver/UwbData.h"   //self-defined msg file to inform of a data info package from p4xx

//Services headers
#include "uwb_driver/uwbModeConfig.h"   //Mode selection service
#include "uwb_driver/uwbRangeComm.h"
#include "srv_return_verbal.h"

//_____________________________________________________________________________
//
// Debugging utilities
//_____________________________________________________________________________

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

#define DFLT_NODE_RATE      40
//_____________________________________________________________________________
//
// ID Management
//_____________________________________________________________________________

//localization data
std::vector<int> nodesId;
uint8_T nodesTotal = 0;
std::vector<double>nodesPos;

//_____________________________________________________________________________
//
// P4xx Serial Communications
//_____________________________________________________________________________
#define     DFLT_P4xx_PORT          "/dev/ttyACM0"
#define     P4XX_ITF_SERIAL         0
#define     P4XX_ITF_USB            1
#define     P4XX_ITF_IP             2

#define     MODE_RCM                0
#define     MODE_RN                 1

extern int msgIdCount;

int status;
rcmIfType rcmIf;
rcmConfiguration rcmConfig;
rnConfiguration rnInitConfig;
rnMsg_GetTDMASlotmapConfirm rnInitTdmaSlotMap;
rnMsg_SetTDMASlotmapRequest initTdmaSlotMapSet;
// Info message structures
rcmMsg_FullRangeInfo rangeInfo;
rnMsg_GetFullNeighborDatabaseConfirm ndbInfo;
rcmMsg_DataInfo dataInfo;
uint8_T uwb_msg[RN_USER_DATA_LENGTH];

//_____________________________________________________________________________
//
// Functions-wide shared variable
//_____________________________________________________________________________
static int32_T hostedP4xxId = -1;
static int32_T hostedP4xxIdx = -1;

int p4xxMode = MODE_RCM;


/*------------------------------------------------------Function prototypes---------------------------------------------*/
bool uwb_mode_config(uwb_driver::uwbModeConfig::Request &req, uwb_driver::uwbModeConfig::Response &res)
{
    ros::Time startTime = ros::Time::now();
    int success = -1;

    // Set the mode with 1 second time out
    while(ros::ok() && (ros::Time::now() - startTime).toSec() < 2)
    {
        success = -1;
        if(req.p4xxmode == MODE_RCM)
            success = rcmOpModeSet(RCRM_OPMODE_RCM);
        else if(req.p4xxmode == MODE_RN)
            success = rcmOpModeSet(RCRM_OPMODE_RN);
        else
        {
            ROS_INFO("There's no mode %d\n", req.p4xxmode);
            return true;
        }

        if (success != OK)
            ROS_INFO("Time out waiting for opmode confirm. Retrying...\n");
        else
        {
            ROS_INFO(KGRN "Mode %d confirmed!" RESET, req.p4xxmode);
            res.result = P4XX_CONFIRMED;
            p4xxMode = req.p4xxmode;
            return true;
        }
    }

    if(success != OK)
    {
        ROS_INFO(KRED "Time out to get confirm!" RESET);
        res.result = P4XX_CONFIRM_TIMEOUT;
    }

    return true;
}

bool uwb_range_comm(uwb_driver::uwbRangeComm::Request &req, uwb_driver::uwbRangeComm::Response &res)
{
    switch(req.action)
    {
    case RANGE_RQST:
    {
        rcmMsg_SendRangeRequest request;
        rcmMsg_SendRangeRequestConfirm confirm;

        // create request message
        request.msgType = htons(RCM_SEND_RANGE_REQUEST);
        request.msgId = htons(msgIdCount++);
        request.responderId = htonl(req.responder);
        request.antennaMode = req.antenna;
        request.dataSize = htons(0);

        // make sure no pending messages
        rcmIfFlush();

        // send request to RCM
        rcmIfSendPacket(&request, sizeof(request));

        // wait for response
        int numBytes = rcmIfGetPacket(&confirm, sizeof(confirm));

        // did we get a response from the RCM?
        if (numBytes == sizeof(confirm))
        {
            // Handle byte ordering
            confirm.msgType = ntohs(confirm.msgType);

            // is this the correct message type?
            if(confirm.msgType == RCM_SEND_RANGE_REQUEST_CONFIRM && confirm.status == OK)
            {
                res.result = P4XX_CONFIRMED;
                ROS_INFO(KGRN "Range request confirmed by P4xx." RESET);
            }
            else
            {
                res.result = P4XX_CONFIRM_TIMEOUT;
                ROS_INFO(KYEL "Range request error. Message Type: 0x%0x, Status: 0x%0x." RESET, confirm.msgType, confirm.status);
            }

        }
        else
        {
            res.result = P4XX_CONFIRM_TIMEOUT;
            ROS_INFO(KYEL "Confirm message not properly received." RESET);
        }

        break;
    }
    case SEND_DATA:
    {
        rcmMsg_SendDataRequest request;
        rcmMsg_SendDataConfirm confirm;

        int dataSize = req.data_size;

        // create request message
        request.msgType = htons(RCM_SEND_DATA_REQUEST);
        request.msgId = htons(msgIdCount++);
        request.antennaMode = req.antenna;
        request.dataSize = htons(dataSize);
        // make sure there isn't too much data
        if (dataSize > RCM_USER_DATA_LENGTH)
            dataSize = RCM_USER_DATA_LENGTH;

        // copy data into message
        memcpy(request.data, &req.data[0], dataSize);

        // make sure no pending messages
        rcmIfFlush();

        // send message to RCM
        rcmIfSendPacket(&request, sizeof(request));

        // wait for response
        int numBytes = rcmIfGetPacket(&confirm, sizeof(confirm));

        // did we get a response from the RCM?
        if (numBytes == sizeof(confirm))
        {
            // Handle byte ordering
            confirm.msgType = ntohs(confirm.msgType);

            // is this the correct message type?
            if (confirm.msgType == RCM_SEND_DATA_CONFIRM && confirm.status == OK)
            {
                res.result = P4XX_CONFIRMED;
                ROS_INFO(KGRN "Data broadcast confirmed by P4xx." RESET);
            }
            else
            {
                res.result = P4XX_CONFIRM_TIMEOUT;
                ROS_INFO(KYEL "Data broadcast error. Message Type: 0x%0x, Status: 0x%0x." RESET, confirm.msgType, confirm.status);
            }
        }
        else
        {
            res.result = P4XX_CONFIRM_TIMEOUT;
            ROS_INFO(KYEL "Confirm message not properly received." RESET);
        }

        break;
    }
    case RANGE_RQST_DATA:
    {
        rcmMsg_SendChannelizedRangeRequest request;
        rcmMsg_SendChannelizedRangeRequestConfirm confirm;

        int dataSize = req.data_size;

        // create request message
        request.msgType = htons(RCM_SEND_CHANNELIZED_RANGE_REQUEST);
        request.msgId = htons(msgIdCount);
        request.responderId = htonl(req.responder);
        request.antennaMode = req.antenna;
        request.codeChannel = req.channel;
        request.dataSize = htons(dataSize);

        memcpy(request.data, &req.data[0], dataSize);

        // make sure no pending messages
        rcmIfFlush();

        // send message to RCM
        rcmIfSendPacket(&request, sizeof(request));

        // wait for response
        int numBytes = rcmIfGetPacket(&confirm, sizeof(confirm));

        // did we get a response from the RCM?
        if (numBytes == sizeof(confirm))
        {
            // Handle byte ordering
            confirm.msgType = ntohs(confirm.msgType);

            // is this the correct message type?
            if (confirm.msgType == RCM_SEND_CHANNELIZED_RANGE_REQUEST_CONFIRM && confirm.status == OK)
            {
                res.result = P4XX_CONFIRMED;
                ROS_INFO(KGRN "Range + Data request confirmed by P4xx." RESET);
            }
            else
            {
                res.result = P4XX_CONFIRM_TIMEOUT;
                ROS_INFO(KYEL "Channelized range + data broadcast error. Message Type: 0x%0x, Status: 0x%0x." RESET, confirm.msgType, confirm.status);
            }

        }
        else
        {
            res.result = P4XX_CONFIRM_TIMEOUT;
            ROS_INFO(KYEL "Confirm message not properly received." RESET);
        }

        break;
    }
    case SET_RQST_DATA:
    {
        rnMsg_SetRequestDataRequest request;
        rnMsg_SetRequestDataConfirm confirm;

        int dataSize = req.data_size;

        // create request message
        request.msgType = htons(RN_SET_REQUEST_USER_DATA_REQUEST);
        request.msgId = htons(msgIdCount++);
        request.dataSize = htons(dataSize);

        memcpy(request.data, &req.data[0], dataSize);
        rcmIfFlush();

        // send message to RCM
        rcmIfSendPacket(&request, sizeof(request));

        // wait for response
        int numBytes = rcmIfGetPacket(&confirm, sizeof(confirm));

        // did we get a response from the RCM?
        if (numBytes == sizeof(rnMsg_SetRequestDataConfirm))
        {
            // Handle byte ordering
            confirm.msgType = ntohs(confirm.msgType);
            confirm.msgId = ntohs(confirm.msgId);

            // is this the correct message type?
            if (confirm.msgType == RN_SET_REQUEST_USER_DATA_CONFIRM && confirm.status == OK)
            {
                res.result = P4XX_CONFIRMED;
                ROS_INFO(KGRN "Set request data confirmed by P4xx." RESET);
            }
            else
            {
                res.result = P4XX_CONFIRM_TIMEOUT;
                ROS_INFO(KYEL "Set request data error. Message Type: 0x%0x, Status: 0x%0x." RESET, confirm.msgType, confirm.status);
            }
        }
        else
        {
            res.result = P4XX_CONFIRM_TIMEOUT;
            ROS_INFO(KYEL "Confirm message not properly received!" RESET);
        }
        break;

    }
    case SET_RSPD_DATA:
    {
        rnMsg_SetResponseDataRequest request;
        rnMsg_SetResponseDataConfirm confirm;

        int dataSize = req.data_size;

        // create request message
        request.msgType = htons(RN_SET_RESPONSE_USER_DATA_REQUEST);
        request.msgId = htons(msgIdCount++);
        request.dataSize = htons(dataSize);

        memcpy(request.data, &req.data[0], dataSize);
        rcmIfFlush();

        // send message to RCM
        rcmIfSendPacket(&request, sizeof(request));

        // wait for response
        int numBytes = rcmIfGetPacket(&confirm, sizeof(confirm));

        // did we get a response from the RCM?
        if (numBytes == sizeof(rnMsg_SetResponseDataConfirm))
        {
            // Handle byte ordering
            confirm.msgType = ntohs(confirm.msgType);
            confirm.msgId = ntohs(confirm.msgId);

            // is this the correct message type?
            if (confirm.msgType == RN_SET_RESPONSE_USER_DATA_CONFIRM && confirm.status == 0)
            {
                res.result = P4XX_CONFIRMED;
                ROS_INFO(KGRN "Set response data confirmed by P4xx." RESET);
            }
            else
            {
                res.result = P4XX_CONFIRM_TIMEOUT;
                ROS_INFO(KYEL "Set request data error. Message Type: 0x%0x, Status: 0x%0x." RESET, confirm.msgType, confirm.status);
            }
        }
        else
        {
            res.result = P4XX_CONFIRM_TIMEOUT;
            ROS_INFO(KYEL "Confirm message not received!" RESET);
        }
        break;

    }
    default:
        break;
    }

    return true;
}


/*------------------------------------------------------Function prototypes---------------------------------------------*/

//_____________________________________________________________________________
//
// main function
//_____________________________________________________________________________
using namespace std;

int main(int argc, char *argv[])
{
    //Create ros handler to node
    ros::init(argc, argv, "uwb_driver");
    ros::NodeHandle uwbDriverNodeHandle("~");
    ros::Publisher uwb_range_publisher;
    ros::Publisher uwb_data_publisher;

    /*-----------------------------------------------Mischelaneous-----------------------------------------------------------*/
    bool restEnable = true;
    if(uwbDriverNodeHandle.getParam("restEnable", restEnable))
    {
        if(restEnable)
            printf(KBLU "UWB node allowed to sleep!\n" RESET);
        else
            printf(KBLU "Not resting at all!\n" RESET);
    }
    else
    {
        printf(KRED "Couldn't retrieve param 'restEnable'. Sleep for 16ms after getting a range." RESET);
        restEnable = true;
    }

    //Collect the nodes IDs
    if(uwbDriverNodeHandle.getParam("/uwb/nodesId", nodesId))
    {
        nodesTotal = nodesId.size();
        printf(KBLU "Retrieved %d nodes IDs: ", nodesTotal);
        for (int i = 0; i < nodesTotal; i++)
            printf("%d\t", nodesId[i]);
        printf("\n" RESET);
    }
    else
    {
        printf(KRED "Failed to collect nodes IDs, terminating program!\n" RESET);
        return 0;
    }

    //Collect the node Positions
    if(uwbDriverNodeHandle.getParam("/uwb/nodesPos", nodesPos))
    {
        if( (nodesPos.size() / (double)nodesTotal) != 3)
        {
            if(nodesTotal != 0)
            {
                printf(KRED "Nodes locations not whole. Exitting\n" RESET);
                return 0;
            }
        }
        else
        {
            printf(KBLU "Retrieved %d pre-defined cordinates:\n" RESET, nodesTotal);

            for(int i = 0; i < nodesTotal; i++)
            {
                printf(KBLU "\ta%d = ", nodesId[i]);
                for( int j = 0; j < 3; j++)
                    printf("%4.2f\t", nodesPos[i*3 + j]);
                printf("\n" RESET);
            }
        }
    }
    else
    {
        printf(KRED "Nodes location not specified!\n" RESET);
        exit(0);
    }

    //Redundant
    int p4xxQueryRate = DFLT_NODE_RATE;
    if(uwbDriverNodeHandle.getParam("p4xxQueryRate", p4xxQueryRate))
        printf(KBLU "Retrieved value %d for param 'p4xxQueryRate'\n" RESET, p4xxQueryRate);
    else
        printf(KYEL "Couldn't retrieve param 'p4xxQueryRate', applying default value %dHz\n" RESET, p4xxQueryRate);

    bool ignrTimeoutUwbInit = true;
    if(uwbDriverNodeHandle.getParam("ignrTimeoutUwbInit", ignrTimeoutUwbInit))
        if(ignrTimeoutUwbInit)
            printf(KBLU "Retrieved value 'true' for param 'ignrTimeoutUwbInit'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'ignrTimeoutUwbInit'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'ignrTimeoutUwbInit', program will proceed if devices init. fails!\n" RESET);
        ignrTimeoutUwbInit = true;
    }

    bool publishUwbInfo = false;
    if(uwbDriverNodeHandle.getParam("publishUwbInfo", publishUwbInfo))
    {
        if(publishUwbInfo)
        {
            printf(KBLU "Retrieved value 'true' for param 'publishUwbData'!\n" RESET);
            uwb_range_publisher = uwbDriverNodeHandle.advertise<uwb_driver::UwbRange>("/uwb_endorange_info", 0);
            uwb_data_publisher = uwbDriverNodeHandle.advertise<uwb_driver::UwbData>("/uwb_data_info", 0);
        }

        else
            printf(KBLU "Retrieved value 'false' for param 'publishUwbInfo'!\n" RESET);
    }
    else
    {
        printf(KYEL "Couldn't retrieve param 'publishUwbInfo', program will proceed without publishing uwb data!\n" RESET);
        publishUwbInfo = false;
    }

    /*-----------------------------------------------Mischelaneous-----------------------------------------------------------*/



    /*-----------------------------------------P4xx interface configuration---------------------------------------------------*/
    //P4xx interface
    int p4xxInterface = P4XX_ITF_SERIAL;
    //check for the interface selected
    if(uwbDriverNodeHandle.getParam("p4xxInterfaceChoice", p4xxInterface))
        switch(p4xxInterface)
        {
        case    P4XX_ITF_SERIAL:
            printf(KBLU "P4xx Serial interface selected!\n" RESET);
            break;
        case    P4XX_ITF_USB:
            printf(KBLU "P4xx USB interface selected!\n" RESET);
            break;
        default:
            break;
        }
    else
    {
        printf(KRED "Failed to select communication interface, terminating program!\n" RESET);
        return 0;
    }

    string p4xxSerialPort = string(DFLT_P4xx_PORT);
    //check for the p4xx port's name
    if(uwbDriverNodeHandle.getParam("p4xxSerialPort", p4xxSerialPort))
        printf(KBLU "Retrieved value %s for param 'p4xxSerialPort'!\n" RESET, p4xxSerialPort.data());
    else
    {
        printf(KRED "Couldn't retrieve param 'p4xxSerialPort', program closed!\n" RESET);
        return 0;
    }

    bool autoConfigRn = true;
    if(uwbDriverNodeHandle.getParam("autoConfigRn", autoConfigRn))
        if(autoConfigRn)
            printf(KBLU "Retrieved value 'true' for param 'autoConfigRn'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'autoConfigRn'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'autoConfigRn'. Retaining configuration on P440 instead!\n" RESET);
        autoConfigRn = false;
    }

    printf("\nRCM Localization App\n\n");

    if(p4xxInterface == P4XX_ITF_SERIAL)
        rcmIf = rcmIfSerial;
    else if (p4xxInterface == P4XX_ITF_USB)
        rcmIf = rcmIfUsb;

    // initialize the interface to the RCM
    if (rcmIfInit(rcmIf, &p4xxSerialPort[0]) != OK)
    {
        printf("Initialization failed.\n");
        exit(0);
    }

    // Make sure RCM is awake
    if (rcmSleepModeSet(RCRM_SLEEP_MODE_ACTIVE) != 0)
    {
        printf("Time out waiting for sleep mode set.\n");
        if(!ignrTimeoutUwbInit)
            exit(0);
    }

    // execute Built-In Test - verify that radio is healthy
    if (rcmBit(&status) != 0)
    {
        printf("Time out waiting for BIT.\n");
        if(!ignrTimeoutUwbInit)
            exit(0);
    }

    if (status != OK)
    {
        printf("Built-in test failed - status %d.\n", status);
        if(!ignrTimeoutUwbInit)
            exit(0);
    }
    else
    {
        printf("Radio passes built-in test.\n\n");
    }

    // retrieve config from RCM
    if (rcmConfigGet(&rcmConfig) != 0)
    {
        printf("Time out waiting for config confirm.\n");
        if(!ignrTimeoutUwbInit)
            exit(0);
    }
    else
    {
        //check for device node ID
        int32_T uwbNodeId = rcmConfig.nodeId;

        //search for index in the mobile profile
        hostedP4xxId = -1;

        for (uint32_T i = 0; i < nodesTotal; i++)
            if(uwbNodeId == nodesId[i])
            {
                hostedP4xxId = uwbNodeId;
                hostedP4xxIdx = i;
            }

        if(hostedP4xxId == -1)
        {
            printf("Node not involved! Exiting\n");
            exit(0);
        }
        else
        {
            printf(KBLU "Device ID: %d\n" RESET, rcmConfig.nodeId);
            uwbDriverNodeHandle.setParam("/uwb/hostedP4xxId", uwbNodeId);
        }
    }

    //initialize P4xx serial interface
    if(p4xxMode == MODE_RCM)
    {
        // Make sure opmode is RN mode
        while(rcmOpModeSet(RCRM_OPMODE_RCM) != 0 && ros::ok())
        {
            printf("Time out waiting for opmode set.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        // Set the P4xx to active mode
        if (rcmSleepModeSet(RCRM_SLEEP_MODE_ACTIVE) != 0)
        {
            printf("Time out waiting for sleep mode set.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

    }
    else
    {
        if(autoConfigRn)
        {
            // Set the NDB Neighbor Age arbitrarily high to keep nodes from dropping out
            rnInitConfig.maxNeighborAgeMs = 4294967295;

            // Set network mode to TDMA
            rnInitConfig.networkSyncMode = RN_NETWORK_SYNC_MODE_TDMA;

            //Range type for NDB = PRM;
            rnInitConfig.rnFlags &= ~(1<<2);

            // Autosend the range.
            rnInitConfig.autosendType = 0x01;

            // Let's do a NDB update rate of 100 ms
            // rnInitConfig.autosendNeighborDbUpdateIntervalMs = 100;

            if (rnConfigSet(&rnInitConfig) != 0)
            {
                printf("Time out waiting for rnConfig confirm.\n");
                if(!ignrTimeoutUwbInit)
                    exit(0);
            }

            printf(KYEL "Slotmap auto-configuration not yet developed! Using the current configuration!" RESET);

            // Retrieve TDMA slot map
            while (rnTdmaSlotMapGet(&rnInitTdmaSlotMap) != 0 && ros::ok())
            {
                printf("Time out waiting for rnTdmaSlotMap confirm.\n");
                if(!ignrTimeoutUwbInit)
                    exit(0);
            }

            for (uint32_T i = 0; i < rnInitTdmaSlotMap.numSlots; i++)
            {
                printf(KBLU "SLOT %02d: ", rnInitTdmaSlotMap.slots[i].slot.slotNumber);
                printf("Req ID:%u   ", rnInitTdmaSlotMap.slots[i].slot.requesterId);
                printf("Rsp ID:%u   ", rnInitTdmaSlotMap.slots[i].slot.responderId);
                printf("Pii:%u   ", rnInitTdmaSlotMap.slots[i].slot.integrationIndex);
                printf("Channel:%u   ", rnInitTdmaSlotMap.slots[i].slot.codeChannel);
                printf("Antenna:%u   ", rnInitTdmaSlotMap.slots[i].slot.antennaMode);
                printf("Flags:0x%X   ", rnInitTdmaSlotMap.slots[i].slot.flags);
                printf("Type:%u   ", rnInitTdmaSlotMap.slots[i].slot.slotType);
                printf("Man Time:%u\n" RESET, rnInitTdmaSlotMap.slots[i].slot.requestedDurationMicroseconds);
            }
        }

        // Make sure opmode is RN mode
        while(rcmOpModeSet(RCRM_OPMODE_RN) != 0 && ros::ok())
        {
            printf("Time out waiting for opmode set.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        // Set the P4xx to active mode
        if (rcmSleepModeSet(RCRM_SLEEP_MODE_ACTIVE) != 0)
        {
            printf("Time out waiting for sleep mode set.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }
    }

    ros::ServiceServer modeService = uwbDriverNodeHandle.advertiseService("/uwb_mode_config", uwb_mode_config);
    ROS_INFO(KGRN "UWB MODE SERVICE AVAILABLE." RESET);

    ros::ServiceServer rangeCommService = uwbDriverNodeHandle.advertiseService("/uwb_range_comm", uwb_range_comm);
    ROS_INFO(KGRN "UWB RANGE-COMM SERVICE AVAILABLE" RESET);

    /*-----------------------------------------P4xx interface configuration---------------------------------------------------*/


    /*-------------------------------------------------Initial Trilateration----------------------------------------------------*/

    ros::Duration sleepTime = ros::Duration(1.0/p4xxQueryRate);
    //get the initial position by trilaterating the average
    while(ros::ok())
    {
        ros::spinOnce();
        std::vector<double> nodesPosUpd;

        //Check if the positions have been updated
        if(uwbDriverNodeHandle.getParam("/uwb/nodesPosUpd", nodesPosUpd))
        {
            if(nodesPosUpd.size() == nodesPos.size())
                nodesPos = nodesPosUpd;
        }
        uint8_T msg_type = -1;
        //Check and find the coresponding index of the update
        int nodeIndex = -1;
        uint8_T msg_type_rt = 1;
        switch (msg_type_rt = rcmInfoGet(&rangeInfo, &dataInfo, &ndbInfo))//get distance
        {
        case RANGEINFO:
        {
            if(rangeInfo.precisionRangeMm < 75000)
            {
                for(int i = 0; i < nodesTotal; i++)
                    if((uint32_T)nodesId[i] == rangeInfo.responderId)
                    {
                        nodeIndex = i;
                        msg_type = RANGEINFO;
                        break;
                    }
            }
        }
        case DATAINFO:
        {
            for(int i = 0; i < nodesTotal; i++)
                if((uint32_T)nodesId[i] == dataInfo.sourceId)
                {
                    nodeIndex = i;
                    msg_type = DATAINFO;
                    break;
                }
        }

        default:
            break;

        }
        if (nodeIndex != -1)
        {
            if(msg_type == RANGEINFO && rangeInfo.rangeStatus == OK)
            {
                uwb_driver::UwbRange uwb_range_info_msg;
                uwb_range_info_msg.header = std_msgs::Header();
                uwb_range_info_msg.header.frame_id = "uwb";
                uwb_range_info_msg.header.stamp = ros::Time::now();
                uwb_range_info_msg.requester_id = hostedP4xxId;
                uwb_range_info_msg.requester_idx = hostedP4xxIdx;
                uwb_range_info_msg.responder_id = nodesId[nodeIndex];
                uwb_range_info_msg.responder_idx = nodeIndex;
                uwb_range_info_msg.distance = rangeInfo.precisionRangeMm/1000.0;
                uwb_range_info_msg.distance_err = rangeInfo.precisionRangeErrEst/1000;
                uwb_range_info_msg.distance_dot = rangeInfo.filteredRangeVel/1000.0;
                uwb_range_info_msg.distance_dot_err = rangeInfo.filteredRangeVel/1000.0;
                uwb_range_info_msg.antenna = rangeInfo.antennaMode;
                uwb_range_info_msg.stopwatch_time = rangeInfo.stopwatchTime;
                uwb_range_info_msg.uwb_time = rangeInfo.timestamp;
                uwb_range_info_msg.responder_location.x = nodesPos[nodeIndex*3];
                uwb_range_info_msg.responder_location.y = nodesPos[nodeIndex*3+1];
                uwb_range_info_msg.responder_location.z = nodesPos[nodeIndex*3+2];

                if(publishUwbInfo)
                    uwb_range_publisher.publish(uwb_range_info_msg);

                printf("RANGEINFO:Time=%.4f\ttu = %zu\tant=%d\tRqIDx=%d\tRqID=%d\tRSIDx=%d\tRSID=%d\td=%6.3f, de = %6.3f, dd = %6.3f, dde = %6.3f\tsw=%d\tx=%6.2f\ty=%6.2f\tz=%6.2f\n",
                       uwb_range_info_msg.header.stamp.toSec(),
                       uwb_range_info_msg.uwb_time,
                       uwb_range_info_msg.antenna,
                       uwb_range_info_msg.requester_idx+1,
                       uwb_range_info_msg.requester_id,
                       uwb_range_info_msg.responder_idx+1,
                       uwb_range_info_msg.responder_id,
                       uwb_range_info_msg.distance,
                       uwb_range_info_msg.distance_err,
                       uwb_range_info_msg.distance_dot,
                       uwb_range_info_msg.distance_dot_err,
                       uwb_range_info_msg.stopwatch_time,
                       uwb_range_info_msg.responder_location.x,
                       uwb_range_info_msg.responder_location.y,
                       uwb_range_info_msg.responder_location.z);

            }
            else if(msg_type == DATAINFO)
            {
                uwb_driver::UwbData uwb_data_info_msg;
                uwb_data_info_msg.header = std_msgs::Header();
                uwb_data_info_msg.header.frame_id = "uwb";
                uwb_data_info_msg.header.stamp = ros::Time::now();
                uwb_data_info_msg.source_id = nodesId[nodeIndex];
                uwb_data_info_msg.source_idx = nodeIndex;

                uwb_data_info_msg.antenna = dataInfo.antennaId;
                uwb_data_info_msg.uwb_time = dataInfo.timestamp;
                uwb_data_info_msg.data_size = dataInfo.dataSize;
                uwb_data_info_msg.data.insert(uwb_data_info_msg.data.end(), &dataInfo.data[0], &dataInfo.data[dataInfo.dataSize]);

                if(publishUwbInfo)
                    uwb_data_publisher.publish(uwb_data_info_msg);

                printf("DATAINFO: Time=%.4f\ttu = %zu\tant=%d\tIndex=%d\tID=%d\tData bytes: %d\t{",
                       uwb_data_info_msg.header.stamp.toSec(),
                       uwb_data_info_msg.uwb_time,
                       uwb_data_info_msg.antenna,
                       uwb_data_info_msg.source_idx + 1,
                       uwb_data_info_msg.source_id,
                       uwb_data_info_msg.data_size);
                for(int i = 0; i < uwb_data_info_msg.data_size-1; i++)
                    printf(KGRN "%02x ", uwb_data_info_msg.data[i]);
                printf("%02x" RESET, uwb_data_info_msg.data[uwb_data_info_msg.data_size-1]);
                printf("}\n");


            }
        }

        if(restEnable)
            sleepTime.sleep();
    }

    //Attempt to set the device to RCM
    rcmOpModeSet(RCRM_OPMODE_RCM);
    rcmIfClose();
}
