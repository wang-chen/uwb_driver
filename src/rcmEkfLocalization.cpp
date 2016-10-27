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
// #include <uwb_localization/rcmEkfStateMsg.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <sensor_msgs/Imu.h>
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

//For log file use
#include <fstream>
#include <pwd.h>

//For i2c communication
#include <linux/i2c-dev.h>
//#include "i2cfunc.h"

//MIT serial library
// #include <serial/serial.h>
#include "ros/ros.h"
#include "time.h"
#include <arpa/inet.h>

#include "mesh_nodes.h"
#include "mesh_nodes_types.h"

#include <thread>
#include <mutex>

//Real time IMU library
// #include "RTIMULib.h"
// #include "RTIMUSettings.h"

// ----------------------------------------------------------------LINHAO----------------------------------------------------------------
#include "uwb_localization/uwbdata.h"  //self-defined msg file

//// Needed when mixing C and C++ code/libraries
#ifdef __cplusplus
    extern "C" {
#endif
        #include "mpu9250.h"
        #include "linux_glue.h"
        #include "local_defaults.h"
        #include "inv_mpu_dmp_motion_driver.h"
#ifdef __cplusplus
    }
#endif

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

#define DFLT_NODE_RATE      10

//_____________________________________________________________________________
//
// Logging declarations
//_____________________________________________________________________________
bool logEnable = false;
bool logOutput = false;
bool logInput = false;

std::ofstream fout;
std::ofstream imuLog;
std::ofstream fccLog;

//_____________________________________________________________________________
//
// Mischelaneous declarations
//_____________________________________________________________________________

//Timer
#define CLOCKID CLOCK_REALTIME
#define SIG SIGUSR1
timer_t timerid;

struct sigevent sev;
struct itimerspec its;
struct sigaction sa;

std::vector<geometry_msgs::PoseStamped> uwbPoses;
uint32_T uwbPoseCount = 0;
//_____________________________________________________________________________
//
// Imu handling variables declarations
//_____________________________________________________________________________
// RTIMU *imuObj = NULL;
#define G_2_MPSS    10.3006864106026
#define uT_2_T      1000000

std::vector< std::vector<double> > imuStampedSeq;


long imuUsed = 0;

//body to local orientation quaternion
quaternion_t qBL = {1, 0, 0, 0};

//offset orientation quaternion
quaternion_t qID0 = {1, 0, 0, 0};

//_____________________________________________________________________________
//
// Trilateration and ekf particulars
//_____________________________________________________________________________
//ekf ert generated code
#include "trilatCalib.h"
#include "ekf_iod.h"
#include "nlsTRI345.h"

//initial conditions for the trilaterator
bool trilaterating = true;

//initial conditions for the ekf
static ekfIodClass ekf_Obj;

//localization data
std::vector<double> ancsPos;
std::vector<double> albega;
std::vector<int>    ancsId;
uint8_T ancsTotal = 0;
std::vector<int>    mobsId;
uint8_T mobsTotal = 0;
std::vector<double> ekfCovXyZ;

static double R = 0.2;              //Measurement covariance ~ maximum change
static double deltat = 0;
static float fcc_x = 0, fcc_y = 0, fcc_z = 0, fcc_xd = 0, fcc_yd = 0, fcc_zd = 0;
static unsigned long long fccTimestamp = 0;
static unsigned long long fccFirstTimestamp = 0;

double uwbStartTime;
uint32_T uwbStartTimeInt32 = -1;
double localStartTime;
double uwbLastUpdateTime;

static uint8_T nodeId = 1;		//WARNING: MATLAB GENERATED FUNCTION USES INDEX 1 FOR 1ST ELEMENT
//output of the ekf
static double x_est[6], x_prev[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static double x_tri[3] = {0.0, 0.0, 0.0};
static double P_est[36], P_prev[36] = { 0.5, 0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.5, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.5, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.5, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.0, 0.5
                                      };
static double trilatPos[3] = { 0, 0, 0 };
static double initialPos[3];

// double imuInitVect[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

//_____________________________________________________________________________
//
// P410 Serial Communications
//_____________________________________________________________________________
#define     DFLT_P410_PORT          "/dev/ttyACM0"
#define     P4XX_ITF_SERIAL         0
#define     P4XX_ITF_USB            1
#define     P4XX_ITF_IP             2

#define     LOSS_MEMORY_LENGTH      20
#define     DFLT_MAX_ATDNS_CALLS    20

uint8_T     lossMarkBuff[LOSS_MEMORY_LENGTH];
uint8_T     lossMarkBuffIndex = 0;
uint8_T     loss = 100;

extern int msgIdCount;

static uint8_T u[6] = {0, 0, 0, 0, 0};
static uint8_T a[6] = {0, 0, 0, 0, 0};
static uint8_T smU[576];
static uint8_T smA[576];

int status;
rcmIfType rcmIf;
rcmConfiguration rcmConfig;
rnConfiguration rnInitConfig;
rnMsg_GetTDMASlotmapConfirm rnInitTdmaSlotMap;
rnMsg_SetTDMASlotmapRequest initTdmaSlotMapSet;
// Info message structures
rcmMsg_FullRangeInfo rangeInfo;
rnMsg_GetFullNeighborDatabaseConfirm ndbInfo;

//_____________________________________________________________________________
//
// Lidar I2C Communications
//_____________________________________________________________________________
//_____________________________________________________________________________
//
// FCC Serial Communications
//_____________________________________________________________________________
#define     DFLT_FCC_PORT           "/dev/ttyO1"
#define     BAUD_MACRO              B115200
#define     USE_MIT_SERIAL

#define F2U00_MSG_LENGTH            38
#define F2U01_MSG_LENGTH            58
#define F2U_MSG_MAX_LENGTH          F2U01_MSG_LENGTH
#define F2U_MSG_MIN_LENGTH          F2U00_MSG_LENGTH


static uint8_T fcc2UwbBuff[F2U_MSG_MAX_LENGTH];

//Frame's initials
#define F2U_HEADER1                 (*(uint8_T *)(fcc2UwbBuff + 0)) //'F'
#define F2U_HEADER2                 (*(uint8_T *)(fcc2UwbBuff + 1)) //'C'
#define F2U_ID                      (*(uint8_T *)(fcc2UwbBuff + 2)) //0x00 or 0x01
#define F2U_LENGTH_BYTE             (*(uint8_T *)(fcc2UwbBuff + 3)) //F2U00_DATA_LENGTH or F2U01_DATA_LENGTH (32 or 52)

//Frame content msg 0x00
#define F2U00_X                     (*(float *)(fcc2UwbBuff + 4))
#define F2U00_Y                     (*(float *)(fcc2UwbBuff + 8))
#define F2U00_Z                     (*(float *)(fcc2UwbBuff + 12))
#define F2U00_XD                    (*(float *)(fcc2UwbBuff + 16))
#define F2U00_YD                    (*(float *)(fcc2UwbBuff + 20))
#define F2U00_ZD                    (*(float *)(fcc2UwbBuff + 24))
#define F2U00_TIMESTAMP             (*(unsigned long long *)(fcc2UwbBuff + 28))
#define F2U00_CS1                   (*(uint8_T *)(fcc2UwbBuff + 36))
#define F2U00_CS2                   (*(uint8_T *)(fcc2UwbBuff + 37))

#define F2U00_SUMCHECK_LENGTH       34
#define F2U00_DATA_LENGTH           32

//Frame content msg 0x01
#define F2U01_ANCID1                (*(uint8_T *)(fcc2UwbBuff + 4))
#define F2U01_ANCX1                 (*(float *)(fcc2UwbBuff + 5))
#define F2U01_ANCY1                 (*(float *)(fcc2UwbBuff + 9))
#define F2U01_ANCZ1                 (*(float *)(fcc2UwbBuff + 13))
#define F2U01_ANCID2                (*(uint8_T *)(fcc2UwbBuff + 17))
#define F2U01_ANCX2                 (*(float *)(fcc2UwbBuff + 18))
#define F2U01_ANCY2                 (*(float *)(fcc2UwbBuff + 22))
#define F2U01_ANCZ2                 (*(float *)(fcc2UwbBuff + 26))
#define F2U01_ANCID3                (*(uint8_T *)(fcc2UwbBuff + 30))
#define F2U01_ANCX3                 (*(float *)(fcc2UwbBuff + 31))
#define F2U01_ANCY3                 (*(float *)(fcc2UwbBuff + 35))
#define F2U01_ANCZ3                 (*(float *)(fcc2UwbBuff + 39))
#define F2U01_ANCID4                (*(uint8_T *)(fcc2UwbBuff + 43))
#define F2U01_ANCX4                 (*(float *)(fcc2UwbBuff + 44))
#define F2U01_ANCY4                 (*(float *)(fcc2UwbBuff + 48))
#define F2U01_ANCZ4                 (*(float *)(fcc2UwbBuff + 52))

#define F2U01_CS1                   (*(uint8_T *)(fcc2UwbBuff + 56))
#define F2U01_CS2                   (*(uint8_T *)(fcc2UwbBuff + 57))

#define F2U01_SUMCHECK_LENGTH       54
#define F2U01_DATA_LENGTH           52

uint8_T F2U01_ANCID(int id)
{
    return (*(uint8_T *)(fcc2UwbBuff + 4 + 13*id));
}
float F2U01_ANCPOS(int id, int var)
{
    return (*(float *)(fcc2UwbBuff + 5 + 13*id + 4*var));
}


#define U2F00_MSG_LENGTH            32
#define U2F01_MSG_LENGTH            58
#define U2F_MSG_MAX_LENGTH          U2F01_MSG_LENGTH

static uint8_T uwb2FccBuff[U2F_MSG_MAX_LENGTH];
#define U2F00_HEADER1               (*(uint8_T *)(uwb2FccBuff + 0)) //'U'
#define U2F00_HEADER2               (*(uint8_T *)(uwb2FccBuff + 1)) //'W'

#define U2F00_ID                    (*(uint8_T *)(uwb2FccBuff + 2))
#define U2F00_LENGTH_BYTE           (*(uint8_T *)(uwb2FccBuff + 3))
#define U2F00_X                     (*(float *)(uwb2FccBuff + 4))
#define U2F00_Y                     (*(float *)(uwb2FccBuff + 8))
#define U2F00_Z                     (*(float *)(uwb2FccBuff + 12))
#define U2F00_DX                    (*(float *)(uwb2FccBuff + 16))
#define U2F00_DY                    (*(float *)(uwb2FccBuff + 20))
#define U2F00_DZ                    (*(float *)(uwb2FccBuff + 24))
#define U2F00_NODECOUNT             (*(uint8_T *)(uwb2FccBuff + 28))
#define U2F00_LOSS                  (*(uint8_T *)(uwb2FccBuff + 29))

#define U2F00_CS1                   (*(uint8_T *)(uwb2FccBuff + 30))
#define U2F00_CS2                   (*(uint8_T *)(uwb2FccBuff + 31))
#define U2F00_SUMCHECK_LENGTH       28
#define U2F00_DATA_LENGTH           26

#define	UP				0xFF
#define DOWN			0x00
//#define RCV_THRESHOLD   40
uint8_T		msgFlag = DOWN;

//declare the file descriptors for FCC
// serial::Serial fd;
int reportRate= 5;

/*------------------------------------------------------Function prototypes---------------------------------------------*/
void timerCallback();

void bufferCheck(std::ofstream& fccLog, bool logEnable);

double getLocalTimeNow();

uint32_T rcmGetTime();

void imuCheck();
/*------------------------------------------------------Function prototypes---------------------------------------------*/

//_____________________________________________________________________________
//
// main function
//_____________________________________________________________________________
using namespace std;

int main(int argc, char *argv[])
{
    //Create ros handler to node
    ros::init(argc, argv, "uwb_localization");
    ros::NodeHandle rcmEkfNodeHandle("~");
    ros::NodeHandle n("~");   
    ros::Publisher uwbPathPublisher, uwbPosePublisher;
    nav_msgs::Path uwbPathMsg;

// -----------------------------------LINHAO---------------------------------------------------------------------------------
    ros::Publisher lh_publisher = n.advertise<uwb_localization::uwbdata> ("uwb_distance", 0);

    std::vector<double> node_pos;    
    n.getParam("/node_pos", node_pos);

    double distance_lh;
    geometry_msgs::Point position;
    ros::Rate loop_rate(40);
    /*-----------------------------------------------Mischelaneous-----------------------------------------------------------*/
    bool restEnable = true;
    if(rcmEkfNodeHandle.getParam("restEnable", restEnable))
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

    //Reporting rate for the fcc
    if(rcmEkfNodeHandle.getParam("reportRate", reportRate))
        printf(KBLU "Retrieved value %d for param 'reportRate'!\n" RESET, reportRate);
    else
    {
        reportRate = 5;
        printf(KYEL "Couldn't retrieve param 'reportRate', or value is invalid (too large). Using default rate %dHz!\n" RESET, reportRate);
    }

    //Trilateration loops
    int TRILAT_TIMES = 50;
    if(rcmEkfNodeHandle.getParam("TRILAT_TIMES", TRILAT_TIMES))
        printf(KBLU "Retrieved value %d for param 'TRILAT_TIMES'!\n" RESET, TRILAT_TIMES);
    else
    {
        TRILAT_TIMES = 50;
        printf(KYEL "Couldn't retrieve param 'TRILAT_TIMES'. Using default value %d times!\n" RESET, TRILAT_TIMES);
    }

    //Trilateration error threshold
    double errThres = 0.01;
    if(rcmEkfNodeHandle.getParam("errThres", errThres))
        printf(KBLU "Retrieved value %f for param 'errThres'!\n" RESET, errThres);
    else
    {
        errThres = 0.01;
        printf(KYEL "Couldn't retrieve param 'errThres'. Using default value %f!\n" RESET, errThres);
    }

    //Trilateration time window
    int timeWindow = 150;
    if(rcmEkfNodeHandle.getParam("timeWindow", timeWindow))
        printf(KBLU "Retrieved value %d for param 'timeWindow'!\n" RESET, timeWindow);
    else
    {
        timeWindow = 150;
        printf(KYEL "Couldn't retrieve param 'timeWindow'. Using default value %d ms!\n" RESET, timeWindow);
    }

    //Collect the mobile node IDs
    if(rcmEkfNodeHandle.getParam("mobsId", mobsId))
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
    if(rcmEkfNodeHandle.getParam("ancsId", ancsId))
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
    if(rcmEkfNodeHandle.getParam("ancsPos", ancsPos))
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

    // Variables to monitor communication
    uint8_T     admittedRange = 0;
    uint8_T     admittedRangeChecklist[ancsTotal];
    memset(admittedRangeChecklist, 0, sizeof(admittedRangeChecklist));

    uint8_T     onlineNodesComm = 0;
    uint8_T     onlineNodesCommChecklist[ancsTotal];
    memset(onlineNodesCommChecklist, 0, sizeof(onlineNodesCommChecklist));

    int mode = 'u';
    //check for the mode selected
    if(rcmEkfNodeHandle.getParam("mode", mode))
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
        printf(KRED "Failed to select uwb op mode, terminating program!\n" RESET);
        return 0;
    }

    //Redundant
    int rcmEkfRate = DFLT_NODE_RATE;
    if(rcmEkfNodeHandle.getParam("rcmLocalizationRate", rcmEkfRate))
        printf(KBLU "Retrieved value %d for param 'rcmLocalizationRate'\n" RESET, rcmEkfRate);
    else
        printf(KYEL "Couldn't retrieve param 'rcmLocalizationRate', applying default value %dHz\n" RESET, rcmEkfRate);

    bool ignrTimeoutUwbInit = true;
    if(rcmEkfNodeHandle.getParam("ignrTimeoutUwbInit", ignrTimeoutUwbInit))
        if(ignrTimeoutUwbInit)
            printf(KBLU "Retrieved value 'true' for param 'ignrTimeoutUwbInit'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'ignrTimeoutUwbInit'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'ignrTimeoutUwbInit', program will proceed if devices init. fails!\n" RESET);
        ignrTimeoutUwbInit = true;
    }

    // double imuSettleTime = 20.0;
    // if(rcmEkfNodeHandle.getParam("imuSettleTime", imuSettleTime))
    //     printf(KBLU "Retrieved value %f for param 'imuSettleTime'!\n" RESET, imuSettleTime);
    // else
    // {
    //     imuSettleTime = 20.0;
    //     printf(KYEL "Couldn't retrieve param 'imuSettleTime'. Using default value %f!\n" RESET, imuSettleTime);
    // }

    bool ignrFailedMpuInit = true;
    if(rcmEkfNodeHandle.getParam("ignrFailedMpuInit", ignrFailedMpuInit))
        if(ignrFailedMpuInit)
            printf(KBLU "Retrieved value 'true' for param 'ignrFailedMpuInit'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'ignrFailedMpuInit'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'ignrFailedMpuInit', program will proceed if devices init. fails!\n" RESET);
        ignrFailedMpuInit = true;
    }

    int attendanceCycle = DFLT_MAX_ATDNS_CALLS;
    if(rcmEkfNodeHandle.getParam("attendanceCycle", attendanceCycle))
        printf(KBLU "Retrieved value %d for param 'attendanceCycle'\n" RESET, attendanceCycle);
    else
        printf(KYEL "Couldn't retrieve param 'attendanceCycle', applying default value %d\n" RESET, attendanceCycle);

    bool publishPose = false;
    uwbPathPublisher = rcmEkfNodeHandle.advertise<nav_msgs::Path>("/uwb_path", 1);
    uwbPosePublisher = rcmEkfNodeHandle.advertise<geometry_msgs::PoseStamped>("/uwb_pose", 1);
    if(rcmEkfNodeHandle.getParam("publishPose", publishPose))
    {
        if(publishPose)
            printf(KBLU "Retrieved value 'true' for param 'publishPose'!\n" RESET);

        else
            printf(KBLU "Retrieved value 'false' for param 'publishPose'!\n" RESET);
    }
    else
    {
        printf(KYEL "Couldn't retrieve param 'publishPose', program will proceed without publishing uwb poses!\n" RESET);
        publishPose = false;
    }

    bool enableVicon = false;
    if(rcmEkfNodeHandle.getParam("enableVicon", enableVicon))
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



    // /*-----------------------------------------FCC interface configuration---------------------------------------------------*/

    // //FCC interfacing
    // string fccSerialPort = string(DFLT_FCC_PORT);
    // //check for the FCC port name
    // if(rcmEkfNodeHandle.getParam("fccSerialPort", fccSerialPort))
    //     printf(KBLU "Retrieved value %s for param 'fccSerialPort'!\n" RESET, fccSerialPort.data());
    // else
    // {
    //     printf(KRED "Couldn't retrieve param 'fccSerialPort', program closed!\n" RESET);
    //     return 0;
    // }

    // //FCC interfacing
    // int fccBaudrate = 115200;
    // //check for the FCC baudrate
    // if(rcmEkfNodeHandle.getParam("fccBaudrate", fccBaudrate))
    //     printf(KBLU "Retrieved value %d for param 'fccBaudrate'!\n" RESET, fccBaudrate);
    // else
    // {
    //     printf(KRED "Couldn't retrieve param 'fccBaudrate', program closed!\n" RESET);
    //     return 0;
    // }

    // bool syncFCC = false;
    // if(rcmEkfNodeHandle.getParam("syncFCC", syncFCC))
    //     if(syncFCC)
    //         printf(KBLU "Retrieved value 'true' for param 'syncFCC'!\n" RESET);
    //     else
    //         printf(KBLU "Retrieved value 'false' for param 'syncFCC'!\n" RESET);
    // else
    // {
    //     printf(KYEL "Couldn't retrieve param 'syncFCC'. First imu timestamp can be zero!\n" RESET);
    //     syncFCC = false;
    // }


    // fd.setPort(fccSerialPort);
    // fd.setBaudrate(fccBaudrate);
    // fd.setTimeout(5, 10, 2, 10, 2);
    // fd.open();
    // if (fd.isOpen())
    // {
    //     fd.flushInput();
    //     printf(KBLU "Connection established\n\n" RESET);
    // }
    // else
    // {
    //     printf(KRED "serialInit: Failed to open port %s\n" RESET, fccSerialPort.data());
    //     return 0;
    // }

    /*------------------------------------------FCC interface configuration---------------------------------------------------*/



    /*-----------------------------------------P410 interface configuration---------------------------------------------------*/
    //P410 interface
    int p4xxInterface = P4XX_ITF_SERIAL;
    //check for the interface selected
    if(rcmEkfNodeHandle.getParam("p4xxInterfaceChoice", p4xxInterface))
        switch(p4xxInterface)
        {
        case    P4XX_ITF_SERIAL:
            printf(KBLU "P410 Serial interface selected!\n" RESET);
            break;
        case    P4XX_ITF_USB:
            printf(KBLU "P410 USB interface selected!\n" RESET);
            break;
        default:
            break;
        }
    else
    {
        printf(KRED "Failed to select communication interface, terminating program!\n" RESET);
        return 0;
    }

    string p4xxSerialPort = string(DFLT_P410_PORT);
    //check for the p4xx port's name
    if(rcmEkfNodeHandle.getParam("p4xxSerialPort", p4xxSerialPort))
        printf(KBLU "Retrieved value %s for param 'p4xxSerialPort'!\n" RESET, p4xxSerialPort.data());
    else
    {
        printf(KRED "Couldn't retrieve param 'p4xxSerialPort', program closed!\n" RESET);
        return 0;
    }

    bool autoConfig = true;
    if(rcmEkfNodeHandle.getParam("autoConfig", autoConfig))
        if(autoConfig)
            printf(KBLU "Retrieved value 'true' for param 'autoConfig'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'autoConfig'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'autoConfig'. Enforcing autoconfiguration from payload!\n" RESET);
        autoConfig = false;
    }

    //initialize P410 serial interface
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

//            do
//            {
//                uwbStartTimeInt32 = rcmGetTime();
//                localStartTime = getLocalTimeNow();
//            }while(uwbStartTimeInt32 == 4294967295);

//            if(uwbStartTimeInt32 == 4294967295)
//            {
//                printf("Error getting timestamp from uwb!");
//                if(!ignrTimeoutUwbInit)
//                    exit(0);
//            }
//            uwbStartTime = (double)uwbStartTimeInt32;

            uint8_T uCount = mobsId.size();
            uint8_T aCount = ancsId.size();

            for (uint32_T i = 0; i < uCount; i++)
                u[i] = (uint8_T)mobsId[i];

            for (uint32_T i = 0; i < aCount; i++)
                a[i] = (uint8_T)ancsId[i];

            uint8_T s[2] = {(uint8_T)ancsId[0], (uint8_T)ancsId[2]};

            uint8_T trueMapSize;

            mesh_nodes(u, uCount, a, aCount, s, 2, mode, smU, smA, &trueMapSize);

            initTdmaSlotMapSet.numSlots = trueMapSize;

            if((char)mode == 'u')
            {

                for(int k = 0; k < trueMapSize; k++)
                {
                    initTdmaSlotMapSet.slots[k].slotNumber = k;
                    initTdmaSlotMapSet.slots[k].requesterId = smU[k];
                    initTdmaSlotMapSet.slots[k].responderId = smU[k + 32];
                    initTdmaSlotMapSet.slots[k].integrationIndex = 7;
                    initTdmaSlotMapSet.slots[k].codeChannel = smU[k + 2*32];
                    initTdmaSlotMapSet.slots[k].antennaMode = 0;
                    initTdmaSlotMapSet.slots[k].flags = 0;
                    initTdmaSlotMapSet.slots[k].slotType = 1;
                    initTdmaSlotMapSet.slots[k].requestedDurationMicroseconds = 25000;
                }
            }

            else
            {

                for(int k = 0; k < trueMapSize; k++)
                {
                    initTdmaSlotMapSet.slots[k].slotNumber = k;
                    initTdmaSlotMapSet.slots[k].requesterId = smU[k + nodeId*32*3];
                    initTdmaSlotMapSet.slots[k].responderId = smU[k + 32 + nodeId*32*3];
                    initTdmaSlotMapSet.slots[k].integrationIndex = 7;
                    initTdmaSlotMapSet.slots[k].codeChannel = smU[k + 2*32 +nodeId*32*3];
                    initTdmaSlotMapSet.slots[k].antennaMode = 0;
                    initTdmaSlotMapSet.slots[k].flags = 0;
                    initTdmaSlotMapSet.slots[k].slotType = 1;
                    initTdmaSlotMapSet.slots[k].requestedDurationMicroseconds = 25000;
                }
            }

            // Set TDMA slot map
            while (rnTdmaSlotMapSet(&initTdmaSlotMapSet) != 0 && ros::ok())
            {
                printf("Time out waiting for rnTdmaSlotMap confirm.\n");
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

    double dists[ancsTotal];
    double dists_calib[ancsTotal];
    memset(dists, 0, sizeof(dists));
    uint32_T tu_stamps[ancsTotal];
    memset(tu_stamps, 0, sizeof(tu_stamps));
    uint32_T tu_stamp_last = 0;
    uint32_T tu_stamps_kronolog[ancsTotal+1];
    memset(tu_stamps_kronolog, 0, sizeof(tu_stamps_kronolog));
    /*-----------------------------------------P410 interface configuration---------------------------------------------------*/


    /*-----------------------------------------IMU I2C interface configuration------------------------------------------------*/

    // RTIMUSettings *imuSettings = new RTIMUSettings("RTIMULib");
    int imuType;
    bool busIsI2C;
    unsigned char slaveAddress;
    // imuSettings->discoverIMU(imuType, busIsI2C, slaveAddress);
    // printf("IMU type: %d. I2C Bus: %d. SlaveAddr: %d\n", imuType, busIsI2C, slaveAddress);

    signed char gyro_orientation[9] = { 1, 0, 0,
                                        0, 1, 0,
                                        0, 0, 1 };

    int i2c_bus = 1;
    if(rcmEkfNodeHandle.getParam("i2c_bus", i2c_bus))
    {
        printf(KBLU "I2c bus %d is specified for IMU\n" RESET, i2c_bus);
        if (i2c_bus < MIN_I2C_BUS || i2c_bus > MAX_I2C_BUS)
        {
            printf(KRED "However I2C bus %d is invalid\n" RESET, i2c_bus);
            return -1;
        }
    }
    else
    {
        printf(KRED "Failed to identify I2C bus for IMU, terminating program!\n" RESET);
        return 0;
    }

    int gyro_accel_sample_rate = 100;
    if(rcmEkfNodeHandle.getParam("gyro_accel_sample_rate", gyro_accel_sample_rate))
    {
        if (gyro_accel_sample_rate < MIN_SAMPLE_RATE || gyro_accel_sample_rate > MAX_SAMPLE_RATE)
        {
            printf(KRED "Invalid sample rate %d, sample rate should be between %dHz and %dHz\n" RESET, gyro_accel_sample_rate, MIN_SAMPLE_RATE, MAX_SAMPLE_RATE);
            return -1;
        }
    }
    else
        printf(KBLU "Received valid sample rate %d\n" RESET, gyro_accel_sample_rate);

    int compass_sample_rate = 4;
    if(rcmEkfNodeHandle.getParam("compass_sample_rate", compass_sample_rate))
    {
        printf(KBLU "Received compass sample rate %dHz!\n" RESET, compass_sample_rate);
        if(compass_sample_rate < 0 || compass_sample_rate > MAX_COMPASS_SAMPLE_RATE)
        {
            printf(KRED "Compass sample rate %dHz is invalid.\n" RESET, compass_sample_rate);
            return -1;
        }
    }
    else
    {
        printf(KRED "Failed to collect IMU configurations, terminating program!\n" RESET);
        return 0;
    }

    /*-----------------------------------------IMU I2C interface configuration------------------------------------------------*/


    /*-------------------------------------------Logging Initialization---------------------------------------------------------*/
    if(rcmEkfNodeHandle.getParam("logEnable", logEnable))
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

    if(rcmEkfNodeHandle.getParam("logInput", logInput))
    {
        if(logInput)
            printf(KBLU "Input logging enabled!\n" RESET);
        else
            printf(KBLU "Input logging disabled!\n" RESET);
    }
    else
    {
        printf(KRED "Couldn't retrieve param 'logInput'. Not used by default." RESET);
        logInput = false;
    }

    if(rcmEkfNodeHandle.getParam("logOutput", logOutput))
    {
        if(logOutput)
            printf(KBLU "logOutput logging enabled!\n" RESET);
        else
            printf(KBLU "logOutput logging disabled!\n" RESET);
    }
    else
    {
        printf(KRED "Couldn't retrieve param 'logOutput'. Not used by default." RESET);
        logOutput = false;
    }

    struct passwd *pw = getpwuid(getuid());
    std::string logFileName;
    if(logEnable)
    {
        logFileName = string(pw->pw_dir) + std::string("/uwb_log/ekfiodlog.m");
        printf("%s\n", logFileName.data());
        fout.open(logFileName.data());

        logFileName = string(pw->pw_dir) + std::string("/uwb_log/imulog.m");
        printf("%s\n", logFileName.data());
        imuLog.open(logFileName.data());

        logFileName = string(pw->pw_dir) + std::string("/uwb_log/fcclog.m");
        printf("%s\n", logFileName.data());
        fccLog.open(logFileName.data());

    }
    /*-------------------------------------------Logging Initialization---------------------------------------------------------*/



    /*-------------------------------------------------Ekf configuration--------------------------------------------------------*/
    bool enableCalib = true;
    if(rcmEkfNodeHandle.getParam("enableCalib", enableCalib))
        if(enableCalib)
            printf(KBLU "Retrieved value 'true' for param 'enableCalib'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'fasle' for param 'enableCalib'!\n" RESET);
    else
    {
        //serialPortName = string(DFLT_PORT);
        printf(KYEL "Couldn't retrieve param 'enableCalib', using default mode without caliberation!\n" RESET);
        enableCalib = true;
    }

    //Collect the calibration parameters
    if(rcmEkfNodeHandle.getParam("albega", albega))
    {
        if( (albega.size() % 3) != 0)
        {
            printf(KRED "calibration param not whole. Exitting\n" RESET);
            return 0;
        }
        else
        {
            // Check to see if gamas are in order
            bool increasing = true;
            if (albega.size()/3 > 1)
                for( int i = 0; i < albega.size()/3; i++)
                {
                    if (albega[3*(i+1)] < albega[3*(i)])
                    {
                        increasing = false;
                        break;
                    }
                }

            if(increasing)
                printf(KBLU "valid gammas declared\n" RESET);
            else
            {
                printf(KRED "gammas not valid, exitting\n" RESET);
                return 0;
            }

            printf(KBLU "Retrieved %d anchor cordinates:\n" RESET, ancsTotal);

            for( int i = 0; i < albega.size()/3; i++)
            {
                printf(KBLU "\tal=%f;bet=%f;gam=%f.\n", albega[3*i], albega[3*i + 1], albega[3*i + 2]);
            }
        }
    }
    else
    {
        printf(KRED "Failed to collect anchor IDs, terminating program!\n" RESET);
        return 0;
    }

    double medianFilterSize = 5.0;
    if(rcmEkfNodeHandle.getParam("medianFilterSize", medianFilterSize))
        printf(KBLU "Retrieved value %f for param 'medianFilterSize'!\n" RESET, medianFilterSize);
    else
    {
        //serialPortName = string(DFLT_PORT);
        medianFilterSize = 5.0;
        printf(KYEL "Couldn't retrieve param 'medianFilterSize', using default size %f!\n" RESET, medianFilterSize);
    }

    double uwbAcceptance = 1.25;
    if(rcmEkfNodeHandle.getParam("uwbAcceptance", uwbAcceptance))
        printf(KBLU "Retrieved value %f for param 'uwbAcceptance'!\n" RESET, uwbAcceptance);
    else
    {
        uwbAcceptance = 1.25;
        printf(KYEL "Couldn't retrieve param 'uwbAcceptance', using default value %f\n" RESET, uwbAcceptance);
    }

    double ekfRescueTime = 1.0;
    if(rcmEkfNodeHandle.getParam("ekfRescueTime", ekfRescueTime))
        printf(KBLU "Retrieved value %f for param 'ekfRescueTime'!\n" RESET, ekfRescueTime);
    else
    {
        ekfRescueTime = 1.0;
        printf(KYEL "Couldn't retrieve param 'ekfRescueTime', using default value %f\n" RESET, ekfRescueTime);
    }

    int outlierThres = 20;
    if(rcmEkfNodeHandle.getParam("outlierThres", outlierThres))
        printf(KBLU "Retrieved value %d for param 'outlierThres'!\n" RESET, outlierThres);
    else
    {
        outlierThres = 20;
        printf(KYEL "Couldn't retrieve param 'outlierThres', using default value %d\n" RESET, outlierThres);
    }

    bool ekfRescueEnable = false;
    if(rcmEkfNodeHandle.getParam("ekfRescueEnable", ekfRescueEnable))
        if(ekfRescueEnable)
            printf(KBLU "Retrieved value 'true' for param 'ekfRescueEnable'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'fasle' for param 'ekfRescueEnable'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'ekfRescueEnable', not used by default.\n" RESET);
        ekfRescueEnable = false;
    }

    bool useImu = false;
    if(rcmEkfNodeHandle.getParam("useImu", useImu))
        if(useImu)
            printf(KBLU "Retrieved value 'true' for param 'useImu'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'useImu'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'useImu', using default mode without imu fusing!\n" RESET);
        useImu = false;
    }

    bool fuseBaro = false;
    if(rcmEkfNodeHandle.getParam("fuseBaro", fuseBaro))
        if(fuseBaro)
            printf(KBLU "Retrieved value 'true' for param 'fuseBaro'!\n" RESET);
        else
            printf(KBLU "Retrieved value 'false' for param 'fuseBaro'!\n" RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'fuseBaro', using default mode without fusing barometer!\n" RESET);
        fuseBaro = false;
    }

    //Collect the covariances parameters
    if(rcmEkfNodeHandle.getParam("ekfCovXyZ", ekfCovXyZ))
    {
        printf(KBLU "Retrieved x,y,z covariances: %f, %f\n" RESET, ekfCovXyZ[0], ekfCovXyZ[1]);
    }
    else
    {
        printf(KYEL "Failed to collect anchor IDs, using default values!\n" RESET);
        ekfCovXyZ.push_back(5.0);
        ekfCovXyZ.push_back(2.0);
    }

    ekf_Obj.ekf_iod_P.R_InitialValue = R;
    ekf_Obj.ekf_iod_P.ADMIT_RATIO_L_InitialValue = 1;
    ekf_Obj.ekf_iod_P.ADMIT_RATIO_R_InitialValue = sqrt(uwbAcceptance);
    ekf_Obj.ekf_iod_P.acc_xy_InitialValue = ekfCovXyZ[0];
    ekf_Obj.ekf_iod_P.acc_z_InitialValue = ekfCovXyZ[1];
    //initialize ekf object
    ekf_Obj.initialize();

    /*-------------------------------------------------Ekf configuration--------------------------------------------------------*/


    /*-------------------------------------------------Report Timer configuration-----------------------------------------------*/
//    printf("Establishing handler for signal %d\n", SIG);
//    sa.sa_flags = SA_SIGINFO;
//    sa.sa_sigaction = timerCallback;
//    sigemptyset(&sa.sa_mask);
//    sigaction(SIG, &sa, NULL);

//    sev.sigev_notify = SIGEV_SIGNAL;
//    sev.sigev_signo = SIG;
//    sev.sigev_value.sival_ptr = &timerid;
//    timer_create(CLOCKID, &sev, &timerid);
//    /* Start the timer */

//    its.it_value.tv_sec = 0;
//    its.it_value.tv_nsec = 1e9/reportRate;
//    its.it_interval.tv_sec = its.it_value.tv_sec;
//    its.it_interval.tv_nsec = its.it_value.tv_nsec;

//    timer_settime(timerid, 0, &its, NULL);
    /*-------------------------------------------------Report Timer configuration-----------------------------------------------*/

    /*-------------------------------------------------Initial Trilateration----------------------------------------------------*/

    //Reset the intial position
    for (uint32_T i = 0; i < 3; i++)
    {
        trilatPos[i] = 0;
        initialPos[i] = 0;
    }

    int failedInitiationCount[ancsTotal];
    memset(failedInitiationCount, 0, sizeof(failedInitiationCount));

    int rangeCount[ancsTotal];
    memset(rangeCount, 0, sizeof(rangeCount));

    //Clean up the buffer before any processing on the received signal
    // fd.flushInput();
    double  distsBuff[ancsTotal][TRILAT_TIMES];
    for(int i = 0; i < ancsTotal; i++)
        for(int j = 0; j < TRILAT_TIMES; j++)
            distsBuff[i][j] = 0;

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

        // std::vector<double> node_pos;    
        // n.getParam("node_pos", node_pos);


        printf("ID: %d\tRange: %f\n", indexUpdate+1, newRange);
        //publish
        if (newRange >0)
        {
            // printf("%f\n", newRange);

            switch(indexUpdate+1)
            {
                // printf ("the indexUpdate: %d\t", indexUpdate+1);
                case 0: position.x = node_pos[0];
                        position.y = node_pos[1];
                        position.z = node_pos[2];
                        break;
                case 1: position.x = node_pos[3];
                        position.y = node_pos[4];
                        position.z = node_pos[5];
                        break;
                case 2: position.x = node_pos[6];
                        position.y = node_pos[7];
                        position.z = node_pos[8];
                        break;
                case 3: position.x = node_pos[9];
                        position.y = node_pos[10];
                        position.z = node_pos[11];
                        break;
            }
            uwb_localization::uwbdata msg;
            msg.distance = newRange;
            msg.position = position;
            msg.nodeID = indexUpdate+1;

            // printf("msgCount: %d\nnodeID: %d\n", count, indexUpdate+1);
            printf("position: [%.3f  %.3f  %.3f]\n", position.x, position.y, position.z);
            printf("distance: %.3f\n", distance_lh);
            printf("---\n");

            lh_publisher.publish(msg);
            // loop_rate.sleep();
        }
    loop_rate.sleep();



        }
}