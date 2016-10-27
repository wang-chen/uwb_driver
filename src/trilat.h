//
// File: trilat.h
//
// Code generated for Simulink model 'trilat'.
//
// Model version                  : 1.27
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Sat Sep 12 15:32:00 2015
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objective: Execution efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_trilat_h_
#define RTW_HEADER_trilat_h_
#include <math.h>
#include <stddef.h>
#ifndef trilat_COMMON_INCLUDES_
# define trilat_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 // trilat_COMMON_INCLUDES_

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

// Forward declaration for rtModel
typedef struct tag_RTM_trilat_T RT_MODEL_trilat_T;

// Real-time Model Data Structure
struct tag_RTM_trilat_T {
  const char_T * volatile errorStatus;
};

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

// Class declaration for model trilat
class trilatModelClass {
  // public data and function members
 public:
  // Model entry point functions

  // model initialize function
  void initialize();

  // model step function
  void step(const real_T arg_a[12], const real_T arg_d[4], real_T arg_p[3]);

  // model terminate function
  void terminate();

  // Constructor
  trilatModelClass();

  // Destructor
  ~trilatModelClass();

  // Real-Time Model get method
  RT_MODEL_trilat_T * getRTM();

  // private data and function members
 private:
  // Real-Time Model
  RT_MODEL_trilat_T trilat_M;
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
//  '<Root>' : 'trilat'
//  '<S1>'   : 'trilat/trilat'

#endif                                 // RTW_HEADER_trilat_h_

//
// File trailer for generated code.
//
// [EOF]
//
