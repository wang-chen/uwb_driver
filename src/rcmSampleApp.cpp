//_____________________________________________________________________________
//
// Copyright 2011-2015 Time Domain Corporation
//
//
// rcmSampleApp.c
//
//   Sample code showing how to interface to P400 RCM module.
//
//   This code uses the functions in rcm.c to:
//      - make sure the RCM is awake and in the correct mode
//      - get the configuration from the RCM and print it
//      - get the status/info from the RCM and print it
//      - range to another RCM node
//      - broadcast that range in a data packet
//
// This sample can communicate with the RCM over Ethernet, the 3.3V serial port,
// or the USB interface (which acts like a serial port).
//
//_____________________________________________________________________________


//_____________________________________________________________________________
//
// #includes
//_____________________________________________________________________________

#define ROS_MASTER_URI		"http://localhost:11311"
#define ROS_ROOT		"/opt/ros/indigo/share/ros"
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
//#include <viconXbee/viconPoseMsg.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <sensor_msgs/Imu.h>
#include <tf/transform_broadcaster.h>
#include <boost/assign/list_of.hpp>
#include <string.h>
#include "rcmIf.h"
#include "rcm.h"

//ekf ert generated code
#include "ekf.h"
#include "trilat.h"

//_____________________________________________________________________________
//
// #defines
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

//#define DEFAULT_DEST_NODE_ID    301
#define
#define     TRILAT_TIMES        100

//_____________________________________________________________________________
//
// typedefs
//_____________________________________________________________________________


//_____________________________________________________________________________
//
// static data
//_____________________________________________________________________________

//initial conditions for the ekf
static ekfModelClass ekf_Obj;
static double ancs[12] =
{
    -3.0, 3.0, 3.0, -3.0,
    -3.0, -3.0, 3.0, 3.0,
    -1.78, -1.17, -1.31, -1.31
};
static double R = 0.2;
//inputs for the ekf
static double dists[4] = { 0, 0, 0, 0 };
static double deltat = 0;
static double imus = 0;
static unsigned int nodeId = 1;		//WARNING: MATLAB GENERATED FUNCTION USES INDEX 1 FOR 1ST ELEMENT
//output of the ekf
static double x_est[6];


//initial conditions for the trilaterator
static trilatModelClass trilat_Obj;
static double ancsTranspose[12] =
{
    -3.0, -3.0, -1.78,
    3.0, -3.0, -1.17,
    3.0, 3.0, -1.31,
    -3.0, 3.0, -1.31
};
static double tempDists[4] = { 0, 0, 0, 0 };//{ 7.51, 13.03, 12.69, 6.90 };
static double trilatPos[3] = { 0, 0, 0 };
static double initialPos[3];

//_____________________________________________________________________________
//
// local function prototypes
//_____________________________________________________________________________


//_____________________________________________________________________________
//
// main - sample app entry point
//_____________________________________________________________________________

int main(int argc, char *argv[])
{
	//Create ros handler to node
	ros::init(argc, argv, "uwb_localization");
    ros::NodeHandle rcmEkfNodeHandle("~");
    ros::Time timeStart, timeEnd;
    int destNodeId;
    int status;
    rcmIfType   rcmIf;
    rcmConfiguration config;
    rcmMsg_GetStatusInfoConfirm statusInfo;
    rcmMsg_FullRangeInfo rangeInfo;
    rcmMsg_DataInfo dataInfo;
    rcmMsg_ScanInfo scanInfo;
    rcmMsg_FullScanInfo fullScanInfo;

	printf("RCM Sample App\n\n");

    rcmIf = rcmIfUsb;

    //initialize P410 serial interface
    // initialize the interface to the RCM
    if (rcmIfInit(rcmIf, USB_COM_NUMBER) != OK)
    {
        printf("Initialization failed.\n");
        exit(0);
    }

    // Make sure RCM is awake
    if (rcmSleepModeSet(RCM_SLEEP_MODE_ACTIVE) != 0)
    {
        printf("Time out waiting for sleep mode set.\n");
        exit(0);
    }

    // Make sure opmode is RCM
    if (rcmOpModeSet(RCM_OPMODE_RCM) != 0)
    {
        printf("Time out waiting for opmode set.\n");
        exit(0);
    }

    // execute Built-In Test - verify that radio is healthy
    if (rcmBit(&status) != 0)
    {
        printf("Time out waiting for BIT.\n");
        exit(0);
    }

    if (status != OK)
    {
        printf("Built-in test failed - status %d.\n", status);
        exit(0);
    }
    else
    {
        printf("Radio passes built-in test.\n\n");
    }

    // retrieve config from RCM
    if (rcmConfigGet(&config) != 0)
    {
        printf("Time out waiting for config confirm.\n");
        exit(0);
    }

    // print out configuration
    printf("Configuration:\n");
    printf("\tnodeId: %d\n", config.nodeId);
    printf("\tintegrationIndex: %d\n", config.integrationIndex);
    printf("\tantennaMode: %d\n", config.antennaMode);
    printf("\tcodeChannel: %d\n", config.codeChannel);
    printf("\telectricalDelayPsA: %d\n", config.electricalDelayPsA);
    printf("\telectricalDelayPsB: %d\n", config.electricalDelayPsB);
    printf("\tflags: 0x%X\n", config.flags);
    printf("\ttxGain: %d\n", config.txGain);

    // retrieve status/info from RCM
    if (rcmStatusInfoGet(&statusInfo) != 0)
    {
        printf("Time out waiting for status info confirm.\n");
        exit(0);
    }

    // print out status/info
    printf("\nStatus/Info:\n");
    printf("\tPackage version: %s\n", statusInfo.packageVersionStr);
    printf("\tRCM version: %d.%d build %d\n", statusInfo.appVersionMajor,
        statusInfo.appVersionMinor, statusInfo.appVersionBuild);
    printf("\tUWB Kernel version: %d.%d build %d\n", statusInfo.uwbKernelVersionMajor,
        statusInfo.uwbKernelVersionMinor, statusInfo.uwbKernelVersionBuild);
    printf("\tFirmware version: %x/%x/%x ver %X\n", statusInfo.firmwareMonth,
        statusInfo.firmwareDay, statusInfo.firmwareYear,
        statusInfo.firmwareVersion);
    printf("\tSerial number: %08X\n", statusInfo.serialNum);
    printf("\tBoard revision: %c\n", statusInfo.boardRev);
    printf("\tTemperature: %.2f degC\n\n", statusInfo.temperature / 4.0);

    //initialize trilateration object
    trilat_Obj.initialize();

    //Reset the intial position
    for (int i = 0; i < 3; i++)
    {
        trilatPos[i] = 0;
        initialPos[i] = 0;
    }

    //get the initial position by multiple trilaterations
    for (int i = 0; i < TRILAT_TIMES; i++)
    {
        //do the trilateration everytime a quadruple of distances is aquired
        for (int nodeId = 1; nodeId < 5; nodeId++)
        {
            //interpreting nodeID to anchor's ID
            switch (nodeId)
            {
            case 1:
                destNodeId = 101;
                break;
            case 2:
                destNodeId = 102;
                break;
            case 3:
                destNodeId = 105;
                break;
            case 4:
                destNodeId = 106;
                break;
            default:
                break;
            }
            rcmRangeTo(destNodeId, RCM_ANTENNAMODE_TXA_RXA, 0, NULL, &rangeInfo, &dataInfo, &scanInfo, &fullScanInfo);
            if ((rangeInfo.rangeMeasurementType & RCM_RANGE_TYPE_PRECISION) & (rangeInfo.precisionRangeMm < 15000))
                tempDists[nodeId - 1] = rangeInfo.precisionRangeMm / 1000.0; //Range measurements are in mm
            //if any ranging fails, restart from the 1st range
            else
            {
                nodeId = 1;
                continue;
            }
        }
        printf("iteration #%d, d1 = %f; d2 = %f; d3 = %f; d4 = %f\n", i, tempDists[0], tempDists[1], tempDists[2], tempDists[3]);
        //add up the measurements to cancel noise
        for (int j = 0; j < 4; j++)
            dists[j] += tempDists[j];
        //dists[0] += tempDists[0];
        //dists[1] += tempDists[1];
        //dists[2] += tempDists[2];
        //dists[3] += tempDists[3];
    }

    //averaging the measurements to get the initial position for the ekf
    for (int j = 0; j < 4; j++)
        dists[j] /= TRILAT_TIMES;
    printf("average ranges , d1 = %f; d2 = %f; d3 = %f; d4 = %f\n", dists[0], dists[1], dists[2], dists[3]);

    //trilatering
    trilat_Obj.step(ancs, dists, initialPos);

    printf(" init pos: x = %f, y = %f, z = %f\n", initialPos[0], initialPos[1], initialPos[2]);

    //Assign initial positions and anchor locations to the ekf
    for (int i = 0; i < 12; i++)
        ekf_Obj.ekf_P.ancs_InitialValue[i] = ancsTranspose[i];
    for (int i = 0; i < 3; i++)
        ekf_Obj.ekf_P.EKF_x_hat0[i] = initialPos[i];
    //initialize ekf object
    ekf_Obj.initialize();

    nodeId = 1;
    bool lastRangingSuccesful = true;
    int loopCount = 0;
    int faultyRangingCount = 0;

    // enter loop ranging to a node and broadcasting the resulting range
    ros::Rate rate(20);
    while(ros::ok())
    {
        if (loopCount == 200)
        {
            loopCount = 0;
            faultyRangingCount = 0;
            lastRangingSuccesful = true;
        }
        else
            loopCount++;

        //reset timer if last ranging was successful
        if (lastRangingSuccesful)
            timeStart = ros::Time::now();
        else
            faultyRangingCount++;

        switch (nodeId)
        {
        case 1:
            destNodeId = 101;
            break;
        case 2:
            destNodeId = 102;
            break;
        case 3:
            destNodeId = 105;
            break;
        case 4:
            destNodeId = 106;
            break;
        default:
            break;
        }

        //get the range to an anchor
        rcmRangeTo(destNodeId, RCM_ANTENNAMODE_TXA_RXA, 0, NULL, &rangeInfo, &dataInfo, &scanInfo, &fullScanInfo);
        //if measurement succeeds proceed to the ekf, otherwise move to the next anchor
        if ((rangeInfo.rangeMeasurementType & RCM_RANGE_TYPE_PRECISION) & (rangeInfo.precisionRangeMm < 15000))
        {
            lastRangingSuccesful = true;
            if (rangeInfo.precisionRangeMm < 0)
            {
                printf("\n negative range value, abort mission!");
                getchar();
                exit(0);
            }
            dists[nodeId - 1] = rangeInfo.precisionRangeMm / 1000.0; //Range measurements are in mm

            //calculate deltat
            timeEnd = ros::Time::now();
            deltat = (timeEnd.sec - timeStart.sec) + (timeEnd.nsec - timeStart.nsec)*1e-6;

            //step the model
            ekf_Obj.step(dists, deltat, 0, nodeId, x_est);

            if (x_est)
                //if no error, print the estimated location to the screen, else exit program
            if (rtmGetErrorStatus(ekf_Obj.getRTM()) == (NULL))
            {
                //Print the state to screen
                printf("                                                                                                                                               \r");
                printf("State vector: x = %7.4f; y = %7.4f; z = %7.4f; p1 = %7.4f; p2 = %7.4f; p3 = %7.4f, deltat = %7.4f, faultRate = %2.2f\r",
                    x_est[0], x_est[1], x_est[2],
                    ekf_Obj.ekf_B.P_pre[0],
                    ekf_Obj.ekf_B.P_pre[7],
                    ekf_Obj.ekf_B.P_pre[14],
                    deltat,
                    (faultyRangingCount/(double)loopCoun)*100
                    );
            }
            else
            {
                printf("Error in ekf model, abandon operation! Press any key to exit.");
                getchar();
                break;
            }
        }
        else
        {
            //Raise this flag to not to reset timer on next loop
            lastRangingSuccesful = false;
            faultyRangingCount++;
        }

        nodeId++;
        if (nodeId > 4)
            nodeId = 1;

        rate.sleep();
    }

    //cleanup
    ekf_Obj.terminate();
    trilat_Obj.terminate();
    getchar();
    rcmIfClose();
    return 0;
}

