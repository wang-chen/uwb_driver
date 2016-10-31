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

#include "uwb_driver/uwb_info.h"  //self-defined msg file

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
// Logging declarations
//_____________________________________________________________________________
bool logEnable = false;

std::ofstream fout;

//_____________________________________________________________________________
//
// ID Management
//_____________________________________________________________________________

//localization data
std::vector<double> ancsPos;
std::vector<double> albega;
std::vector<int>    ancsId;
uint8_T ancsTotal = 0;
std::vector<int>    mobsId;
uint8_T mobsTotal = 0;

//_____________________________________________________________________________
//
// Timestamps Management
//_____________________________________________________________________________

double uwbStartTime;
uint32_T uwbStartTimeInt32 = -1;
double localStartTime;
double uwbLastUpdateTime;
static uint8_T nodeId = 1;		//WARNING: MATLAB GENERATED FUNCTION USES INDEX 1 FOR 1ST ELEMENT

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

/*------------------------------------------------------Function prototypes---------------------------------------------*/
double getLocalTimeNow();

uint32_T rcmGetTime();
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
    ros::Publisher uwb_publisher;

    /*-----------------------------------------------Mischelaneous-----------------------------------------------------------*/
    bool restEnable = true;
    if(uwbDriverNodeHandle.getParam("restEnable", restEnable))
    {
        if(restEnable)
            printf(KBLU "uwb node allowed to sleep!\n" RESET);
        else
            printf(KBLU "not resting at all!\n" RESET);
    }
    else
    {
        printf(KRED "Couldn't retrieve param 'restEnable'. Sleep for 16ms after getting a range." RESET);
        restEnable = true;
    }

    //Collect the mobile node IDs
    if(uwbDriverNodeHandle.getParam("mobsId", mobsId))
    {
        mobsTotal = mobsId.size();
        printf(KBLU "Retrieved %d mobile IDs: ", mobsTotal);
        for(uint32_T i = 0; i < mobsId.size(); i++)
            printf("%d ", mobsId[i]);
        printf("\n" RESET);
    }
    else
    {
        printf(KRED "Failed to collect mobile IDs, terminating program!\n" RESET);
        return 0;
    }

    //Collect the anchors IDs
    if(uwbDriverNodeHandle.getParam("ancsId", ancsId))
    {
        ancsTotal = ancsId.size();
        printf(KBLU "Retrieved %d anchor IDs: ", ancsTotal);
        for (int i = 0; i < ancsTotal; i++)
            printf("%d\t", ancsId[i]);
        printf("\n" RESET);
    }
    else
    {
        printf(KRED "Failed to collect anchor IDs, terminating program!\n" RESET);
        return 0;
    }

    //Collect the anchors Positions
    if(uwbDriverNodeHandle.getParam("ancsPos", ancsPos))
    {
        if( (ancsPos.size() / (double)ancsTotal) != 3)
        {
            printf(KRED "anchors positions not whole. Exitting\n" RESET);
            return 0;
        }
        else
        {
            printf(KBLU "Retrieved %d anchor cordinates:\n" RESET, ancsTotal);

            for(int i = 0; i < ancsTotal; i++)
            {
                printf(KBLU "\ta%d = ", ancsId[i]);
                for( int j = 0; j < 3; j++)
                    printf("%4.2f\t", ancsPos[i*3 + j]);
                printf("\n" RESET);
            }
        }
    }
    else
    {
        printf(KRED "Failed to collect anchor IDs, terminating program!\n" RESET);
        return 0;
    }

    int mode = MODE_RN;
    //check for the mode selected
    if(uwbDriverNodeHandle.getParam("mode", mode))
        switch((char)mode)
        {
        case 'u':
            printf(KBLU "Normal mode selected.\n" RESET);
            break;
        default:
            printf(KBLU "Default mode selected.\n" RESET);
            break;
        }
    else
    {
        printf(KRED "Failed to select uwb operating mode, terminating program!\n" RESET);
        return 0;
    }

    //Redundant
    int p44QueryRate = DFLT_NODE_RATE;
    if(uwbDriverNodeHandle.getParam("p44QueryRate", p44QueryRate))
        printf(KBLU "Retrieved value %d for param 'p44QueryRate'\n" RESET, p44QueryRate);
    else
        printf(KYEL "Couldn't retrieve param 'p44QueryRate', applying default value %dHz\n" RESET, p44QueryRate);

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
            uwb_publisher = uwbDriverNodeHandle.advertise<uwb_driver::uwb_info> ("uwb_info", 0);
        }

        else
            printf(KBLU "Retrieved value 'false' for param 'publishUwbInfo'!\n" RESET);
    }
    else
    {
        printf(KYEL "Couldn't retrieve param 'publishUwbInfo', program will proceed without publishing uwb poses!\n" RESET);
        publishUwbInfo = false;
    }

    bool enableVicon = false;
    if(uwbDriverNodeHandle.getParam("enableVicon", enableVicon))
    {
        if(enableVicon)
            printf(KBLU "Retrieved value 'true' for param 'enableVicon'!\n" RESET);

        else
            printf(KBLU "Retrieved value 'false' for param 'enableVicon'!\n" RESET);
    }
    else
    {
        printf(KYEL "Couldn't retrieve param 'enableVicon', program will proceed without publishing uwb poses to enable vicon!\n" RESET);
        enableVicon = false;
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

    bool autoConfig = true;
    if(uwbDriverNodeHandle.getParam("autoConfig", autoConfig))
        if(autoConfig)
            printf(KBLU "Retrieved value 'true' for param 'autoConfig'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'autoConfig'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'autoConfig'. Enforcing autoconfiguration from payload!\n" RESET);
        autoConfig = false;
    }

    //initialize P4xx serial interface
    {
        printf("\nRCM Localization App\n\n");

        if(p4xxInterface == P4XX_ITF_SERIAL)
            rcmIf = rcmIfSerial;
        else if (p4xxInterface == P4XX_ITF_USB)
            rcmIf = rcmIfUsb;

        // initialize the interface to the RCM
        if (rcmIfInit(rcmIf, &p4xxSerialPort[0]) != OK)
        {
            printf("Initialization failed.\n");
            if(!ignrTimeoutUwbInit)
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

        if(autoConfig)
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

            //check for device node ID
            int32_T uwbNodeId = rcmConfig.nodeId;

            //search for index in the mobile profile
            nodeId = 255;

            for (uint32_T i = 0; i < mobsId.size(); i++)
                if(uwbNodeId == mobsId[i])
                    nodeId = i;

            if(nodeId == 255)
            {
                printf("Node not involved! Exiting\n");
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
    /*-----------------------------------------P4xx interface configuration---------------------------------------------------*/


    /*-------------------------------------------Logging Initialization---------------------------------------------------------*/
    if(uwbDriverNodeHandle.getParam("logEnable", logEnable))
    {
        if(logEnable)
            printf(KBLU "Logging enabled!\n" RESET);
        else
            printf(KBLU "Logging disabled!\n" RESET);
    }
    else
    {
        printf(KRED "Couldn't retrieve param 'logEnable'. Not used by default." RESET);
        logEnable = false;
    }


    struct passwd *pw = getpwuid(getuid());
    std::string logFileName;
    if(logEnable)
    {
        logFileName = string(pw->pw_dir) + std::string("/uwb_log/dists.csv");
        printf("%s\n", logFileName.data());
        fout.open(logFileName.data());
    }
    /*-------------------------------------------Logging Initialization---------------------------------------------------------*/


    /*-------------------------------------------------Initial Trilateration----------------------------------------------------*/

    ros::Rate rate(p44QueryRate);
    //get the initial position by trilaterating the average
    while(ros::ok())
    {
        //Check and find the coresponding index of the update
        int indexUpdate = -1;
        double newRange = -1.0;
        switch (rcmInfoGet(&rangeInfo, &ndbInfo))//get distance
        {
        case RANGEINFO:
        {
            if(rangeInfo.precisionRangeMm < 75000)
                for(int i = 0; i < ancsTotal; i++)
                    if((uint32_T)ancsId[i] == rangeInfo.responderId)
                    {
                        indexUpdate = i;
                        newRange = rangeInfo.precisionRangeMm/1000.0;
                        break;
                    }
        }

        default:
            break;

        }

        printf("ID: %d\tRange: %f\n", indexUpdate+1, newRange);
        uwb_driver::uwb_info uwb_info_msg;

        uwb_info_msg.distance = newRange;

        uwb_info_msg.responder_location.x = ancsPos[nodeId*3];
        uwb_info_msg.responder_location.y = ancsPos[nodeId*3+1];
        uwb_info_msg.responder_location.z = ancsPos[nodeId*3+2];

        uwb_info_msg.responder_id = ancsId[nodeId];
        uwb_info_msg.responder_idx = nodeId;
        printf("requester node location: [%.3f  %.3f  %.3f]\n", uwb_info_msg.responder_location.x, uwb_info_msg.responder_location.y, uwb_info_msg.responder_location.z);
        printf("distance to node %d: %.3f\n", ancsId[nodeId], newRange);
        printf("---\n");

        uwb_publisher.publish(uwb_info_msg);
        rate.sleep();
    }
}
