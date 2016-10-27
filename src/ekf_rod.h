//
// File: ekf_rod.h
//
// Code generated for Simulink model 'ekf_rod'.
//
// Model version                  : 1.189
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Sun Sep 20 21:37:26 2015
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objective: Execution efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_ekf_rod_h_
#define RTW_HEADER_ekf_rod_h_
#include <stddef.h>
#include "rtwtypes.h"
#include <math.h>
#include <string.h>
#ifndef ekf_rod_COMMON_INCLUDES_
# define ekf_rod_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 // ekf_rod_COMMON_INCLUDES_

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

// Forward declaration for rtModel
typedef struct tag_RTM_ekf_rod_T RT_MODEL_ekf_rod_T;

// Block signals (auto storage)
typedef struct {
  real_T A[36];
  real_T P_pre[36];                    // '<S1>/EKF prediction'
  real_T P_upd[36];                    // '<S1>/EKF update'
  real_T A_m[36];
  real_T A_c[36];
  real_T dv0[36];
  real_T H[6];
  real_T K[6];
  real_T x_pre[6];                     // '<S1>/EKF prediction'
  real_T rtb_P_pre_k[6];
  int8_T I[36];
  real_T r_calibrated[4];              // '<S1>/Range Calibration'
  real_T dv1[3];
  real_T dv2[3];
  real_T sig_idx_0;
  real_T sig_idx_1;
  real_T sig_idx_2;
  real_T anchor_near_idx_0;
  real_T anchor_far_idx_0;
} B_ekf_rod_T;

// Block states (auto storage) for system '<Root>'
typedef struct {
  real_T UnitDelay1_DSTATE[6];         // '<S1>/Unit Delay1'
  real_T UnitDelay2_DSTATE[36];        // '<S1>/Unit Delay2'
  real_T R;                            // '<S1>/R'
  real_T acc_xy;                       // '<S1>/acc_xy'
  real_T acc_z;                        // '<S1>/acc_z'
  real_T ancs[12];                     // '<S1>/ancs'
} DW_ekf_rod_T;

// Parameters (auto storage)
struct P_ekf_rod_T_ {
  real_T P_0[36];                      // Variable: P_0
                                       //  Referenced by: '<S1>/Unit Delay2'

  real_T EKFOD_ancs[12];               // Mask Parameter: EKFOD_ancs
                                       //  Referenced by: '<S1>/ancs'

  real_T EKFOD_x_hat0[6];              // Mask Parameter: EKFOD_x_hat0
                                       //  Referenced by: '<S1>/Unit Delay1'

  real_T R_InitialValue;               // Expression: 0.2
                                       //  Referenced by: '<S1>/R'

  real_T acc_xy_InitialValue;          // Expression: 5
                                       //  Referenced by: '<S1>/acc_xy'

  real_T acc_z_InitialValue;           // Expression: 2
                                       //  Referenced by: '<S1>/acc_z'

};

// Parameters (auto storage)
typedef struct P_ekf_rod_T_ P_ekf_rod_T;

// Real-time Model Data Structure
struct tag_RTM_ekf_rod_T {
  const char_T * volatile errorStatus;
};

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

// Class declaration for model ekf_rod
class ekfModelClass {
  // public data and function members
 public:
  // Tunable parameters
  P_ekf_rod_T ekf_rod_P;

  // Block signals
  B_ekf_rod_T ekf_rod_B;

  // Block states
  DW_ekf_rod_T ekf_rod_DW;

  // Model entry point functions

  // model initialize function
  void initialize();

  // model step function
  void step(const real_T arg_dists[4], real_T arg_deltat, real_T arg_imu,
            uint8_T arg_nodeId, real_T arg_x_est[6], real_T *arg_outlier, real_T
            arg_forceUpd);

  // model terminate function
  void terminate();

  // Constructor
  ekfModelClass();

  // Destructor
  ~ekfModelClass();

  // Block parameters get method
  const P_ekf_rod_T & getBlockParameters() const;

  // Block parameters set method
  void setBlockParameters(const P_ekf_rod_T *pekf_rod_P);

  // Block signals get method
  const B_ekf_rod_T & getBlockSignals() const;

  // Block signals set method
  void setBlockSignals(const B_ekf_rod_T *pekf_rod_B);

  // Block states get method
  const DW_ekf_rod_T & getDWork() const;

  // Block states set method
  void setDWork(const DW_ekf_rod_T *pekf_rod_DW);

  // Real-Time Model get method
  RT_MODEL_ekf_rod_T * getRTM();

  // Real-Time Model set method
  void setRTM(const RT_MODEL_ekf_rod_T *pekf_rod_M);

  // private data and function members
 private:
  // Real-Time Model
  RT_MODEL_ekf_rod_T ekf_rod_M;

  // private member function(s) for subsystem '<Root>'
  real_T ekf_rod_norm(const real_T x[3]);
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
//  '<Root>' : 'ekf_rod'
//  '<S1>'   : 'ekf_rod/EKFOD'
//  '<S2>'   : 'ekf_rod/EKFOD/EKF prediction'
//  '<S3>'   : 'ekf_rod/EKFOD/EKF update'
//  '<S4>'   : 'ekf_rod/EKFOD/Outlier assessment'
//  '<S5>'   : 'ekf_rod/EKFOD/Range Calibration'

#endif                                 // RTW_HEADER_ekf_rod_h_

//
// File trailer for generated code.
//
// [EOF]
//
