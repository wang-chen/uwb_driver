//
// File: ekf.h
//
// Code generated for Simulink model 'ekf'.
//
// Model version                  : 1.118
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Sat Sep 12 21:56:16 2015
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objective: Execution efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_ekf_h_
#define RTW_HEADER_ekf_h_
#include <stddef.h>
#include "rtwtypes.h"
#include <math.h>
#include <string.h>
#ifndef ekf_COMMON_INCLUDES_
# define ekf_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 // ekf_COMMON_INCLUDES_

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

// Forward declaration for rtModel
typedef struct tag_RTM_ekf_T RT_MODEL_ekf_T;

// Block signals (auto storage)
typedef struct {
  real_T A[36];
  real_T P_pre[36];                    // '<S1>/EKF prediction'
  real_T A_m[36];
  real_T A_c[36];
  real_T dv0[36];
  real_T H[6];
  real_T K[6];
  real_T x_pre[6];                     // '<S1>/EKF prediction'
  real_T rtb_P_pre_k[6];
  int8_T I[36];
} B_ekf_T;

// Block states (auto storage) for system '<Root>'
typedef struct {
  real_T UnitDelay_DSTATE[6];          // '<S1>/Unit Delay'
  real_T UnitDelay1_DSTATE[36];        // '<S1>/Unit Delay1'
  real_T R;                            // '<S1>/R'
  real_T ancs[12];                     // '<S1>/ancs'
} DW_ekf_T;

// Parameters (auto storage)
struct P_ekf_T_ {
  real_T P_0[36];                      // Variable: P_0
                                       //  Referenced by: '<S1>/Unit Delay1'

  real_T EKF_x_hat0[6];                // Mask Parameter: EKF_x_hat0
                                       //  Referenced by: '<S1>/Unit Delay'

  real_T R_InitialValue;               // Expression: 0.2
                                       //  Referenced by: '<S1>/R'

  real_T ancs_InitialValue[12];        // Expression: [-3.0, 3.0, 3.0, -3.0; -3.0, -3.0, 3.0, 3.0; -1.78, -1.17, -1.31, -1.31]
                                       //  Referenced by: '<S1>/ancs'

};

// Parameters (auto storage)
typedef struct P_ekf_T_ P_ekf_T;

// Real-time Model Data Structure
struct tag_RTM_ekf_T {
  const char_T * volatile errorStatus;
};

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

// Class declaration for model ekf
class ekfModelClass {
  // public data and function members
 public:
  // Tunable parameters
  P_ekf_T ekf_P;

  // Block signals
  B_ekf_T ekf_B;

  // Block states
  DW_ekf_T ekf_DW;

  // Model entry point functions

  // model initialize function
  void initialize();

  // model step function
  void step(const real_T arg_dists[4], real_T arg_deltat, real_T arg_imu,
            uint8_T arg_nodeId, real_T arg_x_est[6]);

  // model terminate function
  void terminate();

  // Constructor
  ekfModelClass();

  // Destructor
  ~ekfModelClass();

  // Real-Time Model get method
  RT_MODEL_ekf_T * getRTM();

  // private data and function members
 private:
  // Real-Time Model
  RT_MODEL_ekf_T ekf_M;
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
//  '<Root>' : 'ekf'
//  '<S1>'   : 'ekf/EKF'
//  '<S2>'   : 'ekf/EKF/EKF prediction'
//  '<S3>'   : 'ekf/EKF/EKF update'

#endif                                 // RTW_HEADER_ekf_h_

//
// File trailer for generated code.
//
// [EOF]
//
