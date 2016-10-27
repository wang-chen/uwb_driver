//
// File: ekf_rod.cpp
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

#include "ekf_rod.h"
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

// Function for MATLAB Function: '<S1>/Outlier assessment'
real_T ekfModelClass::ekf_rod_norm(const real_T x[3])
{
  real_T y;
  real_T scale;
  real_T absxk;
  real_T t;
  scale = 2.2250738585072014E-308;
  absxk = fabs(x[0]);
  if (absxk > 2.2250738585072014E-308) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 2.2250738585072014E-308;
    y = t * t;
  }

  absxk = fabs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = fabs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  return scale * sqrt(y);
}

// Model step function
void ekfModelClass::step(const real_T arg_dists[4], real_T arg_deltat, real_T
  arg_imu, uint8_T arg_nodeId, real_T arg_x_est[6], real_T *arg_outlier, real_T
  arg_forceUpd)
{
  int32_T i;
  int32_T i_0;
  int32_T i_1;
  real_T anchor_near_idx_1;
  real_T anchor_far_idx_1;
  real_T anchor_near_idx_2;
  UNUSED_PARAMETER(arg_imu);

  // Outport: '<Root>/x_est' incorporates:
  //   UnitDelay: '<S1>/Unit Delay1'
  for (i = 0; i < 6; i++) {
    arg_x_est[i] = ekf_rod_DW.UnitDelay1_DSTATE[i];
  }

  // End of Outport: '<Root>/x_est'

  // MATLAB Function: '<S1>/Range Calibration' incorporates:
  //   Inport: '<Root>/dists'
  //   Inport: '<Root>/nodeId'

  // MATLAB Function 'EKFOD/Range Calibration': '<S5>:1'
  // '<S5>:1:3' r_calibrated = dists;
  ekf_rod_B.r_calibrated[0] = arg_dists[0];
  ekf_rod_B.r_calibrated[1] = arg_dists[1];
  ekf_rod_B.r_calibrated[2] = arg_dists[2];
  ekf_rod_B.r_calibrated[3] = arg_dists[3];

  // '<S5>:1:4' if 0 < dists(nodeId) && dists(nodeId) < 1.5
  if ((0.0 < arg_dists[arg_nodeId - 1]) && (arg_dists[arg_nodeId - 1] < 1.5)) {
    // '<S5>:1:5' r_calibrated(nodeId) = 1.0447 * dists(nodeId) - 0.1932;
    ekf_rod_B.r_calibrated[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] * 1.0447
      - 0.1932;
  } else {
    // '<S5>:1:6' else
    // '<S5>:1:7' if dists(nodeId) > 1.5 && dists(nodeId) <= 10
    if ((arg_dists[arg_nodeId - 1] > 1.5) && (arg_dists[arg_nodeId - 1] <= 10.0))
    {
      // '<S5>:1:8' r_calibrated(nodeId) = 1.0029 * dists(nodeId) - 0.0829;
      ekf_rod_B.r_calibrated[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
        1.0029 - 0.0829;
    } else {
      // '<S5>:1:9' else
      // '<S5>:1:10' r_calibrated(nodeId) = 0.9976 * dists(nodeId) - 0.0511;
      ekf_rod_B.r_calibrated[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
        0.9976 - 0.0511;
    }
  }

  // End of MATLAB Function: '<S1>/Range Calibration'

  // MATLAB Function: '<S1>/EKF prediction' incorporates:
  //   Inport: '<Root>/deltat'
  //   UnitDelay: '<S1>/Unit Delay1'
  //   UnitDelay: '<S1>/Unit Delay2'

  // MATLAB Function 'EKFOD/EKF prediction': '<S2>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6
  //  x_est: 6 by 1, pos & vel; P_est: 6 by 6, covariance at last update;
  //  deltat: time difference between 2 effective rangings or estimations
  //  parameters acc_xy & acc_z for producing Q
  //  acceleration magnitude of x-y
  //  acceleration magnitude of z
  // '<S2>:1:11' A = eye(6);
  memset(&ekf_rod_B.A[0], 0, 36U * sizeof(real_T));
  for (i = 0; i < 6; i++) {
    ekf_rod_B.A[i + 6 * i] = 1.0;
  }

  // '<S2>:1:12' A(1,4) = deltat;
  ekf_rod_B.A[18] = arg_deltat;

  // '<S2>:1:13' A(2,5) = deltat;
  ekf_rod_B.A[25] = arg_deltat;

  // '<S2>:1:14' A(3,6) = deltat;
  ekf_rod_B.A[32] = arg_deltat;

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
      ekf_rod_B.A_m[i + 6 * i_0] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_rod_B.A_m[i + 6 * i_0] += ekf_rod_B.A[6 * i_1 + i] *
          ekf_rod_DW.UnitDelay2_DSTATE[6 * i_0 + i_1];
      }
    }
  }

  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_rod_B.A_c[i + 6 * i_0] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_rod_B.A_c[i + 6 * i_0] += ekf_rod_B.A_m[6 * i_1 + i] * ekf_rod_B.A[6
          * i_1 + i_0];
      }
    }
  }

  ekf_rod_B.dv0[0] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[6] = 0.0;
  ekf_rod_B.dv0[12] = 0.0;
  ekf_rod_B.dv0[18] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[24] = 0.0;
  ekf_rod_B.dv0[30] = 0.0;
  ekf_rod_B.dv0[1] = 0.0;
  ekf_rod_B.dv0[7] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[13] = 0.0;
  ekf_rod_B.dv0[19] = 0.0;
  ekf_rod_B.dv0[25] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[31] = 0.0;
  ekf_rod_B.dv0[2] = 0.0;
  ekf_rod_B.dv0[8] = 0.0;
  ekf_rod_B.dv0[14] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * (ekf_rod_DW.acc_z *
    ekf_rod_DW.acc_z);
  ekf_rod_B.dv0[20] = 0.0;
  ekf_rod_B.dv0[26] = 0.0;
  ekf_rod_B.dv0[32] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_z *
    ekf_rod_DW.acc_z);
  ekf_rod_B.dv0[3] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[9] = 0.0;
  ekf_rod_B.dv0[15] = 0.0;
  ekf_rod_B.dv0[21] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[27] = 0.0;
  ekf_rod_B.dv0[33] = 0.0;
  ekf_rod_B.dv0[4] = 0.0;
  ekf_rod_B.dv0[10] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[16] = 0.0;
  ekf_rod_B.dv0[22] = 0.0;
  ekf_rod_B.dv0[28] = arg_deltat * arg_deltat * (ekf_rod_DW.acc_xy *
    ekf_rod_DW.acc_xy);
  ekf_rod_B.dv0[34] = 0.0;
  ekf_rod_B.dv0[5] = 0.0;
  ekf_rod_B.dv0[11] = 0.0;
  ekf_rod_B.dv0[17] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_rod_DW.acc_z *
    ekf_rod_DW.acc_z);
  ekf_rod_B.dv0[23] = 0.0;
  ekf_rod_B.dv0[29] = 0.0;
  ekf_rod_B.dv0[35] = arg_deltat * arg_deltat * (ekf_rod_DW.acc_z *
    ekf_rod_DW.acc_z);
  for (i = 0; i < 6; i++) {
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_rod_B.P_pre[i_0 + 6 * i] = ekf_rod_B.A_c[6 * i + i_0] + ekf_rod_B.dv0
        [6 * i + i_0];
    }
  }

  for (i = 0; i < 6; i++) {
    ekf_rod_B.x_pre[i] = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      ekf_rod_B.x_pre[i] += ekf_rod_B.A[6 * i_0 + i] *
        ekf_rod_DW.UnitDelay1_DSTATE[i_0];
    }
  }

  // End of MATLAB Function: '<S1>/EKF prediction'

  // MATLAB Function: '<S1>/Outlier assessment' incorporates:
  //   Inport: '<Root>/nodeId'

  // MATLAB Function 'EKFOD/Outlier assessment': '<S4>:1'
  //  r: ranging
  //  anchor: anchor position 3 by 1
  //  pos_pre: predicted position
  //  P_pre: predicted covariance of the position
  // '<S4>:1:9' y = 0;
  i = 0;

  // '<S4>:1:11' sig_x = sqrt(P_pre(1,1));
  // '<S4>:1:12' sig_y = sqrt(P_pre(2,2));
  // '<S4>:1:13' sig_z = sqrt(P_pre(3,3));
  // '<S4>:1:15' sig = 2*[sig_x;sig_y;sig_z];
  ekf_rod_B.sig_idx_0 = 2.0 * sqrt(ekf_rod_B.P_pre[0]);
  ekf_rod_B.sig_idx_1 = 2.0 * sqrt(ekf_rod_B.P_pre[7]);
  ekf_rod_B.sig_idx_2 = 2.0 * sqrt(ekf_rod_B.P_pre[14]);

  // '<S4>:1:17' anchor_near = zeros(3,1);
  //  nearest point on the covariance cube from the anchor
  // '<S4>:1:18' anchor_far = zeros(3,1);
  //  farthest point on the covariance cube from the anchor
  // '<S4>:1:20' for i = 1:3
  // '<S4>:1:21' if ancs(i, nodeId) > x_pre(i) + sig(i)
  if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3] > ekf_rod_B.x_pre[0] +
      ekf_rod_B.sig_idx_0) {
    // '<S4>:1:22' anchor_near(i) = x_pre(i) + sig(i);
    ekf_rod_B.anchor_near_idx_0 = ekf_rod_B.x_pre[0] + ekf_rod_B.sig_idx_0;

    // '<S4>:1:23' anchor_far(i) = x_pre(i) - sig(i);
    ekf_rod_B.anchor_far_idx_0 = ekf_rod_B.x_pre[0] - ekf_rod_B.sig_idx_0;
  } else {
    // '<S4>:1:24' else
    // '<S4>:1:24' if ancs(i, nodeId) < x_pre(i) - sig(i)
    if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3] < ekf_rod_B.x_pre[0] -
        ekf_rod_B.sig_idx_0) {
      // '<S4>:1:25' anchor_near(i) = x_pre(i) - sig(i);
      ekf_rod_B.anchor_near_idx_0 = ekf_rod_B.x_pre[0] - ekf_rod_B.sig_idx_0;

      // '<S4>:1:26' anchor_far(i) = x_pre(i) + sig(i);
      ekf_rod_B.anchor_far_idx_0 = ekf_rod_B.x_pre[0] + ekf_rod_B.sig_idx_0;
    } else {
      // '<S4>:1:27' else
      // '<S4>:1:28' anchor_near(i) = ancs(i, nodeId);
      ekf_rod_B.anchor_near_idx_0 = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3];

      // '<S4>:1:29' if ancs(i, nodeId) > x_pre(i)
      if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3] > ekf_rod_B.x_pre[0]) {
        // '<S4>:1:30' anchor_far(i) =  x_pre(i) - sig(i);
        ekf_rod_B.anchor_far_idx_0 = ekf_rod_B.x_pre[0] - ekf_rod_B.sig_idx_0;
      } else {
        // '<S4>:1:31' else
        // '<S4>:1:32' anchor_far(i) =  x_pre(i) + sig(i);
        ekf_rod_B.anchor_far_idx_0 = ekf_rod_B.x_pre[0] + ekf_rod_B.sig_idx_0;
      }
    }
  }

  // '<S4>:1:21' if ancs(i, nodeId) > x_pre(i) + sig(i)
  if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1] > ekf_rod_B.x_pre[1] +
      ekf_rod_B.sig_idx_1) {
    // '<S4>:1:22' anchor_near(i) = x_pre(i) + sig(i);
    anchor_near_idx_1 = ekf_rod_B.x_pre[1] + ekf_rod_B.sig_idx_1;

    // '<S4>:1:23' anchor_far(i) = x_pre(i) - sig(i);
    anchor_far_idx_1 = ekf_rod_B.x_pre[1] - ekf_rod_B.sig_idx_1;
  } else {
    // '<S4>:1:24' else
    // '<S4>:1:24' if ancs(i, nodeId) < x_pre(i) - sig(i)
    if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1] < ekf_rod_B.x_pre[1] -
        ekf_rod_B.sig_idx_1) {
      // '<S4>:1:25' anchor_near(i) = x_pre(i) - sig(i);
      anchor_near_idx_1 = ekf_rod_B.x_pre[1] - ekf_rod_B.sig_idx_1;

      // '<S4>:1:26' anchor_far(i) = x_pre(i) + sig(i);
      anchor_far_idx_1 = ekf_rod_B.x_pre[1] + ekf_rod_B.sig_idx_1;
    } else {
      // '<S4>:1:27' else
      // '<S4>:1:28' anchor_near(i) = ancs(i, nodeId);
      anchor_near_idx_1 = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1];

      // '<S4>:1:29' if ancs(i, nodeId) > x_pre(i)
      if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1] > ekf_rod_B.x_pre[1]) {
        // '<S4>:1:30' anchor_far(i) =  x_pre(i) - sig(i);
        anchor_far_idx_1 = ekf_rod_B.x_pre[1] - ekf_rod_B.sig_idx_1;
      } else {
        // '<S4>:1:31' else
        // '<S4>:1:32' anchor_far(i) =  x_pre(i) + sig(i);
        anchor_far_idx_1 = ekf_rod_B.x_pre[1] + ekf_rod_B.sig_idx_1;
      }
    }
  }

  // '<S4>:1:21' if ancs(i, nodeId) > x_pre(i) + sig(i)
  if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2] > ekf_rod_B.x_pre[2] +
      ekf_rod_B.sig_idx_2) {
    // '<S4>:1:22' anchor_near(i) = x_pre(i) + sig(i);
    anchor_near_idx_2 = ekf_rod_B.x_pre[2] + ekf_rod_B.sig_idx_2;

    // '<S4>:1:23' anchor_far(i) = x_pre(i) - sig(i);
    ekf_rod_B.sig_idx_0 = ekf_rod_B.x_pre[2] - ekf_rod_B.sig_idx_2;
  } else {
    // '<S4>:1:24' else
    // '<S4>:1:24' if ancs(i, nodeId) < x_pre(i) - sig(i)
    if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2] < ekf_rod_B.x_pre[2] -
        ekf_rod_B.sig_idx_2) {
      // '<S4>:1:25' anchor_near(i) = x_pre(i) - sig(i);
      anchor_near_idx_2 = ekf_rod_B.x_pre[2] - ekf_rod_B.sig_idx_2;

      // '<S4>:1:26' anchor_far(i) = x_pre(i) + sig(i);
      ekf_rod_B.sig_idx_0 = ekf_rod_B.x_pre[2] + ekf_rod_B.sig_idx_2;
    } else {
      // '<S4>:1:27' else
      // '<S4>:1:28' anchor_near(i) = ancs(i, nodeId);
      anchor_near_idx_2 = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2];

      // '<S4>:1:29' if ancs(i, nodeId) > x_pre(i)
      if (ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2] > ekf_rod_B.x_pre[2]) {
        // '<S4>:1:30' anchor_far(i) =  x_pre(i) - sig(i);
        ekf_rod_B.sig_idx_0 = ekf_rod_B.x_pre[2] - ekf_rod_B.sig_idx_2;
      } else {
        // '<S4>:1:31' else
        // '<S4>:1:32' anchor_far(i) =  x_pre(i) + sig(i);
        ekf_rod_B.sig_idx_0 = ekf_rod_B.x_pre[2] + ekf_rod_B.sig_idx_2;
      }
    }
  }

  // '<S4>:1:38' r_min = norm(ancs(:,nodeId) - anchor_near);
  // '<S4>:1:39' r_max = norm(ancs(:,nodeId) - anchor_far);
  // '<S4>:1:40' if r(nodeId) <= r_min || r_max <= r(nodeId)
  ekf_rod_B.dv1[0] = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3] -
    ekf_rod_B.anchor_near_idx_0;
  ekf_rod_B.dv1[1] = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1] -
    anchor_near_idx_1;
  ekf_rod_B.dv1[2] = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2] -
    anchor_near_idx_2;
  ekf_rod_B.dv2[0] = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3] -
    ekf_rod_B.anchor_far_idx_0;
  ekf_rod_B.dv2[1] = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1] -
    anchor_far_idx_1;
  ekf_rod_B.dv2[2] = ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2] -
    ekf_rod_B.sig_idx_0;
  if ((ekf_rod_B.r_calibrated[arg_nodeId - 1] <= ekf_rod_norm(ekf_rod_B.dv1)) ||
      (ekf_rod_norm(ekf_rod_B.dv2) <= ekf_rod_B.r_calibrated[arg_nodeId - 1])) {
    // '<S4>:1:41' y = 1;
    i = 1;

    //  outlier detected
  }

  // Outport: '<Root>/outlier' incorporates:
  //   MATLAB Function: '<S1>/Outlier assessment'
  *arg_outlier = (real_T)i;

  // MATLAB Function: '<S1>/EKF update' incorporates:
  //   Inport: '<Root>/forceUpd'
  //   Inport: '<Root>/nodeId'
  //   MATLAB Function: '<S1>/Outlier assessment'
  //   Sum: '<S1>/Sum'
  //   UnitDelay: '<S1>/Unit Delay1'
  //   UnitDelay: '<S1>/Unit Delay2'

  // MATLAB Function 'EKFOD/EKF update': '<S3>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6, predicted x and covariance
  //  x_upd: 6 by 1, pos & vel; P_upd: 6 by 6, updated x and covariance;
  // '<S3>:1:9' if outlierDetected == 0
  if ((real_T)i + arg_forceUpd == 0.0) {
    // '<S3>:1:10' dist_pre = norm(ancs(:,nodeId) - x_pre(1:3), 2);
    ekf_rod_B.sig_idx_0 = 2.2250738585072014E-308;
    ekf_rod_B.sig_idx_1 = fabs(ekf_rod_DW.ancs[(arg_nodeId - 1) * 3] -
      ekf_rod_B.x_pre[0]);
    if (ekf_rod_B.sig_idx_1 > 2.2250738585072014E-308) {
      anchor_near_idx_2 = 1.0;
      ekf_rod_B.sig_idx_0 = ekf_rod_B.sig_idx_1;
    } else {
      anchor_far_idx_1 = ekf_rod_B.sig_idx_1 / 2.2250738585072014E-308;
      anchor_near_idx_2 = anchor_far_idx_1 * anchor_far_idx_1;
    }

    ekf_rod_B.sig_idx_1 = fabs(ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 1] -
      ekf_rod_B.x_pre[1]);
    if (ekf_rod_B.sig_idx_1 > ekf_rod_B.sig_idx_0) {
      anchor_far_idx_1 = ekf_rod_B.sig_idx_0 / ekf_rod_B.sig_idx_1;
      anchor_near_idx_2 = anchor_near_idx_2 * anchor_far_idx_1 *
        anchor_far_idx_1 + 1.0;
      ekf_rod_B.sig_idx_0 = ekf_rod_B.sig_idx_1;
    } else {
      anchor_far_idx_1 = ekf_rod_B.sig_idx_1 / ekf_rod_B.sig_idx_0;
      anchor_near_idx_2 += anchor_far_idx_1 * anchor_far_idx_1;
    }

    ekf_rod_B.sig_idx_1 = fabs(ekf_rod_DW.ancs[(arg_nodeId - 1) * 3 + 2] -
      ekf_rod_B.x_pre[2]);
    if (ekf_rod_B.sig_idx_1 > ekf_rod_B.sig_idx_0) {
      anchor_far_idx_1 = ekf_rod_B.sig_idx_0 / ekf_rod_B.sig_idx_1;
      anchor_near_idx_2 = anchor_near_idx_2 * anchor_far_idx_1 *
        anchor_far_idx_1 + 1.0;
      ekf_rod_B.sig_idx_0 = ekf_rod_B.sig_idx_1;
    } else {
      anchor_far_idx_1 = ekf_rod_B.sig_idx_1 / ekf_rod_B.sig_idx_0;
      anchor_near_idx_2 += anchor_far_idx_1 * anchor_far_idx_1;
    }

    anchor_near_idx_2 = ekf_rod_B.sig_idx_0 * sqrt(anchor_near_idx_2);

    // '<S3>:1:11' H = zeros(1,6);
    for (i = 0; i < 6; i++) {
      ekf_rod_B.H[i] = 0.0;
    }

    // H is
    // '<S3>:1:12' H(1:3) = (x_pre(1:3) - ancs(:,nodeId))/dist_pre;
    ekf_rod_B.H[0] = (ekf_rod_B.x_pre[0] - ekf_rod_DW.ancs[(arg_nodeId - 1) * 3])
      / anchor_near_idx_2;
    ekf_rod_B.H[1] = (ekf_rod_B.x_pre[1] - ekf_rod_DW.ancs[(arg_nodeId - 1) * 3
                      + 1]) / anchor_near_idx_2;
    ekf_rod_B.H[2] = (ekf_rod_B.x_pre[2] - ekf_rod_DW.ancs[(arg_nodeId - 1) * 3
                      + 2]) / anchor_near_idx_2;

    // '<S3>:1:13' K = P_pre*H'*( H*P_pre*H'+ R)^-1;
    ekf_rod_B.sig_idx_0 = 0.0;
    for (i = 0; i < 6; i++) {
      ekf_rod_B.K[i] = 0.0;
      for (i_0 = 0; i_0 < 6; i_0++) {
        ekf_rod_B.K[i] += ekf_rod_B.P_pre[6 * i + i_0] * ekf_rod_B.H[i_0];
      }

      ekf_rod_B.sig_idx_0 += ekf_rod_B.K[i] * ekf_rod_B.H[i];
      ekf_rod_B.rtb_P_pre_k[i] = 0.0;
      for (i_0 = 0; i_0 < 6; i_0++) {
        ekf_rod_B.rtb_P_pre_k[i] += ekf_rod_B.P_pre[6 * i_0 + i] *
          ekf_rod_B.H[i_0];
      }
    }

    ekf_rod_B.sig_idx_0 = 1.0 / (ekf_rod_B.sig_idx_0 + ekf_rod_DW.R);
    for (i = 0; i < 6; i++) {
      ekf_rod_B.K[i] = ekf_rod_B.rtb_P_pre_k[i] * ekf_rod_B.sig_idx_0;
    }

    // '<S3>:1:15' x_upd = x_pre + K*(dists(nodeId) - dist_pre);
    ekf_rod_B.sig_idx_0 = ekf_rod_B.r_calibrated[arg_nodeId - 1] -
      anchor_near_idx_2;
    for (i = 0; i < 6; i++) {
      ekf_rod_B.x_pre[i] += ekf_rod_B.K[i] * ekf_rod_B.sig_idx_0;
    }

    // '<S3>:1:16' P_upd = (eye(6) - K*H)*P_pre;
    for (i = 0; i < 36; i++) {
      ekf_rod_B.I[i] = 0;
    }

    for (i = 0; i < 6; i++) {
      ekf_rod_B.I[i + 6 * i] = 1;
    }

    for (i = 0; i < 6; i++) {
      for (i_0 = 0; i_0 < 6; i_0++) {
        ekf_rod_B.A[i + 6 * i_0] = (real_T)ekf_rod_B.I[6 * i_0 + i] -
          ekf_rod_B.K[i] * ekf_rod_B.H[i_0];
      }
    }

    for (i = 0; i < 6; i++) {
      for (i_0 = 0; i_0 < 6; i_0++) {
        ekf_rod_B.P_upd[i + 6 * i_0] = 0.0;
        for (i_1 = 0; i_1 < 6; i_1++) {
          ekf_rod_B.P_upd[i + 6 * i_0] += ekf_rod_B.A[6 * i_1 + i] *
            ekf_rod_B.P_pre[6 * i_0 + i_1];
        }
      }
    }
  } else {
    // '<S3>:1:17' else
    // '<S3>:1:18' x_upd = x_upd_1;
    for (i = 0; i < 6; i++) {
      ekf_rod_B.x_pre[i] = ekf_rod_DW.UnitDelay1_DSTATE[i];
    }

    // '<S3>:1:19' P_upd = P_upd_1;
    memcpy(&ekf_rod_B.P_upd[0], &ekf_rod_DW.UnitDelay2_DSTATE[0], 36U * sizeof
           (real_T));
  }

  // End of MATLAB Function: '<S1>/EKF update'

  // Update for UnitDelay: '<S1>/Unit Delay1'
  for (i = 0; i < 6; i++) {
    ekf_rod_DW.UnitDelay1_DSTATE[i] = ekf_rod_B.x_pre[i];
  }

  // End of Update for UnitDelay: '<S1>/Unit Delay1'

  // Update for UnitDelay: '<S1>/Unit Delay2'
  memcpy(&ekf_rod_DW.UnitDelay2_DSTATE[0], &ekf_rod_B.P_upd[0], 36U * sizeof
         (real_T));
}

// Model initialize function
void ekfModelClass::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // Start for DataStoreMemory: '<S1>/R'
  ekf_rod_DW.R = ekf_rod_P.R_InitialValue;

  // Start for DataStoreMemory: '<S1>/acc_xy'
  ekf_rod_DW.acc_xy = ekf_rod_P.acc_xy_InitialValue;

  // Start for DataStoreMemory: '<S1>/acc_z'
  ekf_rod_DW.acc_z = ekf_rod_P.acc_z_InitialValue;

  // Start for DataStoreMemory: '<S1>/ancs'
  memcpy(&ekf_rod_DW.ancs[0], &ekf_rod_P.EKFOD_ancs[0], 12U * sizeof(real_T));

  {
    int32_T i;

    // InitializeConditions for UnitDelay: '<S1>/Unit Delay1'
    for (i = 0; i < 6; i++) {
      ekf_rod_DW.UnitDelay1_DSTATE[i] = ekf_rod_P.EKFOD_x_hat0[i];
    }

    // End of InitializeConditions for UnitDelay: '<S1>/Unit Delay1'

    // InitializeConditions for UnitDelay: '<S1>/Unit Delay2'
    memcpy(&ekf_rod_DW.UnitDelay2_DSTATE[0], &ekf_rod_P.P_0[0], 36U * sizeof
           (real_T));
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
  P_ekf_rod_T ekf_rod_P_temp = {
    //  Variable: P_0

    //  Referenced by: '<S1>/Unit Delay2'
    { 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },

    //  Mask Parameter: EKFOD_ancs

    //  Referenced by: '<S1>/ancs'
    { -3.442, -3.96, -0.312, 3.992, -3.512, -2.27, 2.448, 4.426, -0.288, -3.892,
      3.5, -2.258 },

    //  Mask Parameter: EKFOD_x_hat0

    //  Referenced by: '<S1>/Unit Delay1'
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
    0.2,                               // Expression: 0.2

    //  Referenced by: '<S1>/R'
    5.0,                               // Expression: 5

    //  Referenced by: '<S1>/acc_xy'
    2.0                                // Expression: 2
    //  Referenced by: '<S1>/acc_z'
  };                                   // Modifiable parameters

  // Initialize tunable parameters
  ekf_rod_P = ekf_rod_P_temp;
}

// Destructor
ekfModelClass::~ekfModelClass()
{
  // Currently there is no destructor body generated.
}

// Block parameters get method
const P_ekf_rod_T & ekfModelClass::getBlockParameters() const
{
  return ekf_rod_P;
}

// Block parameters set method
void ekfModelClass::setBlockParameters(const P_ekf_rod_T *pekf_rod_P)
{
  ekf_rod_P = *pekf_rod_P;
}

// Block signals get method
const B_ekf_rod_T & ekfModelClass::getBlockSignals() const
{
  return ekf_rod_B;
}

// Block signals set method
void ekfModelClass::setBlockSignals(const B_ekf_rod_T *pekf_rod_B)
{
  ekf_rod_B = *pekf_rod_B;
}

// Block states get method
const DW_ekf_rod_T & ekfModelClass::getDWork() const
{
  return ekf_rod_DW;
}

// Block states set method
void ekfModelClass::setDWork(const DW_ekf_rod_T *pekf_rod_DW)
{
  ekf_rod_DW = *pekf_rod_DW;
}

// Real-Time Model get method
RT_MODEL_ekf_rod_T * ekfModelClass::getRTM()
{
  return (&ekf_rod_M);
}

// Real-Time Model set method
void ekfModelClass::setRTM(const RT_MODEL_ekf_rod_T *pekf_rod_M)
{
  ekf_rod_M = *pekf_rod_M;
}

//
// File trailer for generated code.
//
// [EOF]
//
