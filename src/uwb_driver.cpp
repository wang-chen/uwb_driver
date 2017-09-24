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

//Force configuration for nodes ID
#include <glob.h>

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
// P4xx Serial Communications
//_____________________________________________________________________________
#define     DFLT_P4xx_PORT          "/dev/ttyACM0"
#define     P4XX_ITF_SERIAL         0
#define     P4XX_ITF_USB            1
#define     P4XX_ITF_IP             2

#define     MODE_RCM                0
#define     MODE_RN                 1
#define     MODE_SLUMBER            2

extern int msgIdCount;

int status;
rcmIfType rcmIf;
rcmConfiguration rcmConfig;
rnConfiguration rnInitConfig;
rnTDMAConfiguration rnTDMAConfig;
rnMsg_GetTDMASlotmapConfirm rnInitTdmaSlotMap;
rnMsg_SetTDMASlotmapRequest initTdmaSlotMapSet;
// Info message structures
rcmMsg_FullRangeInfo rangeInfo;
rnMsg_GetFullNeighborDatabaseConfirm ndbInfo;
rcmMsg_DataInfo dataInfo;
uint8_t uwb_msg[RN_USER_DATA_LENGTH];

//To search for the device name
glob_t glob_results;

//_____________________________________________________________________________
//
// ID Management
//_____________________________________________________________________________
std::vector<int> nodesId;
uint8_t nodesTotal = 0;
std::vector<double>nodesPos;
static int32_t hostedP4xxId = -1;
static int32_t hostedP4xxIdx = -1;

int p4xxCurrMode = MODE_RCM;
int p4xxMode = MODE_RCM;
bool slumberTime = false;

/*------------------------------------------------------Function prototypes---------------------------------------------*/
bool uwb_mode_config(uwb_driver::uwbModeConfig::Request &req, uwb_driver::uwbModeConfig::Response &res)
{
    ros::Time startTime = ros::Time::now();
    int success = -1;

    //If mode is 2 we have to close the node
    if(req.p4xxmode == MODE_SLUMBER)
    {
        slumberTime = true;
        return true;
    }

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
            p4xxCurrMode = req.p4xxmode;
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
            confirm.status = ntohs(confirm.status);            

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
            confirm.status = ntohs(confirm.status);            

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
            confirm.status = ntohs(confirm.status);            

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
    if(uwbDriverNodeHandle.getParam("nodesId", nodesId))
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
    if(uwbDriverNodeHandle.getParam("nodesPos", nodesPos))
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
	
    //Collect the calibration numbers
    std::vector<std::vector<double>> rangeOffset;
    std::vector<double> rangeOffsetA;
    if(uwbDriverNodeHandle.getParam("rangeOffsetA", rangeOffsetA))
    {
        if(rangeOffsetA.size() != nodesTotal)
        {
            printf(KRED "Calibration locations not valid. Initialize with zeros.\n" RESET);

            for(int i = 0; i < nodesTotal; i++)
                rangeOffsetA.push_back(0);
        }
        else
        {
            printf(KBLU "Retrieved calibration values for antennas A: \n" RESET);
            for(int i = 0; i < rangeOffsetA.size(); i++)
                printf(KBLU "\t%f", rangeOffsetA[i]);
            printf("\n" RESET);

        }
    }
    else
    {
        printf(KRED "rangeOffsetA not specified. Initialize with zeros!\n" RESET);

        for(int i = 0; i < nodesTotal; i++)
            rangeOffsetA.push_back(0);
    }

    std::vector<double> rangeOffsetB;
    if(uwbDriverNodeHandle.getParam("rangeOffsetB", rangeOffsetB))
    {
        if(rangeOffsetB.size() != nodesTotal)
        {
            printf(KRED "Calibration locations not valid. Initialize with zeros.\n" RESET);

            for(int i = 0; i < nodesTotal; i++)
                rangeOffsetB.push_back(0);
        }
        else
        {
            printf(KBLU "Retrieved calibration values for antennas A: \n" RESET);
            for(int i = 0; i < rangeOffsetB.size(); i++)
                printf(KBLU "\t%f", rangeOffsetB[i]);
            printf("\n" RESET);

        }
    }
    else
    {
        printf(KRED "rangeOffsetA not specified. Initialize with zeros!\n" RESET);

        for(int i = 0; i < nodesTotal; i++)
            rangeOffsetB.push_back(0);
    }

    rangeOffset.push_back(rangeOffsetA);
    rangeOffset.push_back(rangeOffsetB);

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

    if(uwbDriverNodeHandle.getParam("p4xxMode", p4xxMode))
    {
        switch(p4xxMode)
        {
        case MODE_RCM:
            printf(KBLU "Started with RCM mode\n" RESET);
            break;
        case MODE_RN:
            printf(KBLU "Started with RN mode\n" RESET);
            break;
        default:
            printf(KYEL "Not RCM or RN specific! Default is RCM.\n" RESET);
            p4xxMode = MODE_RCM;
            break;
            break;
        }

    }
    else
    {
        printf(KYEL "Couldn't retrieve param 'p4xxCurrMode'. Started with default mode RCM!\n" RESET);
        p4xxCurrMode = MODE_RCM;
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

    bool portFound = false;
    int firstUnmatchedPort = -1;
    string p4xxIdNum;

    string p4xxSerialPortAffix = string("/dev/serial/by-id/usb-1027_Time_Domain_P440_*-if00");
    string p4xxSerialPortFullName = string("/dev/serial/by-id/usb-1027_Time_Domain_P440_") + p4xxSerialPort + string("-if00");

    while(!portFound && ros::ok())
    {
        glob(p4xxSerialPortAffix.c_str(), 0, NULL, &glob_results);
        // printf("Port \"%s\" not found!\n"RESET, p4xxSerialPortFull.data());
        int matches = glob_results.gl_pathc;
        if (matches != 0)
        {
            printf("Ports found: \n");
            for(int i = 0; i < matches; i++)
            {
                if(strcmp(glob_results.gl_pathv[glob_results.gl_offs + i],
                    p4xxSerialPortFullName.data()) == 0)
                {
                    printf("\tMatched: %s\n", glob_results.gl_pathv[glob_results.gl_offs + i]);
                    globfree(&glob_results);
                    portFound = true;
                    break;
                }
                else
                {
                    printf(KYEL "\tPassed: %s\n" RESET, glob_results.gl_pathv[glob_results.gl_offs + i]);
                    if(firstUnmatchedPort == -1)
                        firstUnmatchedPort = i;
                }
				
            }
        }
        else
            printf("No ports available!\n");
		
		if(portFound)
		{
			p4xxIdNum = p4xxSerialPort;
            globfree(&glob_results);
			break;
		}
		else if(firstUnmatchedPort != -1)
        {
            p4xxSerialPortFullName = string(glob_results.gl_pathv[glob_results.gl_offs + firstUnmatchedPort]);
            //Erase the end
            p4xxSerialPortFullName.erase(p4xxSerialPortFullName.find("-if00"), 5);
            //get the ID number
            p4xxIdNum = p4xxSerialPortFullName.substr(p4xxSerialPortAffix.length()-6,
                                                            p4xxSerialPortFullName.length()-(p4xxSerialPortAffix.length()-1));
            printf("\tPort %s will be reconfigured to %s.\n", p4xxIdNum.data(), p4xxSerialPort.data());
            globfree(&glob_results);
            break;

        }                    
        ros::Duration(0.5).sleep();
    }
	


    printf("\nRCM Localization App\n\n");

    if(p4xxInterface == P4XX_ITF_SERIAL)
        rcmIf = rcmIfSerial;
    else if (p4xxInterface == P4XX_ITF_USB)
        rcmIf = rcmIfUsb;

    // initialize the interface to the P4xx
    if (rcmIfInit(rcmIf, &p4xxIdNum[0]) != OK)
    {
        printf("Initialization failed.\n");
        exit(0);
    }

    //Change to RCM mode directly to avoid all the unexpected confirm messages
    while(rcmOpModeSet(RCRM_OPMODE_RCM) != 0 && ros::ok())
    {
        printf("Time out waiting for opmode set.\n");
        if(!ignrTimeoutUwbInit)
            exit(0);
    }
    p4xxCurrMode = MODE_RCM;
    printf("RCM mode confirmed\n");

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
        if(!portFound)
        {
            rcmConfig.nodeId = stoi(p4xxSerialPort);

            while(rcmConfigSet(&rcmConfig) != 0 && ros::ok())
            {
                printf("Time out waiting for RCM_CONFIG_SET_CONFIRM.\n");
                if(!ignrTimeoutUwbInit)
                    exit(0);
            }
            printf("Node ID reconfigured successfully\n");

            rcmConfig.nodeId = -1;
            while(rcmConfigGet(&rcmConfig) != 0)
            {
                printf("Time out waiting for RCM_CONFIG_GET_CONFIRM.\n");
                if(!ignrTimeoutUwbInit)
                    exit(0);
            }
        }
    }

    hostedP4xxId = rcmConfig.nodeId;
    for (uint32_t i = 0; i < nodesId.size(); i++)
        if(hostedP4xxId == nodesId[i])
                hostedP4xxIdx = i;
    printf("Node ID: %d. Node Index: %d\n", hostedP4xxId, hostedP4xxIdx);

    if(autoConfigRn)
    {
        string smFileName = string("");
        if(uwbDriverNodeHandle.getParam("smFileName", smFileName))
            printf(KBLU "Retrieved value %s for param 'smFileName'!\n" RESET, smFileName.data());
        else
        {
            printf(KRED "Couldn't retrieve param 'smFileName', program closed!\n" RESET);
            return 0;
        }

        struct stat buf;
        if(stat(smFileName.c_str(), &buf) != -1)
            printf("\tCSV found: %s\n", smFileName.c_str());
        else
        {
            printf(KRED "\tCSV not found!\n" RESET);
            exit(-1);
        }

        ifstream smFile (smFileName.data());
        string line;

        //Ditch the first row of titles
        getline(smFile, line);

        uint8_t k = 0;
        while(!smFile.eof())
        {
            //Get the slot number
            getline(smFile, line, ',');
            if(line.length() == 0)
                break;

            cout << "# " << line << endl;
            initTdmaSlotMapSet.slots[k].slotNumber = atoi(line.c_str());

            getline(smFile, line, ',');
            cout << "rqsterId: " << line << endl;
            initTdmaSlotMapSet.slots[k].requesterId = atoi(line.c_str());

            getline(smFile, line, ',');
            cout << "responderId: " << line << endl;
            initTdmaSlotMapSet.slots[k].responderId = atoi(line.c_str());

            getline(smFile, line, ',');
            cout << "integrationIndex: " << line << endl;
            initTdmaSlotMapSet.slots[k].integrationIndex = atoi(line.c_str());

            getline(smFile, line, ',');
            cout << "codeChannel: " << line << endl;
            initTdmaSlotMapSet.slots[k].codeChannel = atoi(line.c_str());

            getline(smFile, line, ',');
            cout << "antennaMode: " << line << endl;
            if (strcmp(line.data(), "Antenna A") == 0)
                initTdmaSlotMapSet.slots[k].antennaMode = 0;
            else if (strcmp(line.data(), "Antenna B") == 0)
                initTdmaSlotMapSet.slots[k].antennaMode = 1;
            else if (strcmp(line.data(), "TX A / RX B") == 0)
                initTdmaSlotMapSet.slots[k].antennaMode = 2;
            else if (strcmp(line.data(), "TX B / RX A") == 0)
                initTdmaSlotMapSet.slots[k].antennaMode = 3;
            else if (strcmp(line.data(), "Toggle A/B") == 0)
                initTdmaSlotMapSet.slots[k].antennaMode = 128;
            else
                initTdmaSlotMapSet.slots[k].antennaMode = 0;

            initTdmaSlotMapSet.slots[k].flags = 0;
            getline(smFile, line, ',');
            cout << "rqstData: " << line << endl;
            if (strcmp(line.data(), "1") == 0)
                initTdmaSlotMapSet.slots[k].flags |= 0x02;
            
            getline(smFile, line, ',');
            cout << "rspdData: " << line << endl;
            if (strcmp(line.data(), "1") == 0)
                initTdmaSlotMapSet.slots[k].flags |= 0x04;

            //Slot type is range by default
            getline(smFile, line, ',');
            cout << "slotType: " << line << endl;
            initTdmaSlotMapSet.slots[k].slotType = 1;

            // Ditch the sleep entry
            getline(smFile, line, ',');
            cout << "sleep: " << line << endl;

            getline(smFile, line, ',');
            cout << "requestedDurationMicroseconds: " << line << endl;
            initTdmaSlotMapSet.slots[k].requestedDurationMicroseconds = atoi(line.c_str())*1000;

            // Ditch the calculated time entry
            getline(smFile, line, '\n');
            cout << "calc. requestedDurationMicroseconds: " << line << endl << endl;

            k++;
            initTdmaSlotMapSet.numSlots = k;
        }

        // Set TDMA slot map
        while (rnTdmaSlotMapSet(&initTdmaSlotMapSet) != 0 && ros::ok())
        {
            printf("Time out waiting for rnTdmaSlotMap confirm.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        // Set the NDB Neighbor Age arbitrarily high to keep nodes from dropping out
        rnInitConfig.maxNeighborAgeMs = 4294967295;

        // Set network mode to TDMA
        rnInitConfig.networkSyncMode = RN_NETWORK_SYNC_MODE_TDMA;

        //Range type for NDB = PRM;
        rnInitConfig.rnFlags &= ~(1<<2);

        // Autosend the range.
        rnInitConfig.autosendType = 0x01;

        if (rnConfigSet(&rnInitConfig) != 0)
        {
            printf("Time out waiting for rnConfig confirm.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        // Retrieve TDMA slot map
        while (rnTdmaSlotMapGet(&rnInitTdmaSlotMap) != 0 && ros::ok())
        {
            printf("Time out waiting for rnTdmaSlotMap confirm.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        if (rnTdmaConfigGet(&rnTDMAConfig) != 0)
        {
            printf("Time out waiting for rnConfig confirm.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        printf("TDMA Current config: Max rqst data %d, Max rspd data %d\n",
                    rnTDMAConfig.maxRequestDataSize, rnTDMAConfig.maxResponseDataSize);

        rnTDMAConfig.maxRequestDataSize = 40;
        rnTDMAConfig.maxResponseDataSize = 0;

        // Retrieve TDMA config
        while (rnTdmaConfigSet(&rnTDMAConfig) != 0 && ros::ok())
        {
            printf("Time out waiting for rnTdmaSlotMap confirm.\n");
            if(!ignrTimeoutUwbInit)
                exit(0);
        }

        for (uint32_t i = 0; i < rnInitTdmaSlotMap.numSlots; i++)
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


        //Change to RN if requested
        if(p4xxMode == MODE_RN)
        {
            // Make sure opmode is RN mode
            while(rcmOpModeSet(RCRM_OPMODE_RN) != 0 && ros::ok())
            {
                printf("Time out waiting for opmode set.\n");
                if(!ignrTimeoutUwbInit)
                    exit(0);
            }
            p4xxCurrMode = MODE_RN;
            printf("RN mode confirmed\n");
        }
    }

    // Set the P4xx to active mode
    if (rcmSleepModeSet(RCRM_SLEEP_MODE_ACTIVE) != 0)
    {
        printf("Time out waiting for sleep mode set.\n");
        if(!ignrTimeoutUwbInit)
            exit(0);
    }

    // Set the hosted Id number
    uwbDriverNodeHandle.setParam("/uwb/hostedP4xxId", hostedP4xxId);

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
        std::vector<double> ancsPosUpd;

        //Check if the positions have been updated
        if(uwbDriverNodeHandle.getParam("/uwb/ancsPosUpd", ancsPosUpd))
        {
            if(ancsPosUpd.size() == nodesPos.size() - 3)
            {
                for(int i = 0; i < ancsPosUpd.size(); i++)
                nodesPos[i] = ancsPosUpd[i];
            }
        }
        uint8_t msg_type = -1;
        //Check and find the coresponding index of the update
        int nodeIndex = -1;
        uint8_t msg_type_rt = 1;
        switch (msg_type_rt = rcmInfoGet(&rangeInfo, &dataInfo, &ndbInfo))//get distance
        {
        case RANGEINFO:
        {
            if(rangeInfo.precisionRangeMm < 75000)
            {
                for(int i = 0; i < nodesTotal; i++)
                    if((uint32_t)nodesId[i] == rangeInfo.responderId)
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
                if((uint32_t)nodesId[i] == dataInfo.sourceId)
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
                uint8_t rqstAnt = rangeInfo.antennaMode & 0x0F;
                uint8_t rspdAnt = (rangeInfo.antennaMode >> 4) & 0x0F;

                uwb_driver::UwbRange uwb_range_info_msg;
                uwb_range_info_msg.header = std_msgs::Header();
                uwb_range_info_msg.header.frame_id = "uwb";
                uwb_range_info_msg.header.stamp = ros::Time::now();
                uwb_range_info_msg.requester_id = hostedP4xxId;
                uwb_range_info_msg.requester_idx = hostedP4xxIdx;
                uwb_range_info_msg.responder_id = nodesId[nodeIndex];
                uwb_range_info_msg.responder_idx = nodeIndex;
                uwb_range_info_msg.requester_LED_flag = rangeInfo.reqLEDFlags;
                uwb_range_info_msg.responder_LED_flag = rangeInfo.respLEDFlags;
                uwb_range_info_msg.noise = rangeInfo.noise;
                uwb_range_info_msg.vPeak = rangeInfo.vPeak;
                uwb_range_info_msg.distance = rangeInfo.precisionRangeMm/1000.0 + rangeOffset[rqstAnt][hostedP4xxIdx] + rangeOffset[rspdAnt][nodeIndex];
                uwb_range_info_msg.distance_err = rangeInfo.precisionRangeErrEst/1000.0;
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

                printf("RANGEINFO:Time=%.4f\ttu = %zu\tant=%02x\tRqIDx=%d\tRqID=%d\tRSIDx=%d\tRSID=%d\td=%6.3f, de = %6.3f, dd = %6.3f, dde = %6.3f\trqLED = %02x\trsLED = %02x\tnoise = %d\tVp = %d\tsw=%d\tx=%6.2f\ty=%6.2f\tz=%6.2f\n",
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
                       uwb_range_info_msg.requester_LED_flag,
                       uwb_range_info_msg.responder_LED_flag,
                       uwb_range_info_msg.noise,
                       uwb_range_info_msg.vPeak,
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

                printf("DATAINFO: Time=%.4f\ttu = %zu\tant=0x%02x\tIndex=%d\tID=%d\tData bytes: %d\t{",
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

        if(slumberTime)
        {
            rcmIfClose();
            printf("uwb_driver recessing...!\n");
            ros::Duration oneMonthTime(2592000);
            oneMonthTime.sleep();
        }

        if(restEnable)
            sleepTime.sleep();
    }
    rcmIfClose();
}
