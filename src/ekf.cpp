//
// File: ekf.cpp
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

#include "ekf.h"
#define NumBitsPerChar                 8U

extern real_T rt_powd_snf(real_T u0, real_T u1);
extern "C" {
  extern real_T rtGetInf(void);
  extern real32_T rtGetInfF(void);
  extern real_T rtGetMinusInf(void);
  extern real32_T rtGetMinusInfF(void);
}                                      // extern "C"
  extern "C"
{
  extern real_T rtGetNaN(void);
  extern real32_T rtGetNaNF(void);
}                                      // extern "C"

extern "C" {
  extern real_T rtInf;
  extern real_T rtMinusInf;
  extern real_T rtNaN;
  extern real32_T rtInfF;
  extern real32_T rtMinusInfF;
  extern real32_T rtNaNF;
  extern void rt_InitInfAndNaN(size_t realSize);
  extern boolean_T rtIsInf(real_T value);
  extern boolean_T rtIsInfF(real32_T value);
  extern boolean_T rtIsNaN(real_T value);
  extern boolean_T rtIsNaNF(real32_T value);
  typedef struct {
    struct {
      uint32_T wordH;
      uint32_T wordL;
    } words;
  } BigEndianIEEEDouble;

  typedef struct {
    struct {
      uint32_T wordL;
      uint32_T wordH;
    } words;
  } LittleEndianIEEEDouble;

  typedef struct {
    union {
      real32_T wordLreal;
      uint32_T wordLuint;
    } wordL;
  } IEEESingle;
}                                      // extern "C"
  extern "C"
{
  real_T rtInf;
  real_T rtMinusInf;
  real_T rtNaN;
  real32_T rtInfF;
  real32_T rtMinusInfF;
  real32_T rtNaNF;
}

//===========*
//  Constants *
// ===========
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F

//
//  UNUSED_PARAMETER(x)
//    Used to specify that a function parameter (argument) is required but not
//    accessed by the function body.
#ifndef UNUSED_PARAMETER
# if defined(__LCC__)
#   define UNUSED_PARAMETER(x)                                   // do nothing
# else

//
//  This is the semi-ANSI standard way of indicating that an
//  unused function parameter is required.
#   define UNUSED_PARAMETER(x)         (void) (x)
# endif
#endif

extern "C" {
  //
  // Initialize rtInf needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  real_T rtGetInf(void)
  {
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    real_T inf = 0.0;
    if (bitsPerReal == 32U) {
      inf = rtGetInfF();
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.bitVal.words.wordH = 0x7FF00000U;
      tmpVal.bitVal.words.wordL = 0x00000000U;
      inf = tmpVal.fltVal;
    }

    return inf;
  }

  //
  // Initialize rtInfF needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  real32_T rtGetInfF(void)
  {
    IEEESingle infF;
    infF.wordL.wordLuint = 0x7F800000U;
    return infF.wordL.wordLreal;
  }

  //
  // Initialize rtMinusInf needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  real_T rtGetMinusInf(void)
  {
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    real_T minf = 0.0;
    if (bitsPerReal == 32U) {
      minf = rtGetMinusInfF();
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.bitVal.words.wordH = 0xFFF00000U;
      tmpVal.bitVal.words.wordL = 0x00000000U;
      minf = tmpVal.fltVal;
    }

    return minf;
  }

  //
  // Initialize rtMinusInfF needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  real32_T rtGetMinusInfF(void)
  {
    IEEESingle minfF;
    minfF.wordL.wordLuint = 0xFF800000U;
    return minfF.wordL.wordLreal;
  }
}
  extern "C"
{
  //
  // Initialize rtNaN needed by the generated code.
  // NaN is initialized as non-signaling. Assumes IEEE.
  //
  real_T rtGetNaN(void)
  {
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    real_T nan = 0.0;
    if (bitsPerReal == 32U) {
      nan = rtGetNaNF();
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.bitVal.words.wordH = 0xFFF80000U;
      tmpVal.bitVal.words.wordL = 0x00000000U;
      nan = tmpVal.fltVal;
    }

    return nan;
  }

  //
  // Initialize rtNaNF needed by the generated code.
  // NaN is initialized as non-signaling. Assumes IEEE.
  //
  real32_T rtGetNaNF(void)
  {
    IEEESingle nanF = { { 0 } };

    nanF.wordL.wordLuint = 0xFFC00000U;
    return nanF.wordL.wordLreal;
  }
}

extern "C" {
  //
  // Initialize the rtInf, rtMinusInf, and rtNaN needed by the
  // generated code. NaN is initialized as non-signaling. Assumes IEEE.
  //
  void rt_InitInfAndNaN(size_t realSize)
  {
    (void) (realSize);
    rtNaN = rtGetNaN();
    rtNaNF = rtGetNaNF();
    rtInf = rtGetInf();
    rtInfF = rtGetInfF();
    rtMinusInf = rtGetMinusInf();
    rtMinusInfF = rtGetMinusInfF();
  }

  // Test if value is infinite
  boolean_T rtIsInf(real_T value)
  {
    return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
  }

  // Test if single-precision value is infinite
  boolean_T rtIsInfF(real32_T value)
  {
    return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
  }

  // Test if value is not a number
  boolean_T rtIsNaN(real_T value)
  {
    return (boolean_T)((value!=value) ? 1U : 0U);
  }

  // Test if single-precision value is not a number
  boolean_T rtIsNaNF(real32_T value)
  {
    return (boolean_T)(((value!=value) ? 1U : 0U));
  }
}
  real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T tmp;
  real_T tmp_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = (rtNaN);
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

// Model step function
void ekfModelClass::step(const real_T arg_dists[4], real_T arg_deltat, real_T
  arg_imu, uint8_T arg_nodeId, real_T arg_x_est[6])
{
  real_T dist_pre;
  real_T scale;
  real_T absxk;
  real_T t;
  int32_T i;
  int32_T i_0;
  int32_T i_1;
  UNUSED_PARAMETER(arg_imu);

  // MATLAB Function: '<S1>/EKF prediction' incorporates:
  //   Inport: '<Root>/deltat'
  //   UnitDelay: '<S1>/Unit Delay'
  //   UnitDelay: '<S1>/Unit Delay1'

  // MATLAB Function 'EKF/EKF prediction': '<S2>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6
  //  x_est: 6 by 1, pos & vel; P_est: 6 by 6, covariance at last update;
  //  deltat: time difference between 2 effective rangings or estimations
  //  parameters acc_xy & acc_z for producing Q
  // '<S2>:1:8' acc_xy = 5;
  //  acceleration magnitude of x-y
  // '<S2>:1:9' acc_z = 2;
  //  acceleration magnitude of z
  // '<S2>:1:11' A = eye(6);
  memset(&ekf_B.A[0], 0, 36U * sizeof(real_T));
  for (i = 0; i < 6; i++) {
    ekf_B.A[i + 6 * i] = 1.0;
  }

  // '<S2>:1:12' A(1,4) = deltat;
  ekf_B.A[18] = arg_deltat;

  // '<S2>:1:13' A(2,5) = deltat;
  ekf_B.A[25] = arg_deltat;

  // '<S2>:1:14' A(3,6) = deltat;
  ekf_B.A[32] = arg_deltat;

  // Q = zeros(6,6);
  // '<S2>:1:17' Q = [deltat^4/4*acc_xy^2            0                       0               deltat^3/2*acc_xy^2             0                       0;...
  // '<S2>:1:18'      0                     deltat^4/4*acc_xy^2              0                       0               deltat^3/2*acc_xy^2             0;...
  // '<S2>:1:19'      0                              0               deltat^4/4*acc_z^2              0                       0               deltat^3/2*acc_z^2;...
  // '<S2>:1:20'      deltat^3/2*acc_xy^2            0                       0               deltat^3/2*acc_xy^2             0                       0;...
  // '<S2>:1:21'      0                     deltat^3/2*acc_xy^2              0                       0               deltat^2*acc_xy^2               0;...
  // '<S2>:1:22'      0                              0               deltat^3/2*acc_z^2              0                       0               deltat^2*acc_z^2];
  //  Q(1,1) = deltat^4/4*acc_xy^2; Q(1,4) = deltat^3/2*acc_xy^2;
  //  Q(2,2) = deltat^4/4*acc_xy^2; Q(2,5) = deltat^3/2*acc_xy^2;
  //  Q(3,3) = deltat^4/4*acc_z^2; Q(3,6) = deltat^3/2*acc_z^2;
  //  Q(4,1) = deltat^3/2*acc_xy^2; Q(4,4) = deltat^2*acc_xy^2;
  //  Q(5,2) = deltat^3/2*acc_xy^2; Q(5,5) = deltat^2*acc_xy^2;
  //  Q(6,3) = deltat^3/2*acc_z^2; Q(6,6) = deltat^2*acc_z^2;
  // '<S2>:1:31' x_pre = A*x_est;
  // '<S2>:1:32' P_pre = A*P_est*A' + Q;
  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.A_m[i + 6 * i_0] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_B.A_m[i + 6 * i_0] += ekf_B.A[6 * i_1 + i] *
          ekf_DW.UnitDelay1_DSTATE[6 * i_0 + i_1];
      }
    }
  }

  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.A_c[i + 6 * i_0] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_B.A_c[i + 6 * i_0] += ekf_B.A_m[6 * i_1 + i] * ekf_B.A[6 * i_1 + i_0];
      }
    }
  }

  ekf_B.dv0[0] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * 25.0;
  ekf_B.dv0[6] = 0.0;
  ekf_B.dv0[12] = 0.0;
  ekf_B.dv0[18] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 25.0;
  ekf_B.dv0[24] = 0.0;
  ekf_B.dv0[30] = 0.0;
  ekf_B.dv0[1] = 0.0;
  ekf_B.dv0[7] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * 25.0;
  ekf_B.dv0[13] = 0.0;
  ekf_B.dv0[19] = 0.0;
  ekf_B.dv0[25] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 25.0;
  ekf_B.dv0[31] = 0.0;
  ekf_B.dv0[2] = 0.0;
  ekf_B.dv0[8] = 0.0;
  ekf_B.dv0[14] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * 4.0;
  ekf_B.dv0[20] = 0.0;
  ekf_B.dv0[26] = 0.0;
  ekf_B.dv0[32] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 4.0;
  ekf_B.dv0[3] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 25.0;
  ekf_B.dv0[9] = 0.0;
  ekf_B.dv0[15] = 0.0;
  ekf_B.dv0[21] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 25.0;
  ekf_B.dv0[27] = 0.0;
  ekf_B.dv0[33] = 0.0;
  ekf_B.dv0[4] = 0.0;
  ekf_B.dv0[10] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 25.0;
  ekf_B.dv0[16] = 0.0;
  ekf_B.dv0[22] = 0.0;
  ekf_B.dv0[28] = arg_deltat * arg_deltat * 25.0;
  ekf_B.dv0[34] = 0.0;
  ekf_B.dv0[5] = 0.0;
  ekf_B.dv0[11] = 0.0;
  ekf_B.dv0[17] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * 4.0;
  ekf_B.dv0[23] = 0.0;
  ekf_B.dv0[29] = 0.0;
  ekf_B.dv0[35] = arg_deltat * arg_deltat * 4.0;
  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.P_pre[i_0 + 6 * i] = ekf_B.A_c[6 * i + i_0] + ekf_B.dv0[6 * i + i_0];
    }
  }

  for (i = 0; i < 6; i++) {
    ekf_B.x_pre[i] = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.x_pre[i] += ekf_B.A[6 * i_0 + i] * ekf_DW.UnitDelay_DSTATE[i_0];
    }
  }

  // End of MATLAB Function: '<S1>/EKF prediction'

  // MATLAB Function: '<S1>/EKF update' incorporates:
  //   Inport: '<Root>/dists'
  //   Inport: '<Root>/nodeId'

  // MATLAB Function 'EKF/EKF update': '<S3>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6, predicted x and covariance
  //  x_upd: 6 by 1, pos & vel; P_upd: 6 by 6, updated x and covariance;
  // '<S3>:1:9' dist_pre = norm(ancs(:,nodeId) - x_pre(1:3), 2);
  scale = 2.2250738585072014E-308;
  absxk = fabs(ekf_DW.ancs[(arg_nodeId - 1) * 3] - ekf_B.x_pre[0]);
  if (absxk > 2.2250738585072014E-308) {
    dist_pre = 1.0;
    scale = absxk;
  } else {
    t = absxk / 2.2250738585072014E-308;
    dist_pre = t * t;
  }

  absxk = fabs(ekf_DW.ancs[(arg_nodeId - 1) * 3 + 1] - ekf_B.x_pre[1]);
  if (absxk > scale) {
    t = scale / absxk;
    dist_pre = dist_pre * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    dist_pre += t * t;
  }

  absxk = fabs(ekf_DW.ancs[(arg_nodeId - 1) * 3 + 2] - ekf_B.x_pre[2]);
  if (absxk > scale) {
    t = scale / absxk;
    dist_pre = dist_pre * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    dist_pre += t * t;
  }

  dist_pre = scale * sqrt(dist_pre);

  // '<S3>:1:10' H = zeros(1,6);
  for (i = 0; i < 6; i++) {
    ekf_B.H[i] = 0.0;
  }

  // H is
  // '<S3>:1:11' H(1:3) = (x_pre(1:3) - ancs(:,nodeId))/dist_pre;
  ekf_B.H[0] = (ekf_B.x_pre[0] - ekf_DW.ancs[(arg_nodeId - 1) * 3]) / dist_pre;
  ekf_B.H[1] = (ekf_B.x_pre[1] - ekf_DW.ancs[(arg_nodeId - 1) * 3 + 1]) /
    dist_pre;
  ekf_B.H[2] = (ekf_B.x_pre[2] - ekf_DW.ancs[(arg_nodeId - 1) * 3 + 2]) /
    dist_pre;

  // '<S3>:1:12' K = P_pre*H'*( H*P_pre*H'+ R)^-1;
  scale = 0.0;
  for (i = 0; i < 6; i++) {
    ekf_B.K[i] = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.K[i] += ekf_B.P_pre[6 * i + i_0] * ekf_B.H[i_0];
    }

    scale += ekf_B.K[i] * ekf_B.H[i];
    ekf_B.rtb_P_pre_k[i] = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.rtb_P_pre_k[i] += ekf_B.P_pre[6 * i_0 + i] * ekf_B.H[i_0];
    }
  }

  scale = 1.0 / (scale + ekf_DW.R);
  for (i = 0; i < 6; i++) {
    ekf_B.K[i] = ekf_B.rtb_P_pre_k[i] * scale;
  }

  // '<S3>:1:14' x_upd = x_pre + K*(dist(nodeId) - dist_pre);
  scale = arg_dists[arg_nodeId - 1] - dist_pre;
  for (i = 0; i < 6; i++) {
    ekf_B.x_pre[i] += ekf_B.K[i] * scale;
  }

  // '<S3>:1:15' P_upd = (eye(6) - K*H)*P_pre;
  for (i = 0; i < 36; i++) {
    ekf_B.I[i] = 0;
  }

  for (i = 0; i < 6; i++) {
    ekf_B.I[i + 6 * i] = 1;
  }

  for (i = 0; i < 6; i++) {
    // Outport: '<Root>/x_est'
    arg_x_est[i] = ekf_B.x_pre[i];

    // Update for UnitDelay: '<S1>/Unit Delay'
    ekf_DW.UnitDelay_DSTATE[i] = ekf_B.x_pre[i];
  }

  // MATLAB Function: '<S1>/EKF update'
  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_B.A[i + 6 * i_0] = (real_T)ekf_B.I[6 * i_0 + i] - ekf_B.K[i] *
        ekf_B.H[i_0];
    }
  }

  // Update for UnitDelay: '<S1>/Unit Delay1' incorporates:
  //   MATLAB Function: '<S1>/EKF update'
  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_DW.UnitDelay1_DSTATE[i + 6 * i_0] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_DW.UnitDelay1_DSTATE[i + 6 * i_0] += ekf_B.A[6 * i_1 + i] *
          ekf_B.P_pre[6 * i_0 + i_1];
      }
    }
  }

  // End of Update for UnitDelay: '<S1>/Unit Delay1'
}

// Model initialize function
void ekfModelClass::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // Start for DataStoreMemory: '<S1>/R'
  ekf_DW.R = ekf_P.R_InitialValue;

  // Start for DataStoreMemory: '<S1>/ancs'
  memcpy(&ekf_DW.ancs[0], &ekf_P.ancs_InitialValue[0], 12U * sizeof(real_T));

  {
    int32_T i;

    // InitializeConditions for UnitDelay: '<S1>/Unit Delay'
    for (i = 0; i < 6; i++) {
      ekf_DW.UnitDelay_DSTATE[i] = ekf_P.EKF_x_hat0[i];
    }

    // End of InitializeConditions for UnitDelay: '<S1>/Unit Delay'

    // InitializeConditions for UnitDelay: '<S1>/Unit Delay1'
    memcpy(&ekf_DW.UnitDelay1_DSTATE[0], &ekf_P.P_0[0], 36U * sizeof(real_T));
  }
}

// Model terminate function
void ekfModelClass::terminate()
{
  // (no terminate code required)
}

// Constructor
ekfModelClass::ekfModelClass()
{
  P_ekf_T ekf_P_temp = {
    //  Variable: P_0

    //  Referenced by: '<S1>/Unit Delay1'
    { 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },

    //  Mask Parameter: EKF_x_hat0

    //  Referenced by: '<S1>/Unit Delay'
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
    0.2,                               // Expression: 0.2

    //  Referenced by: '<S1>/R'

    //  Expression: [-3.0, 3.0, 3.0, -3.0; -3.0, -3.0, 3.0, 3.0; -1.78, -1.17, -1.31, -1.31]

    //  Referenced by: '<S1>/ancs'
    { -3.0, -3.0, -1.78, 3.0, -3.0, -1.17, 3.0, 3.0, -1.31, -3.0, 3.0, -1.31 }
  };                                   // Modifiable parameters

  // Initialize tunable parameters
  ekf_P = ekf_P_temp;
}

// Destructor
ekfModelClass::~ekfModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL_ekf_T * ekfModelClass::getRTM()
{
  return (&ekf_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
