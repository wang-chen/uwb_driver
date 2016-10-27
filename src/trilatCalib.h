//
// File: trilatCalib.h
//
// Code generated for Simulink model 'trilatCalib'.
//
// Model version                  : 1.353
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Sat Oct 24 16:47:17 2015
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objectives:
//    1. Execution efficiency
//    2. Traceability
//    3. Safety precaution
// Validation result: Not run
//
#ifndef RTW_HEADER_trilatCalib_h_
#define RTW_HEADER_trilatCalib_h_
#include <math.h>
#include <stddef.h>
#ifndef trilatCalib_COMMON_INCLUDES_
# define trilatCalib_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 // trilatCalib_COMMON_INCLUDES_

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

// Forward declaration for rtModel
typedef struct tag_RTM_trilatCalib_T RT_MODEL_trilatCalib_T;

// Block signals (auto storage)
typedef struct {
  real_T a[12];
  real_T A[9];
  real_T a_m[9];
  real_T B[3];
} B_trilatCalib_T;

// Real-time Model Data Structure
struct tag_RTM_trilatCalib_T {
  const char_T * volatile errorStatus;
};

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

// Class declaration for model trilatCalib
class trilatCalibClass {
  // public data and function members
 public:
  // Block signals
  B_trilatCalib_T trilatCalib_B;

  // Model entry point functions

  // model initialize function
  void initialize();

  // model step function
  void step(real_T arg_ancs[12], real_T arg_dists[4], boolean_T arg_calibEnable,
            real_T arg_x_est[3]);

  // model terminate function
  void terminate();

  // Constructor
  trilatCalibClass();

  // Destructor
  ~trilatCalibClass();

  // Real-Time Model get method
  RT_MODEL_trilatCalib_T * getRTM();

  // private data and function members
 private:
  // Real-Time Model
  RT_MODEL_trilatCalib_T trilatCalib_M;
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
//  '<Root>' : 'trilatCalib'
//  '<S1>'   : 'trilatCalib/trilatCalib'
//  '<S2>'   : 'trilatCalib/trilatCalib/Calibrator'
//  '<S3>'   : 'trilatCalib/trilatCalib/trilat'

#endif                                 // RTW_HEADER_trilatCalib_h_

//
// File trailer for generated code.
//
// [EOF]
//
