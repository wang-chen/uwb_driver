//
// File: trilat.cpp
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
#include "trilat.h"

// Model step function
void trilatModelClass::step(const real_T arg_a[12], const real_T arg_d[4],
  real_T arg_p[3])
{
  real_T A[9];
  real_T B[3];
  int32_T r1;
  int32_T r2;
  int32_T r3;
  real_T maxval;
  real_T a21;
  int32_T rtemp;
  real_T tmp[9];

  // MATLAB Function: '<Root>/trilat' incorporates:
  //   Inport: '<Root>/a'
  //   Inport: '<Root>/d'

  // MATLAB Function 'trilat': '<S1>:1'
  // '<S1>:1:2' A = 2*[...
  // '<S1>:1:3'     a(2,:)-a(1,:);...
  // '<S1>:1:4'     a(3,:)-a(2,:);...
  // '<S1>:1:5'     a(4,:)-a(3,:)...
  // '<S1>:1:6'     ];
  tmp[0] = arg_a[1] - arg_a[0];
  tmp[3] = arg_a[5] - arg_a[4];
  tmp[6] = arg_a[9] - arg_a[8];
  tmp[1] = arg_a[2] - arg_a[1];
  tmp[4] = arg_a[6] - arg_a[5];
  tmp[7] = arg_a[10] - arg_a[9];
  tmp[2] = arg_a[3] - arg_a[2];
  tmp[5] = arg_a[7] - arg_a[6];
  tmp[8] = arg_a[11] - arg_a[10];
  for (r1 = 0; r1 < 3; r1++) {
    A[3 * r1] = tmp[3 * r1] * 2.0;
    A[1 + 3 * r1] = tmp[3 * r1 + 1] * 2.0;
    A[2 + 3 * r1] = tmp[3 * r1 + 2] * 2.0;
  }

  // '<S1>:1:8' B = [...
  // '<S1>:1:9'     d(1)^2 - d(2)^2 + a(2,:)*a(2,:)' - a(1,:)*a(1,:)';...
  // '<S1>:1:10'     d(2)^2 - d(3)^2 + a(3,:)*a(3,:)' - a(2,:)*a(2,:)';...
  // '<S1>:1:11'     d(3)^2 - d(4)^2 + a(4,:)*a(4,:)' - a(3,:)*a(3,:)'...
  // '<S1>:1:12'     ];
  B[0] = (((arg_a[1] * arg_a[1] + arg_a[5] * arg_a[5]) + arg_a[9] * arg_a[9]) +
          (arg_d[0] * arg_d[0] - arg_d[1] * arg_d[1])) - ((arg_a[0] * arg_a[0] +
    arg_a[4] * arg_a[4]) + arg_a[8] * arg_a[8]);
  B[1] = (((arg_a[2] * arg_a[2] + arg_a[6] * arg_a[6]) + arg_a[10] * arg_a[10])
          + (arg_d[1] * arg_d[1] - arg_d[2] * arg_d[2])) - ((arg_a[1] * arg_a[1]
    + arg_a[5] * arg_a[5]) + arg_a[9] * arg_a[9]);
  B[2] = (((arg_a[3] * arg_a[3] + arg_a[7] * arg_a[7]) + arg_a[11] * arg_a[11])
          + (arg_d[2] * arg_d[2] - arg_d[3] * arg_d[3])) - ((arg_a[2] * arg_a[2]
    + arg_a[6] * arg_a[6]) + arg_a[10] * arg_a[10]);

  // '<S1>:1:13' p = (A\B);
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = fabs(A[0]);
  a21 = fabs(A[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (fabs(A[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  A[r2] /= A[r1];
  A[r3] /= A[r1];
  A[3 + r2] -= A[3 + r1] * A[r2];
  A[3 + r3] -= A[3 + r1] * A[r3];
  A[6 + r2] -= A[6 + r1] * A[r2];
  A[6 + r3] -= A[6 + r1] * A[r3];
  if (fabs(A[3 + r3]) > fabs(A[3 + r2])) {
    rtemp = r2;
    r2 = r3;
    r3 = rtemp;
  }

  A[3 + r3] /= A[3 + r2];
  A[6 + r3] -= A[3 + r3] * A[6 + r2];
  maxval = B[r2] - B[r1] * A[r2];
  a21 = ((B[r3] - B[r1] * A[r3]) - A[3 + r3] * maxval) / A[6 + r3];
  maxval -= A[6 + r2] * a21;
  maxval /= A[3 + r2];

  // Outport: '<Root>/p' incorporates:
  //   MATLAB Function: '<Root>/trilat'

  arg_p[0] = ((B[r1] - A[6 + r1] * a21) - A[3 + r1] * maxval) / A[r1];
  arg_p[1] = maxval;
  arg_p[2] = a21;
}

// Model initialize function
void trilatModelClass::initialize()
{
  // Registration code

  // initialize error status
  rtmSetErrorStatus((&trilat_M), (NULL));
}

// Model terminate function
void trilatModelClass::terminate()
{
  // (no terminate code required)
}

// Constructor
trilatModelClass::trilatModelClass()
{
}

// Destructor
trilatModelClass::~trilatModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL_trilat_T * trilatModelClass::getRTM()
{
  return (&trilat_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
