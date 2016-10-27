//
// File: ekf_iod.h
//
// Code generated for Simulink model 'ekf_iod'.
//
// Model version                  : 1.425
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Thu Jan 07 15:23:22 2016
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objectives:
//    1. Execution efficiency
//    2. Traceability
// Validation result: Not run
//
#ifndef RTW_HEADER_ekf_iod_h_
#define RTW_HEADER_ekf_iod_h_
#include <stddef.h>
#include "rtwtypes.h"
#include <math.h>
#include <string.h>
#ifndef ekf_iod_COMMON_INCLUDES_
# define ekf_iod_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 // ekf_iod_COMMON_INCLUDES_

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

// Forward declaration for rtModel
typedef struct tag_RTM_ekf_iod_T RT_MODEL_ekf_iod_T;

// Block signals (auto storage)
typedef struct {
  real_T F[36];
  real_T P_pre[36];
  real_T P_est[36];                    // '<S1>/ekf estimation'
  real_T F_m[36];
  real_T H[12];
  real_T b_K[12];
  real_T H_c[12];
  real_T x_pre[6];
  real_T H1[6];
  real_T K[6];
  real_T x_est[6];                     // '<S1>/ekf estimation'
  real_T P_pre_k[6];
  real_T distsCalibed[4];              // '<S1>/Range Calibration'
  real_T dist_pre;
  real_T innov1;
  real_T innov2;
  real_T scale;
} B_ekf_iod_T;

// Block states (auto storage) for system '<Root>'
typedef struct {
  real_T ADMIT_RATIO_L;                // '<S1>/ADMIT_RATIO_L'
  real_T ADMIT_RATIO_R;                // '<S1>/ADMIT_RATIO_R'
  real_T R;                            // '<S1>/R'
  real_T acc_xy;                       // '<S1>/acc_xy'
  real_T acc_z;                        // '<S1>/acc_z'
} DW_ekf_iod_T;

// Parameters (auto storage)
struct P_ekf_iod_T_ {
  real_T ADMIT_RATIO_L_InitialValue;   // Expression: 0.1
                                       //  Referenced by: '<S1>/ADMIT_RATIO_L'

  real_T ADMIT_RATIO_R_InitialValue;   // Expression: 1.5
                                       //  Referenced by: '<S1>/ADMIT_RATIO_R'

  real_T R_InitialValue;               // Expression: 0.2
                                       //  Referenced by: '<S1>/R'

  real_T acc_xy_InitialValue;          // Expression: 5
                                       //  Referenced by: '<S1>/acc_xy'

  real_T acc_z_InitialValue;           // Expression: 2
                                       //  Referenced by: '<S1>/acc_z'

};

// Parameters (auto storage)
typedef struct P_ekf_iod_T_ P_ekf_iod_T;

// Real-time Model Data Structure
struct tag_RTM_ekf_iod_T {
  const char_T * volatile errorStatus;
};

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

// Class declaration for model ekf_iod
class ekfIodClass {
  // public data and function members
 public:
  // Tunable parameters
  P_ekf_iod_T ekf_iod_P;

  // Block signals
  B_ekf_iod_T ekf_iod_B;

  // Block states
  DW_ekf_iod_T ekf_iod_DW;

  // Model entry point functions

  // model initialize function
  void initialize();

  // model step function
  void step(real_T arg_dists[4], uint8_T arg_nodeId, real_T arg_deltat,
            boolean_T arg_calibEnable, real_T arg_ancs[12], real_T arg_x_prev[6],
            real_T arg_P_prev[36], real_T arg_z, real_T arg_u, real_T arg_x_est
            [6], real_T arg_P_est[36], boolean_T *arg_outlierR, boolean_T
            *arg_outlierH);

  // model terminate function
  void terminate();

  // Constructor
  ekfIodClass();

  // Destructor
  ~ekfIodClass();

  // Real-Time Model get method
  RT_MODEL_ekf_iod_T * getRTM();

  // private data and function members
 private:
  // Real-Time Model
  RT_MODEL_ekf_iod_T ekf_iod_M;
};

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'ekf_iod'
//  '<S1>'   : 'ekf_iod/ekf_iod'
//  '<S2>'   : 'ekf_iod/ekf_iod/Range Calibration'
//  '<S3>'   : 'ekf_iod/ekf_iod/ekf estimation'

#endif                                 // RTW_HEADER_ekf_iod_h_

//
// File trailer for generated code.
//
// [EOF]
//
