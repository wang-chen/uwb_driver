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
#include <uwb_localization/rcmEkfStateMsg.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <sensor_msgs/Imu.h>
#include <tf/transform_broadcaster.h>
#include <boost/assign/list_of.hpp>
#include <string.h>
#include "rcmIf.h"
#include "rcm.h"
#include <sys/ioctl.h>
//For log file use
#include <fstream>
//For i2c communication
#include <errno.h>
#include <linux/i2c-dev.h>
#include <pwd.h>
//#include "i2cfunc.h"
//MIT serial library
#include <serial/serial.h>
//ekf ert generated code
#include "trilatCalib.h"
#include "ekf_iod.h"

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

#define     DFLT_NODE_RATE      10
extern double simdt[3087];
extern double simd[3087];
extern uint8_T simi[3087];
//_____________________________________________________________________________
//
// Trilateration and ekf particulars
//_____________________________________________________________________________

//initial conditions for the trilaterator
static trilatCalibClass trilatCalib_Obj;
bool trilaterating = true;
#define     TRILAT_TIMES            20

//initial conditions for the ekf
static ekfIodClass ekf_Obj;
//inputs for the ekf
static double ancs[12] =
{
    -3.0, 3.0, 3.0, -3.0,
    -3.0, -3.0, 3.0, 3.0,
    -1.78, -1.21, -1.31, -1.31
    //-1.78, -0.1 , -1.31, -0.21
};
static double ancsTranspose[12] =
{
    -3.0, -3.0, -1.71,
    3.0, -3.0, -1.21,
    3.0, 3.0, -1.31,
    -3.0, 3.0, -1.31
};

//localization data
std::vector<double> ancsPos;
std::vector<int>    ancsId;

static double R = 0.2;              //Measurement
static double dists[4] = { 0, 0, 0, 0 };
static double deltat = 0;
static float baroGnd = 0, baroSea = 0, sonar_alt = 0, laser_alt = 0, xdd = 0, ydd = 0, zdd = 0;
static uint32_T imuTimestamp = 0;
static uint32_T imuUwbFirstTimestamp = 0;
static uint8_T nodeId = 1;		//WARNING: MATLAB GENERATED FUNCTION USES INDEX 1 FOR 1ST ELEMENT
//output of the ekf
static double x_est[6], x_prev[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static double P_est[36], P_prev[36] = { 0.5, 0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.5, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.5, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.5, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.0, 0.5
                                       };
static double tempDists[4] = { 0, 0, 0, 0 };//{ 7.51, 13.03, 12.69, 6.90 };
static double trilatPos[3] = { 0, 0, 0 };
static double initialPos[3];

//_____________________________________________________________________________
//
// P410 Serial Communications
//_____________________________________________________________________________
#define     DFLT_P410_PORT          "/dev/ttyACM0"
#define     P410_ITF_SERIAL         0
#define     P410_ITF_USB            1

#define     LOSS_MEMORY_LENGTH  20
uint8_t     lossMarkBuff[LOSS_MEMORY_LENGTH];
uint8_t     lossMarkBuffIndex = 0;
uint8_t     onlineNodes = 0;
uint8_t     onlineNodesTemp = 0;
uint8_t     loss = 100;

//_____________________________________________________________________________
//
// Lidar I2C Communications
//_____________________________________________________________________________
int lidarFile;
char *lidarFileName = "/dev/i2c-2";
#define LIDAR_ADDR  0x62
uint8_t lidarConf[2];               //First byte is the register's address, second byte is the data
uint8_T laserRangeArray[2];

//_____________________________________________________________________________
//
// FCC Serial Communications
//_____________________________________________________________________________
#define     DFLT_FCC_PORT           "/dev/ttyO1"
#define     BAUD_MACRO              B115200
#define     USE_MIT_SERIAL

#define F2U00_MSG_LENGTH            34
#define F2U01_MSG_LENGTH            58
#define F2U_MSG_MAX_LENGTH          F2U01_MSG_LENGTH
#define F2U_MSG_MIN_LENGTH          F2U00_MSG_LENGTH


static uint8_T fcc2UwbBuff[F2U_MSG_MAX_LENGTH];

//Frame's initials
#define F2U_HEADER1               (*(uint8_T *)(fcc2UwbBuff + 0)) //'F'
#define F2U_HEADER2               (*(uint8_T *)(fcc2UwbBuff + 1)) //'C'
#define F2U_ID                    (*(uint8_T *)(fcc2UwbBuff + 2)) //0x00 or 0x01

//Frame content msg 0x00
#define F2U00_LENGTH_BYTE           (*(uint8_T *)(fcc2UwbBuff + 3))
#define F2U00_ACCX                  (*(float *)(fcc2UwbBuff + 4))
#define F2U00_ACCY                  (*(float *)(fcc2UwbBuff + 8))
#define F2U00_ACCZ                  (*(float *)(fcc2UwbBuff + 12))
#define F2U00_ALTGND                (*(float *)(fcc2UwbBuff + 16))
#define F2U00_ALTSEA                (*(float *)(fcc2UwbBuff + 20))
#define F2U00_SNR_ALT               (*(float *)(fcc2UwbBuff + 24))
#define F2U00_TIMESTAMP             (*(uint32_T *)(fcc2UwbBuff + 28))
#define F2U00_CS1                   (*(uint8_T *)(fcc2UwbBuff + 32))
#define F2U00_CS2                   (*(uint8_T *)(fcc2UwbBuff + 33))

#define F2U00_SUMCHECK_LENGTH       30
#define F2U00_LENGTH                28

//Frame content msg 0x01
#define F2U01_LENGTH_BYTE           (*(uint8_T *)(fcc2UwbBuff + 3))
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
#define F2U01_LENGTH                52

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




//#define U2F01_HEADER1               (*(uint8_T *)(uwb2FccBuff + 0)) //'U'
//#define U2F01_HEADER2               (*(uint8_T *)(uwb2FccBuff + 1)) //'W'

//#define U2F01_ID                    (*(uint8_T *)(uwb2FccBuff + 2))
//#define U2F01_LENGTH                (*(uint8_T *)(uwb2FccBuff + 3))
//#define U2F01_ANCID0                (*(uint8_T *)(uwb2FccBuff + 4))
//#define U2F01_ANCX0                 (*(float *)(uwb2FccBuff + 5))
//#define U2F01_ANCY0                 (*(float *)(uwb2FccBuff + 9))
//#define U2F01_ANCZ0                 (*(float *)(uwb2FccBuff + 13))
//#define U2F01_ANCID1                (*(uint8_T *)(uwb2FccBuff + 17))
//#define U2F01_ANCX1                 (*(float *)(uwb2FccBuff + 18))
//#define U2F01_ANCY1                 (*(float *)(uwb2FccBuff + 22))
//#define U2F01_ANCZ1                 (*(float *)(uwb2FccBuff + 26))
//#define U2F01_ANCID2                (*(uint8_T *)(uwb2FccBuff + 30))
//#define U2F01_ANCX2                 (*(float *)(uwb2FccBuff + 31))
//#define U2F01_ANCY2                 (*(float *)(uwb2FccBuff + 35))
//#define U2F01_ANCZ2                 (*(float *)(uwb2FccBuff + 39))
//#define U2F01_ANCID3                (*(uint8_T *)(uwb2FccBuff + 43))
//#define U2F01_ANCX3                 (*(float *)(uwb2FccBuff + 44))
//#define U2F01_ANCY3                 (*(float *)(uwb2FccBuff + 48))
//#define U2F01_ANCZ3                 (*(float *)(uwb2FccBuff + 52))

//#define U2F01_CS1                   (*(uint8_T *)(uwb2FccBuff + 56))
//#define U2F01_CS2                   (*(uint8_T *)(uwb2FccBuff + 57))
//#define U2F01_SUMCHECK_LENGTH       54

#define	UP				0xFF
#define DOWN			0x00
//#define RCV_THRESHOLD   40
uint8_T		msgFlag = DOWN;

//declare the file descriptors for FCC
#ifdef USE_MIT_SERIAL
serial::Serial fd;
#else
int fd = -1;
struct termios newtio;
struct sigaction saio;           //definition of signal action
FILE *fpSerial = NULL;
#endif
//void signal_handler_IO(int status)
//{
//    int bytes_avail;
//    ioctl(fd, FIONREAD, &bytes_avail);
//    if(bytes_avail == F2U00_MSG_LENGTH)
//        printf("received 00 msg\n");
//    else if(bytes_avail == F2U01_MSG_LENGTH)
//        printf("received 01 msg\n");
//}

void timerCallback(const ros::TimerEvent& e)
{
    //Prepare the frame to send to FCC
    U2F00_HEADER1 = 'U';
    U2F00_HEADER2 = 'W';
    U2F00_ID      = 0x00;
    U2F00_LENGTH_BYTE  = 26;
    U2F00_X       = (float)x_est[0];
    U2F00_Y       = (float)x_est[1];
    U2F00_Z       = (float)x_est[2];
    U2F00_DX      = (float)x_est[3];
    U2F00_DY      = (float)x_est[4];
    U2F00_DZ      = (float)x_est[5];

    if(trilaterating)
    {
        U2F00_LOSS      = 100;
        U2F00_NODECOUNT = 0;

    }
    else
    {
        U2F00_LOSS          = loss*100/LOSS_MEMORY_LENGTH;//(uint8_t)(ambiRadius & 0x0F);
        U2F00_NODECOUNT     = onlineNodes;
    }
    //Checksum
    uint8_t CSA = 0, CSB = 0;
    for (uint8_T i = 0; i < U2F00_SUMCHECK_LENGTH; i++)
    {
        CSA = CSA + uwb2FccBuff[2+i];
        CSB = CSB + CSA;
    }
    U2F00_CS1 = CSA;
    U2F00_CS2 = CSB;

#ifdef USE_MIT_SERIAL
    fd.write(uwb2FccBuff, U2F00_MSG_LENGTH);
#else
    //Send the message to the FCC
    //tcdrain(fd);
    write(fd, uwb2FccBuff, U2F00_MSG_LENGTH);
#endif
}

void bufferCheck(std::ofstream& imuLog, bool logEnable)
{
    int bytes_avail = 0;

#ifdef USE_MIT_SERIAL
    bytes_avail = fd.available();
#else
    ioctl(fd, FIONREAD, &bytes_avail);
#endif
    //go on if there's no data sent
    if(bytes_avail != 0)
    {
        while(bytes_avail >= F2U_MSG_MIN_LENGTH)
        {
            printf("Buffered data = %d\n", bytes_avail);
            //first check the headers and id
#ifdef USE_MIT_SERIAL
            fd.read(fcc2UwbBuff, 4);
#else
            read(fd, fcc2UwbBuff, 4);
#endif
            if(F2U_HEADER1 == 'F' && F2U_HEADER2 == 'C' && F2U_ID == 0x00 && F2U00_LENGTH_BYTE == F2U00_LENGTH)
            {
                printf("Recieved an imu message.\n");
#ifdef USE_MIT_SERIAL
                fd.read(fcc2UwbBuff + 4, F2U00_MSG_LENGTH - 4);
#else
                read(fd, fcc2UwbBuff + 4, F2U00_MSG_LENGTH - 4);
#endif

                uint8_t CSA = 0, CSB = 0;
                for (uint8_T i = 0; i < F2U00_SUMCHECK_LENGTH; i++)
                {
                    CSA = CSA + fcc2UwbBuff[2+i];
                    CSB = CSB + CSA;
                }
                printf("CSA = %d|%d, CSB = %d|%d\n", CSA, F2U00_CS1, CSB, F2U00_CS2);
                if(CSA == F2U00_CS1 && CSB == F2U00_CS2)
                {
                    printf("IMU update CRC valid\n");
                    for(int i = 0; i < F2U00_MSG_LENGTH; i++)
                        printf("%x ", fcc2UwbBuff[i]);
                    printf("\n");
                    for(int i = 0; i < 6; i++)
                    {
                        float temp = (*(float *)(fcc2UwbBuff + 4 + i*4));
                        printf("%4.2f ", temp);
                    }
                    if(imuTimestamp == F2U00_TIMESTAMP)
                    {
                        printf(KYEL"Timestamp not changed, frame ignored!\n"RESET);
//#ifdef USE_MIT_SERIAL
//                        fd.flush();
//#else

//                        tcflush(fd, TCIFLUSH);
//#endif
                        return;
                    }
                    else
                    {
                        printf("%d\n", F2U00_TIMESTAMP);
                        xdd = F2U00_ACCX;
                        ydd = F2U00_ACCY;
                        zdd = F2U00_ACCZ;
                        baroGnd = F2U00_ALTGND;
                        baroSea = F2U00_ALTSEA;
                        sonar_alt = F2U00_SNR_ALT;
                        imuTimestamp = F2U00_TIMESTAMP;
                        if(logEnable)
                            imuLog << "ti=[ti " << imuTimestamp << "];xdd=[xdd " << xdd << "];ydd=[ydd "<< ydd << "];zdd=[zdd " << zdd << "];bg=[bg " << baroGnd << "];bs=[bs " << baroSea << "];sa=[sa " << sonar_alt << "];" << std::endl;
                    }
                }
                else
                {
                    printf(KRED"CRC failed!\n"RESET);
                    for(int i = 0; i < F2U00_MSG_LENGTH; i++)
                        printf("%x ", fcc2UwbBuff[i]);
                    printf("\n");
#ifdef USE_MIT_SERIAL
                    fd.flush();
#else

                    tcflush(fd, TCIFLUSH);
#endif
                    return;
                }

            }
            else if(F2U_HEADER1 == 'F' && F2U_HEADER2 == 'C' && F2U_ID == 0x01 && F2U01_LENGTH_BYTE == F2U01_LENGTH)
            {
                printf("Recieved an anchor message.\n");
#ifdef USE_MIT_SERIAL
                fd.read(fcc2UwbBuff + 4, F2U01_MSG_LENGTH - 4);
#else
                read(fd, fcc2UwbBuff + 4, F2U01_MSG_LENGTH - 4);
#endif
                uint8_t CSA = 0, CSB = 0;
                for (uint8_T i = 0; i < F2U01_SUMCHECK_LENGTH; i++)
                {
                    CSA = CSA + fcc2UwbBuff[2+i];
                    CSB = CSB + CSA;
                }
                printf("CSA = %d|%d, CSB = %d|%d\n", CSA, F2U01_CS1, CSB, F2U01_CS2);
                if(CSA == F2U01_CS1 && CSB == F2U01_CS2)
                {
                    printf("Anchor update CRC valid\n");
                    for(int i = 0; i < F2U01_MSG_LENGTH; i++)
                        printf("%x ", fcc2UwbBuff[i]);
                    printf("\n");
                    for(int i = 0; i < 4; i++)
                    {
                        ancsId[i] = F2U01_ANCID(i);
                        for(int j = 0; j < 3; j++)
                            ancsPos[i*3 + j] = F2U01_ANCPOS(i, j);
                    }

                    for(int i = 0; i < 3; i++)
                        for(int j = 0; j < 4; j++)
                        {
                            //printf("Transposing i=%d, j=%d.\n", i, j);
                            ancs[i*4 + j] = ancsPos[j*3 + i];
                        }

                    for(int i = 0; i < 12; i++)
                        printf("%4.2f # ", ancsPos[i]);
                    printf("\n");

                    //send back the frame to FCC to confirm
                    F2U_HEADER1 = 'U';
                    F2U_HEADER2 = 'W';
                    //tcdrain(fd);
#ifdef USE_MIT_SERIAL
                    fd.write(fcc2UwbBuff, F2U01_MSG_LENGTH);
#else
                    //Send the message to the FCC
                    //tcdrain(fd);
                    write(fd, fcc2UwbBuff, F2U01_MSG_LENGTH);
#endif
                    //                    printf("Sent: \n");
                    //                    for(int i = 0; i < F2U01_MSG_LENGTH; i++)
                    //                        printf("%x ", fcc2UwbBuff[i]);
                    //                    printf("\n");
                }
                else
                {
                    printf(KRED"CRC failed!\n"RESET);
                    for(int i = 0; i < F2U01_MSG_LENGTH; i++)
                        printf("%x ", fcc2UwbBuff[i]);
                    printf("\n");
#ifdef USE_MIT_SERIAL
                    fd.flush();
#else

                    tcflush(fd, TCIFLUSH);
#endif
                    return;
                }

            }
            else
            {
                //if anything other than this then we have got a misalignment,
                //flush to remove all residue
                printf(KRED"Unrecognized headers!\n"RESET);
#ifdef USE_MIT_SERIAL
                fd.read(fcc2UwbBuff + 4, bytes_avail - 4);
#else
                read(fd, fcc2UwbBuff + 4, bytes_avail - 4);
#endif
                for(int i = 0; i < bytes_avail; i++)
                    printf("%x ", fcc2UwbBuff[i]);
                printf("\n");
#ifdef USE_MIT_SERIAL
                    fd.flush();
#else

                    tcflush(fd, TCIFLUSH);
#endif
                return;
            }
        }
    }
}

//_____________________________________________________________________________
//
// main - sample app entry point
//_____________________________________________________________________________
using namespace std;
static double tu_stamps[4] = {0, 0, 0, 0};
static double x_tri[3] = {0.0, 0.0, 0.0};
#include "nlsTRI345.h"
int main(int argc, char *argv[])
{
    //Create ros handler to node
    ros::init(argc, argv, "uwb_localization");
    ros::NodeHandle rcmEkfNodeHandle("~");
    /*-----------------------------------------------Mischelaneous-----------------------------------------------------------*/

    //ros timer to synchronize transmission frequency to FCC
    ros::Timer timer = rcmEkfNodeHandle.createTimer(ros::Duration(0.2), timerCallback);

    //Collect the anchors IDs
    if(rcmEkfNodeHandle.getParam("ancsId", ancsId))
    {
        printf(KBLU"Retrieved anchor IDs: %d, %d, %d, %d\n"RESET, ancsId[0], ancsId[1], ancsId[2], ancsId[3]);
    }
    else
    {
        printf(KRED "Failed to collect anchor IDs, program exited!\n"RESET);
        return 0;
    }

    //Collect the anchors Positions
    if(rcmEkfNodeHandle.getParam("ancsPos", ancsPos))
    {
        printf(KBLU"Retrieved anchor positions IDs:\n\t"RESET);
        printf(KBLU"a%d[%4.2f, %4.2f, %4.2f]\n\t"RESET, ancsId[0], ancsPos[0], ancsPos[1], ancsPos[2]);
        printf(KBLU"a%d[%4.2f, %4.2f, %4.2f]\n\t"RESET, ancsId[1], ancsPos[3], ancsPos[4], ancsPos[5]);
        printf(KBLU"a%d[%4.2f, %4.2f, %4.2f]\n\t"RESET, ancsId[2], ancsPos[6], ancsPos[7], ancsPos[8]);
        printf(KBLU"a%d[%4.2f, %4.2f, %4.2f]\n"RESET, ancsId[3], ancsPos[9], ancsPos[10], ancsPos[11]);
        //        for(int i =0; i < 12; i++)
        //            ancsTranspose[i] = ancsPos[i];
        //        for(int i = 0; i < 3; i++)
        //            for(int j = 0; j < 4; j++)
        //                ancs[i*3 + j] = ancsTranspose[j*4 + i];
    }
    else
    {
        printf(KRED "Failed to collect anchor IDs, program exited!\n"RESET);
        return 0;
    }

    int rcmEkfRate = DFLT_NODE_RATE;
    if(rcmEkfNodeHandle.getParam("rcmLocalizationRate", rcmEkfRate))
        printf(KBLU"Retrieved value %d for param 'rcmLocalizationRate'\n"RESET, rcmEkfRate);
    else
        printf(KYEL "Couldn't retrieve param 'rcmLocalizationRate', applying default value %dHz\n"RESET, rcmEkfRate);

    /*-----------------------------------------------Mischelaneous-----------------------------------------------------------*/



    /*-----------------------------------------FCC interface configuration---------------------------------------------------*/
//    //FCC interfacing
//    string fccSerialPort = string(DFLT_FCC_PORT);
//    //check for the FCC port name
//    if(rcmEkfNodeHandle.getParam("fccSerialPort", fccSerialPort))
//        printf(KBLU"Retrieved value %s for param 'fccSerialPort'!\n"RESET, fccSerialPort.data());
//    else
//    {
//        printf(KRED "Couldn't retrieve param 'fccSerialPort', program closed!\n"RESET);
//        return 0;
//    }

//#ifdef USE_MIT_SERIAL
//    fd.setPort(fccSerialPort);
//    fd.setBaudrate(115200);
//    fd.setTimeout(5, 10, 2, 10, 2);
//    fd.open();
//    if (fd.isOpen())
//    {
//        fd.flush();
//        printf(KBLU "Connection established\n\n" RESET);
//    }
//    else
//    {
//        printf(KRED "serialInit: Failed to open port %s\n" RESET, fccSerialPort.data());
//        return 0;
//    }
//#else
//    //Open the serial port as a file descriptor for low level configuration
//    //read/write, not controlling terminal for process,
//    fd = open(fccSerialPort.data(), O_RDWR | O_NOCTTY | O_NDELAY);	//program won't be blocked during read
//    //fd = open(fccSerialPort.data(), O_RDWR | O_NOCTTY);			//program will be blocked in during read
//    if ( fd < 0 )
//    {
//        printf(KRED "serialInit: Could not open serial device %s\n" RESET, fccSerialPort.data());
//        return 0;
//    }

//    //    //Install the signal handler before making the device asynchronous
//    //    saio.sa_handler = signal_handler_IO;
//    //    sigemptyset(&saio.sa_mask);
//    //    saio.sa_flags = 0;
//    //    saio.sa_restorer = NULL;
//    //    sigaction(SIGIO,&saio,NULL);

//    //    //allow the process to receive SIGIO
//    //    fcntl(fd, F_SETOWN, getpid());
//    //    //Make the file descriptor asynchronous
//    //    fcntl(fd, F_SETFL, O_ASYNC | O_NONBLOCK);

//    //Set up serial settings
//    memset(&newtio, 0,sizeof(newtio));
//    newtio.c_cflag =  CS8 | CLOCAL | CREAD;		//no parity, 1 stop bit
//    newtio.c_iflag |= IGNBRK;  					//ignore break condition
//    newtio.c_oflag = 0;							//all options off
//    //set input mode (non-canonical, no echo,...)
//    newtio.c_lflag = 0;
//    //non-canonical condition, RCV_THRESHOLD is the number of bytes to return
//    newtio.c_cc[VTIME] = 0;
//    newtio.c_cc[VMIN] = 0;

//    //Flush the residual data and activate new settings
//    tcflush(fd, TCIFLUSH);
//    if (cfsetispeed(&newtio, BAUD_MACRO) < 0 || cfsetospeed(&newtio, BAUD_MACRO) < 0)
//    {
//        printf(KRED"Cannot set baudrate for %s\n", fccSerialPort.data());
//        close(fd);
//        return 0;
//    }
//    else
//    {
//        tcsetattr(fd, TCSANOW, &newtio);
//        tcflush(fd, TCIOFLUSH);
//        printf(KBLU "Connection established\n\n" RESET);
//    }
//    //Open file as a standard I/O stream
//    fpSerial = fdopen(fd, "r+");
//    if (!fpSerial)
//    {
//        printf(KRED "serialInit: Failed to open %s as serial stream\n" RESET, fccSerialPort.data());
//        fpSerial = NULL;
//    }
//#endif

    /*------------------------------------------FCC interface configuration---------------------------------------------------*/



    /*-----------------------------------------P410 interface configuration---------------------------------------------------*/
    //P410 interface
    int p410Interface = P410_ITF_SERIAL;
    //check for the interface selected
    if(rcmEkfNodeHandle.getParam("p410InterfaceChoice", p410Interface))
        switch(p410Interface)
        {
        case    P410_ITF_SERIAL:
            printf(KBLU"P410 Serial interface selected!\n"RESET);
            break;
        case    P410_ITF_USB:
            printf(KBLU"P410 USB interface selected!\n"RESET);
            break;
        default:
            break;
        }
    else
    {
        printf(KRED "Failed to select communication interface, program exited!\n"RESET);
        return 0;
    }

    string p410SerialPort = string(DFLT_P410_PORT);
    //check for the p410 port's name
    if(rcmEkfNodeHandle.getParam("p410SerialPort", p410SerialPort))
        printf(KBLU"Retrieved value %s for param 'p410SerialPort'!\n"RESET, p410SerialPort.data());
    else
    {
        printf(KRED "Couldn't retrieve param 'p410SerialPort', program closed!\n"RESET);
        return 0;
    }

    //P410 configuration variables
    //int destNodeId[4] = {ancsId[0], ancsId[1], ancsId[2], ancsId[3]};
    int status;
    rcmIfType   rcmIf;
    rcmConfiguration config;
    rcmMsg_FullRangeInfo rangeInfo;
    rcmMsg_DataInfo dataInfo;

//    //initialize P410 serial interface
//    {
//        printf("\nRCM Localization App\n\n");

//        if(p410Interface == P410_ITF_SERIAL)
//            rcmIf = rcmIfSerial;
//        else if (p410Interface == P410_ITF_USB)
//            rcmIf = rcmIfUsb;

//        // initialize the interface to the RCM
//        if (rcmIfInit(rcmIf, &p410SerialPort[0]) != OK)
//        {
//            printf("Initialization failed.\n");
////            exit(0);
//        }

//        // Make sure RCM is awake
//        if (rcmSleepModeSet(RCM_SLEEP_MODE_ACTIVE) != 0)
//        {
//            printf("Time out waiting for sleep mode set.\n");
////            exit(0);
//        }

//        // Make sure opmode is RCM
//        if (rcmOpModeSet(RCM_OPMODE_RCM) != 0)
//        {
//            printf("Time out waiting for opmode set.\n");
////            exit(0);
//        }


//        // execute Built-In Test - verify that radio is healthy
//        if (rcmBit(&status) != 0)
//        {
//            printf("Time out waiting for BIT.\n");
////            exit(0);
//        }

//        if (status != OK)
//        {
//            printf("Built-in test failed - status %d.\n", status);
////            exit(0);
//        }
//        else
//        {
//            printf("Radio passes built-in test.\n\n");
//        }

//        // retrieve config from RCM
//        if (rcmConfigGet(&config) != 0)
//        {
//            printf("Time out waiting for config confirm.\n");
////            exit(0);
//        }

//        // print out configuration
//        printf("Configuration:\n");
//        printf("\tnodeId: %d\n", config.nodeId);
//        printf("\tintegrationIndex: %d\n", config.integrationIndex);
//        printf("\tantennaMode: %d\n", config.antennaMode);
//        printf("\tcodeChannel: %d\n", config.codeChannel);
//        printf("\telectricalDelayPsA: %d\n", config.electricalDelayPsA);
//        printf("\telectricalDelayPsB: %d\n", config.electricalDelayPsB);
//        printf("\tflags: 0x%X\n", config.flags);
//        printf("\ttxGain: %d\n", config.txGain);

//        // retrieve status/info from RCM
//        if (rcmStatusInfoGet(&statusInfo) != 0)
//        {
//            printf("Time out waiting for status info confirm.\n");
////            exit(0);
//        }

//        // print out status/info
//        printf("\nStatus/Info:\n");
//        printf("\tPackage version: %s\n", statusInfo.packageVersionStr);
//        printf("\tRCM version: %d.%d build %d\n", statusInfo.appVersionMajor,
//               statusInfo.appVersionMinor, statusInfo.appVersionBuild);
//        printf("\tUWB Kernel version: %d.%d build %d\n", statusInfo.uwbKernelVersionMajor,
//               statusInfo.uwbKernelVersionMinor, statusInfo.uwbKernelVersionBuild);
//        printf("\tFirmware version: %x/%x/%x ver %X\n", statusInfo.firmwareMonth,
//               statusInfo.firmwareDay, statusInfo.firmwareYear,
//               statusInfo.firmwareVersion);
//        printf("\tSerial number: %08X\n", statusInfo.serialNum);
//        printf("\tBoard revision: %c\n", statusInfo.boardRev);
//        printf("\tTemperature: %.2f degC\n\n", statusInfo.temperature / 4.0);
//    }
    /*-----------------------------------------P410 interface configuration---------------------------------------------------*/



    /*---------------------------------------LIDAR I2C interface configuration------------------------------------------------*/
    if ((lidarFile = open(lidarFileName, O_RDWR)) < 0)
        printf(KRED"Failed to open the i2c bus\n"RESET);
    else
        printf("I2C connection to device %x established.\n", LIDAR_ADDR);

    if (ioctl(lidarFile, I2C_SLAVE, LIDAR_ADDR)  < 0)
        printf(KRED"Failed to acquire i2c bus\n"RESET);
    else
        printf("Acquired bus as master. LiDar sensor is now I2C slave.\n");

    //Set the frequency of reading to 100Hz by writing 0x13 to 0x45
    lidarConf[0] = 0x45;
    lidarConf[1] = 0x13;
    write(lidarFile, lidarConf, 2);

    //Set the number of readings until stop by writing to the 0x11 register, 0xff means infinite
    lidarConf[0] = 0x11;
    lidarConf[1] = 0xff;
    write(lidarFile, lidarConf, 2);


    //Enable the laser senso
    lidarConf[0] = 0x00;
    lidarConf[1] = 0x04;
    write(lidarFile, lidarConf, 2);

    //Get the initial laser reading
    lidarConf[0] = 0x8f;
    write(lidarFile, lidarConf, 1);
    read(lidarFile, laserRangeArray, 2);
    laser_alt = (float)(((uint32_T)laserRangeArray[0] << 8) + (uint32_T)laserRangeArray[1])/100.0;
    printf(KGRN"Initial Laser Range: %f\n"RESET, laser_alt);
    /*-----------------------------------------LIDAR I2C interface configuration------------------------------------------------*/



    /*-------------------------------------------Logging Initialization---------------------------------------------------------*/
    bool logEnable = false;
    if(rcmEkfNodeHandle.getParam("logEnable", logEnable))
    {
        if(logEnable)
            printf(KBLU"Logging enabled!\n"RESET);
        else
            printf(KBLU"Logging disabled!\n"RESET);
    }
    else
    {
        printf(KRED "Couldn't retrieve param 'logEnable'. Not used by default."RESET);
        logEnable = false;
    }
    ofstream fout;
    ofstream imuLog;
    struct passwd *pw = getpwuid(getuid());
    char *logFileName = NULL;
    if(logEnable)
    {
        char *ekfLogFileName = "/uwb_log/ekfiodlog.m";
        asprintf(&logFileName, "%s%s", pw->pw_dir, ekfLogFileName);
        printf("%s\n", logFileName);
        fout.open(logFileName);

        char *imuLogFileName = "/uwb_log/imulog.m";
        asprintf(&logFileName, "%s%s", pw->pw_dir, imuLogFileName);
        printf("%s\n", logFileName);
        imuLog.open(logFileName);
        imuLog << "ti=0;" << "bg=0;" << "bs=0;" << "sa=0;" << "xdd=0;" << "ydd=0;" << "zdd=0;" << endl;
    }
    /*-------------------------------------------Logging Initialization---------------------------------------------------------*/



    /*-------------------------------------------------Ekf configuration--------------------------------------------------------*/

    bool enableCalib = true;
    if(rcmEkfNodeHandle.getParam("enableCalib", enableCalib))
        if(enableCalib)
            printf(KBLU"Retrieved value 'true' for param 'enableCalib'!\n"RESET);
        else
            printf(KBLU"Retrieved value 'fasle' for param 'enableCalib'!\n"RESET);
    else
    {
        //serialPortName = string(DFLT_PORT);
        printf(KYEL "Couldn't retrieve param 'enableCalib', using default mode without caliberation!\n"RESET);
        enableCalib = true;
    }

    double medianFilterSize = 5.0;
    if(rcmEkfNodeHandle.getParam("medianFilterSize", medianFilterSize))
        printf(KBLU"Retrieved value %f for param 'medianFilterSize'!\n"RESET, medianFilterSize);
    else
    {
        //serialPortName = string(DFLT_PORT);
        medianFilterSize = 5.0;
        printf(KYEL "Couldn't retrieve param 'medianFilterSize', using default size %f!\n"RESET, medianFilterSize);
    }

    bool useLaser = true;
    if(rcmEkfNodeHandle.getParam("useLaser", useLaser))
        if(useLaser)
            printf(KBLU"Retrieved value 'true' for param 'useLaser'!\n"RESET);
        else
            printf(KBLU"Retrieved value 'fasle' for param 'useLaser'!\n"RESET);
    else
    {
        printf(KYEL "Couldn't retrieve param 'useLaser', using default mode with laser ranging!\n"RESET);
        useLaser = true;
    }
    /*-------------------------------------------------Ekf configuration--------------------------------------------------------*/



    /*-------------------------------------------------Initial Trilateration----------------------------------------------------*/
    //initialize trilateration object
    trilatCalib_Obj.initialize();

    //Reset the intial position
    for (int i = 0; i < 3; i++)
    {
        trilatPos[i] = 0;
        initialPos[i] = 0;
    }

    long failedInitiationCount = 0;
    //Clean up the buffer before any processing on the received signal
//#ifdef USE_MIT_SERIAL
//    fd.flush();
//#else

//    tcflush(fd, TCIFLUSH);
//#endif
//    //get the initial position by multiple trilaterations
//    for (int i = 0; i < TRILAT_TIMES; i++)
//    {

//        //do the trilateration everytime a quadruple of distances is aquired
//        for (int nodeId = 1; nodeId < 5; nodeId++)
//        {
//            //Make the measurement
//            rcmRangeTo(ancsId[nodeId - 1], RCM_ANTENNAMODE_TXA_RXA, 0, NULL, &rangeInfo, &dataInfo, &scanInfo, &fullScanInfo);

//            //Get the laser reading
//            lidarConf[0] = 0x8f;
//            write(lidarFile, lidarConf, 1);
//            read(lidarFile, laserRangeArray, 2);
//            laser_alt = (float)(((uint32_T)laserRangeArray[0] << 8) + (uint32_T)laserRangeArray[1])/100.0;
//            printf(KGRN"LIDAR Range: %f\n"RESET, laser_alt);

//            ros::spinOnce();
//            bufferCheck(imuLog, logEnable);
//            //Value must be less than 15m
//            if ((rangeInfo.rangeMeasurementType & RCM_RANGE_TYPE_PRECISION) && (rangeInfo.precisionRangeMm < 50000))
//            {
//                if (i == 0)
//                {
//                    tempDists[nodeId - 1] = rangeInfo.precisionRangeMm / 1000.0; //Range measurements are in mm
//                }
//                else if (abs(rangeInfo.precisionRangeMm / 1000.0 - tempDists[nodeId - 1]) < 1)
//                {
//                    tempDists[nodeId - 1] = rangeInfo.precisionRangeMm / 1000.0; //Range measurements are in mm
//                }
//                else
//                {
//                    nodeId = 0;
//                    failedInitiationCount++;
//                    if (failedInitiationCount == 10)
//                    {
//                        //if failing to go a head due to too much difference with previous value, then previous range
//                        //appears to be wrong, reverse to previous update
//                        i--;
//                        failedInitiationCount = 0;
//                        continue;
//                    }
//                    continue;
//                }
//            }
//            //if any ranging fails, restart from the 1st range
//            else
//            {
//                nodeId = 0;
//                continue;
//            }
//        }
//        printf("iteration #%d:\td1 = %f, d2 = %f, d3 = %f, d4 = %f;\n", i, tempDists[0], tempDists[1], tempDists[2], tempDists[3]);
//        //add up the measurements to cancel noise
//        for (int j = 0; j < 4; j++)
//            dists[j] += tempDists[j];
//    }

//    //averaging the measurements to get the initial position for the ekf
//    for (int j = 0; j < 4; j++)
//        dists[j] /= TRILAT_TIMES;

    uint32_T k = 4;

    dists[0] = simd[0];
    dists[1] = simd[1];
    dists[2] = simd[2];
    dists[3] = simd[3];

    printf("average ranges , d1 = %f; d2 = %f; d3 = %f; d4 = %f\n", dists[0], dists[1], dists[2], dists[3]);

    //trilatering
    trilatCalib_Obj.step(&ancsPos[0], dists, enableCalib, initialPos);

    printf(" init pos: x = %f, y = %f, z = %f\n", initialPos[0], initialPos[1], initialPos[2]);

    x_prev[0] = initialPos[0];
    x_prev[1] = initialPos[1];
    x_prev[2] = initialPos[2];

    ekf_Obj.ekf_iod_P.R_InitialValue = 0.2;
    ekf_Obj.ekf_iod_P.acc_xy_InitialValue = 5.0;
    ekf_Obj.ekf_iod_P.acc_z_InitialValue = 4.0;


    trilaterating = false;
    //initialize ekf object
    ekf_Obj.initialize();
    if(imuUwbFirstTimestamp == 0)
        imuUwbFirstTimestamp = imuTimestamp;

    if(logEnable)
    {
        fout << "imuStartTime = " << imuUwbFirstTimestamp << ";" << endl;
        fout << "i=" << 1 << ";d=" << dists[0] << ";oR=" << 0 << ";oL=" << 0 << ";df=" << tempDists[0] << ";x=" << initialPos[0] << ";y=" << initialPos[1] << ";z=" << initialPos[2] << ";";
        fout << "xd=" << 0 << ";yd=" << 0 << ";zd=" << 0 << ";";
        fout << "p1=" << P_prev[0] << "; p2=" << P_prev[7] << "; p3=" << P_prev[14] << "; ";
        fout << "dt=" << deltat << "; loss=" << 0 << ";" << endl;
        fout << "la=" << laser_alt << ";" << endl;

        fout << "i=[i " << 2 << "];d=[d " << dists[1] << "];oR=[oR " << 0 << "];oL=[oL " << 0 << "];df=[df " << tempDists[1] << "];x=[x " << initialPos[0] << "];y=[y " << initialPos[1] << "];z=[z " << initialPos[2] << "];";
        fout << "xd=[xd " << 0 << "];yd=[yd " << 0 << "];zd=[zd " << 0 << "];";
        fout << "p1=[p1 " << P_prev[0] << "]; p2=[p2 " << P_prev[7] << "]; p3=[p3 " << P_prev[14] << "]; ";
        fout << "dt=[dt " << deltat << "]; loss=[loss " << 0 << "];" << endl;
        fout << "la=[la " << laser_alt << "];" << endl;

        fout << "i=[i " << 3 << "];d=[d " << dists[2] << "];oR=[oR " << 0 << "];oL=[oL " << 0 << "];df=[df " << tempDists[2] << "];x=[x " << initialPos[0] << "];y=[y " << initialPos[1] << "];z=[z " << initialPos[2] << "];";
        fout << "xd=[xd " << 0 << "];yd=[yd " << 0 << "];zd=[zd " << 0 << "];";
        fout << "p1=[p1 " << P_prev[0] << "]; p2=[p2 " << P_prev[7] << "]; p3=[p3 " << P_prev[14] << "]; ";
        fout << "dt=[dt " << deltat << "]; loss=[loss " << 0 << "];" << endl;
        fout << "la=[la " << laser_alt << "];" << endl;

        fout << "i=[i " << 4 << "];d=[d " << dists[3] << "];oR=[oR " << 0 << "];oL=[oL " << 0 << "];df=[df " << tempDists[3] << "];x=[x " << initialPos[0] << "];y=[y " << initialPos[1] << "];z=[z " << initialPos[2] << "];";
        fout << "xd=[xd " << 0 << "];yd=[yd " << 0 << "];zd=[zd " << 0 << "];";
        fout << "p1=[p1 " << P_prev[0] << "]; p2=[p2 " << P_prev[7] << "]; p3=[p3 " << P_prev[14] << "]; ";
        fout << "dt=[dt " << deltat << "]; loss=[loss " << 0 << "];" << endl;
        fout << "la=[la " << laser_alt << "];" << endl;
    }

    /*-------------------------------------------------Initial Trilateration----------------------------------------------------*/


    /*------------------------------------------------------EKF Localization----------------------------------------------------*/

    nodeId = 1;
    bool lastRangingSuccesful = true;

    //enter loop to a ranging a node and broadcasting the resulting range
    ros::Rate rate(rcmEkfRate);
    ros::Time timeStart, timeEnd;
    //clean up buffer before reading
//#ifdef USE_MIT_SERIAL
//    fd.flush();
//#else
//    tcflush(fd, TCIFLUSH);
//#endif
    while(ros::ok())
    {

        printf("step %d\n", k);
        nodeId = simi[k];
        dists[nodeId - 1] = simd[k];
        deltat = simdt[k] - simdt[k-1];
        laser_alt = 10000.0;
        boolean_T outlierR = true, outlierL = true;

        tu_stamps[nodeId - 1] = simdt[k];

        double maxStamp = tu_stamps[0];
        double minStamp = tu_stamps[0];

        uint8_T ancs_ids[4] = {1, 2, 3, 4};
        uint8_T ancs_tri_count = 4;
        uint8_T minStampId = 0;
        double timeWindow = 0.25;

        printf("\n");
        for(int i=0; i < 4; i++)
            printf("t%d=%f\t", i, tu_stamps[i]);
        printf("\n");

        while(ros::ok())
        {
            //find the latest and earliest stamps
            for(int j = 1; j < 4; j++)
            {
                if(maxStamp < tu_stamps[j])
                    maxStamp = tu_stamps[j];
                if(minStamp > tu_stamps[j] && ancs_ids[j] != 0)
                {
                    minStamp = tu_stamps[j];
                    minStampId = j;
                }
            }

            //if time difference between 4 is larger than window time
            //then we mark the aged range and check for the less redundant case

            if(maxStamp - minStamp > timeWindow)
            {
                ancs_ids[minStampId] = 0;
                ancs_tri_count--;
            }
            else
                break;  //if the time difference is smaller than time window
                        //then can proceed to least square trilateration

            //Stop checking time if there are not enough fresh ranges
            if(ancs_tri_count < 3)
                break;
        }

        printf("Age gap: %f\n", maxStamp - minStamp);

//        printf("ids=[");
//        for(int i = 0; i< 4; i++)
//            printf("%d\t", ancs_ids[i]);
//        printf("];\n");

        uint32_T ancsHoloLength = 3*ancs_tri_count;
        double ancs_tri[ancsHoloLength];
        double dists_tri[ancs_tri_count];
        for (uint32_T i = 0; i < ancs_tri_count; i++)
            for (int j = 0; j < 4; j++)
                if(ancs_ids[j] == 0)
                    continue;
                else
                {
                    for(int k = 0; k < 3; k++)
                        ancs_tri[i*3+k] = ancsPos[j*3+k];

                    dists_tri[i] = dists[j];
                    ancs_ids[j] = 0;
                    break;
                }
        int32_T ancs_tri_dims[2] = {3, ancs_tri_count};
        int32_T dists_dims[2] = {1, ancs_tri_count};
        double ites, errEst;
        //printf("na=%d, arxac=%dx%d, drxdc=%dx%d\n", ancs_tri_count, ancs_tri_dims[0], ancs_tri_dims[1], dists_dims[0], dists_dims[1]);

//        printf("d=[");
//        for(int i = 0; i< ancs_tri_count; i++)
//            printf("%4.2f\t", dists_tri[i]);
//        printf("];\n");

//        printf("ancs=[");
//        for(int i = 0; i < ancs_tri_count*3; i++)
//            printf("%4.2f\t", ancs_tri[i]);
//        printf("];\n");

        ros::Time nlsTime = ros::Time::now();

        if(ancs_tri_count > 2)
        {
            nlsTRI345(ancs_tri_count, ancs_tri, ancs_tri_dims, dists_tri, dists_dims, 0.0, 40, x_tri, &ites, &errEst);
            printf("nls time: %f\n", (ros::Time::now() - nlsTime).toSec());
        }
        else
            printf(KYEL"Ranges are not in the s time window\n"RESET);

        ros::Time ekfTime = ros::Time::now();

        ekf_Obj.step(dists, nodeId, deltat, enableCalib, &ancsPos[0], x_prev, P_prev, -laser_alt, 0, x_est, P_est, &outlierR, &outlierL);
        printf("ekf time: %f\n", (ros::Time::now() - ekfTime).toSec());

//        ros::spinOnce();
//        //check the msg buffer
//        bufferCheck(imuLog, logEnable);

//        //reset timer if last ranging was successful
//        if (lastRangingSuccesful)
//            timeStart = ros::Time::now();
//        //        else
//        //            faultyRangingCount++;

//        //get the range to an anchor
//        rcmRangeTo(ancsId[nodeId - 1], RCM_ANTENNAMODE_TXA_RXA, 0, NULL, &rangeInfo, &dataInfo, &scanInfo, &fullScanInfo);
//        //if measurement succeeds proceed to the ekf, otherwise move to the next anchor
//        if ((rangeInfo.rangeMeasurementType & RCM_RANGE_TYPE_PRECISION) && rangeInfo.precisionRangeMm < 50000)
//        {
//            rate.sleep();
//            lossMarkBuff[lossMarkBuffIndex] = DOWN;
//            if(lossMarkBuffIndex++ == LOSS_MEMORY_LENGTH)
//                lossMarkBuffIndex = 0;
//            loss = 0;
//            for(int i = 0; i < LOSS_MEMORY_LENGTH; i++)
//                if(lossMarkBuff[i] == UP)
//                    loss += 1;

//            //calculate deltat
//            timeEnd = ros::Time::now();
//            deltat = (ros::Time::now() - timeStart).toSec();

//            dists[nodeId - 1] = rangeInfo.precisionRangeMm / 1000.0; //Range measurements are in mm

            //Get the laser reading
            lidarConf[0] = 0x8f;
            write(lidarFile, lidarConf, 1);
            read(lidarFile, laserRangeArray, 2);
            laser_alt = (float)(((uint32_T)laserRangeArray[0] << 8) + (uint32_T)laserRangeArray[1])/100.0;
            printf(KGRN"LIDAR Range: %f\n"RESET, laser_alt);

//            boolean_T outlierR = false, outlierL=false;
//            //step the model
//            if(!useLaser)
//                laser_alt = 1000.0;

//            ekf_Obj.step(dists, nodeId, deltat, laser_alt, enableCalib, &ancsPos[0], x_prev, P_prev, x_est, P_est, &outlierR, &outlierL);

//            //Print and log state values
//            if (lastRangingSuccesful)
                printf("i=[i %d]; d=[d %6.4f]; xt=[xt %6.4f]; yt=[yt %6.4f]; zt=[zt %6.4f]; xe=[xe %6.4f]; ye=[ye %6.4f]; ze=[ze %6.4f]; dt=[dt %6.4f]; loss=[loss %d];\n",
                       nodeId,
                       dists[nodeId - 1],
                        x_tri[0], x_tri[1], x_tri[2],
                        x_est[0], x_est[1], x_est[2],
                        deltat,
                        loss
                        );
//            else
//                printf("\nI=[I %d]; D=[D %6.4f]; X=[X %6.4f]; Y=[Y %6.4f]; Z=[Z %6.4f]; P1=[P1 %6.4f]; P2=[P2 %6.4f]; P3=[P3 %6.4f]; DT=[DT %6.4f]; LOSS=[LOSS %d];\n\n",
//                       nodeId,
//                       dists[nodeId - 1],
//                        x_est[0], x_est[1], x_est[2],
//                        P_est[0],
//                        P_est[7],
//                        P_est[14],
//                        deltat,
//                        loss
//                        );
            if(logEnable)
            {
                fout << "i=[i " << (int)nodeId << "];d=[d " << dists[nodeId - 1] << "];oR=[oR " << (int) outlierR << "];oL=[oL " << (int) outlierL << "];df=[df " << dists[nodeId - 1]<< "];x=[x " << x_tri[0] << "];y=[y " << x_tri[1] << "];z=[z " << x_tri[2] << "];";
                fout << "xd=[xd " << x_est[3] << "];yd=[yd " << x_est[4] << "];zd=[zd " << x_est[5] << "];";
                fout << "p1 = [p1 " << P_est[0] << "]; p2 = [p2 "<<P_est[7]<<"]; p3 = [p3 "<<P_est[14]<<"]; ";
                fout << "dt = [dt "<<deltat<<"]; loss = [loss "<< (int)loss <<"];" << endl;
                fout << "la=[la " << laser_alt << "];" << endl;
            }

            for(int i = 0; i < 6; i++)
                x_prev[i]=x_est[i];
            for(int i = 0; i < 36; i++)
                P_prev[i]=P_est[i];

//            lastRangingSuccesful = true;
//        }
//        else
//        {
//            lossMarkBuff[lossMarkBuffIndex] = UP;
//            if(lossMarkBuffIndex++ == LOSS_MEMORY_LENGTH)
//                lossMarkBuffIndex = 0;
//            //Raise this flag to not to reset timer on next loop
//            lastRangingSuccesful = false;
//        }


//        if(lastRangingSuccesful)
//            onlineNodesTemp++;
//        nodeId++;
//        if (nodeId > 4)
//        {
//            onlineNodes = onlineNodesTemp;
//            onlineNodesTemp = 0;
//            nodeId = 1;
//        }
        //Sleep(80);
            k++;
            if(k >= 1579)
            {
                printf("Simulation done!\n");
                exit(0);
            }
    }

    //cleanup
    ekf_Obj.terminate();
    getchar();
    rcmIfClose();
    return 0;
}
/*------------------------------------------------------EKF Localization----------------------------------------------------*/

