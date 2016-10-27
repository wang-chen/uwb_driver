//
// File: ekf_iod.cpp
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

#include "ekf_iod.h"
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
void ekfIodClass::step(real_T arg_dists[4], uint8_T arg_nodeId, real_T
  arg_deltat, boolean_T arg_calibEnable, real_T arg_ancs[12], real_T arg_x_prev
  [6], real_T arg_P_prev[36], real_T arg_z, real_T arg_u, real_T arg_x_est[6],
  real_T arg_P_est[36], boolean_T *arg_outlierR, boolean_T *arg_outlierH)
{
  boolean_T outlierR;
  boolean_T outlierL;
  static const int8_T a[6] = { 0, 0, 1, 0, 0, 0 };

  static const int8_T b[6] = { 0, 0, 0, 0, 0, 1 };

  static const int8_T d[6] = { 0, 0, 0, 0, 1, 0 };

  static const int8_T e[6] = { 0, 0, 0, 1, 0, 0 };

  static const int8_T b_b[6] = { 0, 0, 1, 0, 0, 0 };

  int32_T i;
  int32_T i_0;
  int32_T i_1;
  real_T S_idx_3;
  real_T disp_pre_idx_0;
  real_T disp_pre_idx_1;
  real_T disp_pre_idx_2;

  // MATLAB Function: '<S1>/Range Calibration' incorporates:
  //   Inport: '<Root>/calibEnable'
  //   Inport: '<Root>/dists'
  //   Inport: '<Root>/nodeId'

  // MATLAB Function 'ekf_iod/Range Calibration': '<S2>:1'
  // '<S2>:1:3' distsCalibed = dists;
  ekf_iod_B.distsCalibed[0] = arg_dists[0];
  ekf_iod_B.distsCalibed[1] = arg_dists[1];
  ekf_iod_B.distsCalibed[2] = arg_dists[2];
  ekf_iod_B.distsCalibed[3] = arg_dists[3];

  // '<S2>:1:4' if calibEnable
  if (arg_calibEnable) {
    // '<S2>:1:5' if 0 < dists(nodeId) && dists(nodeId) < 1.5
    if ((0.0 < arg_dists[arg_nodeId - 1]) && (arg_dists[arg_nodeId - 1] < 1.5))
    {
      // '<S2>:1:6' distsCalibed(nodeId) = 1.0447 * dists(nodeId) - 0.1932;
      ekf_iod_B.distsCalibed[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
        1.0447 - 0.1932;
    } else {
      // '<S2>:1:7' else
      // '<S2>:1:8' if dists(nodeId) > 1.5 && dists(nodeId) <= 10
      if ((arg_dists[arg_nodeId - 1] > 1.5) && (arg_dists[arg_nodeId - 1] <=
           10.0)) {
        // '<S2>:1:9' distsCalibed(nodeId) = 1.0029 * dists(nodeId) - 0.0829;
        ekf_iod_B.distsCalibed[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
          1.0029 - 0.0829;
      } else {
        // '<S2>:1:10' else
        // '<S2>:1:11' distsCalibed(nodeId) = 0.9976 * dists(nodeId) - 0.0511;
        ekf_iod_B.distsCalibed[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
          0.9976 - 0.0511;
      }
    }
  }

  // End of MATLAB Function: '<S1>/Range Calibration'

  // MATLAB Function: '<S1>/ekf estimation' incorporates:
  //   Inport: '<Root>/P_prev'
  //   Inport: '<Root>/ancs'
  //   Inport: '<Root>/deltat'
  //   Inport: '<Root>/nodeId'
  //   Inport: '<Root>/u'
  //   Inport: '<Root>/x_prev'
  //   Inport: '<Root>/z'

  // MATLAB Function 'ekf_iod/ekf estimation': '<S3>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6
  //  x_est: 6 by 1, pos & vel; P_est: 6 by 6, covariance at last update;
  //  deltat: time difference between 2 effective rangings or estimations
  //  parameters acc_xy & acc_z for producing Q
  //  acceleration magnitude of x-y
  //  acceleration magnitude of z
  // '<S3>:1:14' F = [...
  // '<S3>:1:15'         1       0    	0   	deltat  0       0;...
  // '<S3>:1:16'         0       1   	0   	0       deltat  0;...
  // '<S3>:1:17'         0       0       1       0       0       deltat;...
  // '<S3>:1:18'         0       0       0       1       0       0;...
  // '<S3>:1:19'         0       0       0       0       1       0;...
  // '<S3>:1:20'         0       0       0       0       0       1];
  ekf_iod_B.F[0] = 1.0;
  ekf_iod_B.F[6] = 0.0;
  ekf_iod_B.F[12] = 0.0;
  ekf_iod_B.F[18] = arg_deltat;
  ekf_iod_B.F[24] = 0.0;
  ekf_iod_B.F[30] = 0.0;
  ekf_iod_B.F[1] = 0.0;
  ekf_iod_B.F[7] = 1.0;
  ekf_iod_B.F[13] = 0.0;
  ekf_iod_B.F[19] = 0.0;
  ekf_iod_B.F[25] = arg_deltat;
  ekf_iod_B.F[31] = 0.0;
  ekf_iod_B.F[2] = 0.0;
  ekf_iod_B.F[8] = 0.0;
  ekf_iod_B.F[14] = 1.0;
  ekf_iod_B.F[20] = 0.0;
  ekf_iod_B.F[26] = 0.0;
  ekf_iod_B.F[32] = arg_deltat;
  for (i = 0; i < 6; i++) {
    ekf_iod_B.F[3 + 6 * i] = e[i];
  }

  for (i = 0; i < 6; i++) {
    ekf_iod_B.F[4 + 6 * i] = d[i];
  }

  for (i = 0; i < 6; i++) {
    ekf_iod_B.F[5 + 6 * i] = b[i];
  }

  // '<S3>:1:22' B = [0; 0; 0.5*deltat^2; 0; 0; deltat];
  // '<S3>:1:24' Q = [deltat^4/4*acc_xy^2           0                       0               deltat^3/2*acc_xy^2             0                       0;...
  // '<S3>:1:25'         0                     deltat^4/4*acc_xy^2              0                       0               deltat^3/2*acc_xy^2             0;...
  // '<S3>:1:26'         0                              0               deltat^4/4*acc_z^2              0                       0               deltat^3/2*acc_z^2;...
  // '<S3>:1:27'         deltat^3/2*acc_xy^2            0                       0               deltat^2*acc_xy^2             0                       0;...
  // '<S3>:1:28'         0                     deltat^3/2*acc_xy^2              0                       0               deltat^2*acc_xy^2               0;...
  // '<S3>:1:29'         0                              0               deltat^3/2*acc_z^2              0                       0               deltat^2*acc_z^2];
  ekf_iod_B.P_est[0] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[6] = 0.0;
  ekf_iod_B.P_est[12] = 0.0;
  ekf_iod_B.P_est[18] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[24] = 0.0;
  ekf_iod_B.P_est[30] = 0.0;
  ekf_iod_B.P_est[1] = 0.0;
  ekf_iod_B.P_est[7] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[13] = 0.0;
  ekf_iod_B.P_est[19] = 0.0;
  ekf_iod_B.P_est[25] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[31] = 0.0;
  ekf_iod_B.P_est[2] = 0.0;
  ekf_iod_B.P_est[8] = 0.0;
  ekf_iod_B.P_est[14] = rt_powd_snf(arg_deltat, 4.0) / 4.0 * (ekf_iod_DW.acc_z *
    ekf_iod_DW.acc_z);
  ekf_iod_B.P_est[20] = 0.0;
  ekf_iod_B.P_est[26] = 0.0;
  ekf_iod_B.P_est[32] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_iod_DW.acc_z *
    ekf_iod_DW.acc_z);
  ekf_iod_B.P_est[3] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[9] = 0.0;
  ekf_iod_B.P_est[15] = 0.0;
  ekf_iod_B.P_est[21] = arg_deltat * arg_deltat * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[27] = 0.0;
  ekf_iod_B.P_est[33] = 0.0;
  ekf_iod_B.P_est[4] = 0.0;
  ekf_iod_B.P_est[10] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[16] = 0.0;
  ekf_iod_B.P_est[22] = 0.0;
  ekf_iod_B.P_est[28] = arg_deltat * arg_deltat * (ekf_iod_DW.acc_xy *
    ekf_iod_DW.acc_xy);
  ekf_iod_B.P_est[34] = 0.0;
  ekf_iod_B.P_est[5] = 0.0;
  ekf_iod_B.P_est[11] = 0.0;
  ekf_iod_B.P_est[17] = rt_powd_snf(arg_deltat, 3.0) / 2.0 * (ekf_iod_DW.acc_z *
    ekf_iod_DW.acc_z);
  ekf_iod_B.P_est[23] = 0.0;
  ekf_iod_B.P_est[29] = 0.0;
  ekf_iod_B.P_est[35] = arg_deltat * arg_deltat * (ekf_iod_DW.acc_z *
    ekf_iod_DW.acc_z);

  // '<S3>:1:31' x_pre = F*x_prev + B*u;
  ekf_iod_B.x_est[0] = 0.0;
  ekf_iod_B.x_est[1] = 0.0;
  ekf_iod_B.x_est[2] = arg_deltat * arg_deltat * 0.5;
  ekf_iod_B.x_est[3] = 0.0;
  ekf_iod_B.x_est[4] = 0.0;
  ekf_iod_B.x_est[5] = arg_deltat;
  for (i = 0; i < 6; i++) {
    S_idx_3 = 0.0;
    for (i_1 = 0; i_1 < 6; i_1++) {
      S_idx_3 += ekf_iod_B.F[6 * i_1 + i] * arg_x_prev[i_1];
    }

    ekf_iod_B.x_pre[i] = ekf_iod_B.x_est[i] * arg_u + S_idx_3;
  }

  // '<S3>:1:32' P_pre = F*P_prev*F' + Q;
  for (i = 0; i < 6; i++) {
    for (i_1 = 0; i_1 < 6; i_1++) {
      ekf_iod_B.F_m[i + 6 * i_1] = 0.0;
      for (i_0 = 0; i_0 < 6; i_0++) {
        ekf_iod_B.F_m[i + 6 * i_1] += ekf_iod_B.F[6 * i_0 + i] * arg_P_prev[6 *
          i_1 + i_0];
      }
    }
  }

  for (i = 0; i < 6; i++) {
    for (i_1 = 0; i_1 < 6; i_1++) {
      S_idx_3 = 0.0;
      for (i_0 = 0; i_0 < 6; i_0++) {
        S_idx_3 += ekf_iod_B.F_m[6 * i_0 + i] * ekf_iod_B.F[6 * i_0 + i_1];
      }

      ekf_iod_B.P_pre[i + 6 * i_1] = ekf_iod_B.P_est[6 * i_1 + i] + S_idx_3;
    }
  }

  //  predicted displacement
  // '<S3>:1:35' disp_pre = x_pre(1:3)' - ancs(:, nodeId)';
  disp_pre_idx_0 = ekf_iod_B.x_pre[0] - arg_ancs[(arg_nodeId - 1) * 3];
  disp_pre_idx_1 = ekf_iod_B.x_pre[1] - arg_ancs[(arg_nodeId - 1) * 3 + 1];
  disp_pre_idx_2 = ekf_iod_B.x_pre[2] - arg_ancs[(arg_nodeId - 1) * 3 + 2];

  //  predicted distance
  // '<S3>:1:37' dist_pre=norm(disp_pre, 2);
  ekf_iod_B.scale = 2.2250738585072014E-308;
  ekf_iod_B.innov2 = fabs(disp_pre_idx_0);
  if (ekf_iod_B.innov2 > 2.2250738585072014E-308) {
    ekf_iod_B.dist_pre = 1.0;
    ekf_iod_B.scale = ekf_iod_B.innov2;
  } else {
    ekf_iod_B.innov1 = ekf_iod_B.innov2 / 2.2250738585072014E-308;
    ekf_iod_B.dist_pre = ekf_iod_B.innov1 * ekf_iod_B.innov1;
  }

  ekf_iod_B.innov2 = fabs(disp_pre_idx_1);
  if (ekf_iod_B.innov2 > ekf_iod_B.scale) {
    ekf_iod_B.innov1 = ekf_iod_B.scale / ekf_iod_B.innov2;
    ekf_iod_B.dist_pre = ekf_iod_B.dist_pre * ekf_iod_B.innov1 *
      ekf_iod_B.innov1 + 1.0;
    ekf_iod_B.scale = ekf_iod_B.innov2;
  } else {
    ekf_iod_B.innov1 = ekf_iod_B.innov2 / ekf_iod_B.scale;
    ekf_iod_B.dist_pre += ekf_iod_B.innov1 * ekf_iod_B.innov1;
  }

  ekf_iod_B.innov2 = fabs(disp_pre_idx_2);
  if (ekf_iod_B.innov2 > ekf_iod_B.scale) {
    ekf_iod_B.innov1 = ekf_iod_B.scale / ekf_iod_B.innov2;
    ekf_iod_B.dist_pre = ekf_iod_B.dist_pre * ekf_iod_B.innov1 *
      ekf_iod_B.innov1 + 1.0;
    ekf_iod_B.scale = ekf_iod_B.innov2;
  } else {
    ekf_iod_B.innov1 = ekf_iod_B.innov2 / ekf_iod_B.scale;
    ekf_iod_B.dist_pre += ekf_iod_B.innov1 * ekf_iod_B.innov1;
  }

  ekf_iod_B.dist_pre = ekf_iod_B.scale * sqrt(ekf_iod_B.dist_pre);

  // '<S3>:1:39' outlierR = true;
  outlierR = true;

  // '<S3>:1:40' outlierL = true;
  outlierL = true;

  // outlier assessment for radio reading
  // '<S3>:1:43' H1 = [disp_pre/dist_pre 0 0 0];
  ekf_iod_B.H1[0] = disp_pre_idx_0 / ekf_iod_B.dist_pre;
  ekf_iod_B.H1[1] = disp_pre_idx_1 / ekf_iod_B.dist_pre;
  ekf_iod_B.H1[2] = disp_pre_idx_2 / ekf_iod_B.dist_pre;
  ekf_iod_B.H1[3] = 0.0;
  ekf_iod_B.H1[4] = 0.0;
  ekf_iod_B.H1[5] = 0.0;

  // '<S3>:1:44' innov1 = [dists(nodeId)-dist_pre];
  ekf_iod_B.innov1 = ekf_iod_B.distsCalibed[arg_nodeId - 1] - ekf_iod_B.dist_pre;

  // '<S3>:1:45' S1 = R + H1*P_pre*H1';
  S_idx_3 = 0.0;
  for (i = 0; i < 6; i++) {
    ekf_iod_B.x_est[i] = 0.0;
    for (i_1 = 0; i_1 < 6; i_1++) {
      ekf_iod_B.x_est[i] += ekf_iod_B.P_pre[6 * i + i_1] * ekf_iod_B.H1[i_1];
    }

    S_idx_3 += ekf_iod_B.x_est[i] * ekf_iod_B.H1[i];
  }

  ekf_iod_B.dist_pre = ekf_iod_DW.R + S_idx_3;

  // '<S3>:1:46' if (innov1^2/S1 < ADMIT_RATIO_R^2)
  if (ekf_iod_B.innov1 * ekf_iod_B.innov1 / ekf_iod_B.dist_pre <
      ekf_iod_DW.ADMIT_RATIO_R * ekf_iod_DW.ADMIT_RATIO_R) {
    // '<S3>:1:47' outlierR = false;
    outlierR = false;
  }

  // outlier assessment for laser reading
  // '<S3>:1:51' H2 = [0 0 1 0 0 0];
  // '<S3>:1:52' innov2 = [z-x_pre(3)];
  ekf_iod_B.innov2 = arg_z - ekf_iod_B.x_pre[2];

  // '<S3>:1:53' if z(1) < 0
  S_idx_3 = 0.0;
  for (i = 0; i < 6; i++) {
    ekf_iod_B.x_est[i] = 0.0;
    for (i_1 = 0; i_1 < 6; i_1++) {
      ekf_iod_B.x_est[i] += ekf_iod_B.P_pre[6 * i + i_1] * (real_T)a[i_1];
    }

    S_idx_3 += ekf_iod_B.x_est[i] * (real_T)b_b[i];
  }

  if ((arg_z < 0.0) && ((ekf_iod_B.innov2 * ekf_iod_B.innov2 / (ekf_iod_DW.R +
         S_idx_3) < ekf_iod_DW.ADMIT_RATIO_L * ekf_iod_DW.ADMIT_RATIO_L) ||
                        (fabs(ekf_iod_B.innov2) < 0.2))) {
    // '<S3>:1:54' S2 = R + H2*P_pre*H2';
    // '<S3>:1:55' if innov2^2/S2 < ADMIT_RATIO_L^2 || abs(innov2) < 0.2
    // '<S3>:1:56' outlierL = false;
    outlierL = false;
  }

  // '<S3>:1:60' if ~outlierR && ~outlierL
  if ((!outlierR) && (!outlierL)) {
    // '<S3>:1:61' H = [H1; H2];
    for (i = 0; i < 6; i++) {
      ekf_iod_B.H[i << 1] = ekf_iod_B.H1[i];
    }

    for (i = 0; i < 6; i++) {
      ekf_iod_B.H[1 + (i << 1)] = a[i];
    }

    // '<S3>:1:62' innov = [innov1; innov2];
    // '<S3>:1:63' S = [R 0; 0 R] + H*P_pre*H';
    for (i = 0; i < 2; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.H_c[i + (i_1 << 1)] = 0.0;
        for (i_0 = 0; i_0 < 6; i_0++) {
          ekf_iod_B.H_c[i + (i_1 << 1)] += ekf_iod_B.H[(i_0 << 1) + i] *
            ekf_iod_B.P_pre[6 * i_1 + i_0];
        }
      }
    }

    for (i = 0; i < 2; i++) {
      for (i_1 = 0; i_1 < 2; i_1++) {
        ekf_iod_B.distsCalibed[i + (i_1 << 1)] = 0.0;
        for (i_0 = 0; i_0 < 6; i_0++) {
          ekf_iod_B.distsCalibed[i + (i_1 << 1)] += ekf_iod_B.H_c[(i_0 << 1) + i]
            * ekf_iod_B.H[(i_0 << 1) + i_1];
        }
      }
    }

    disp_pre_idx_0 = ekf_iod_DW.R + ekf_iod_B.distsCalibed[0];
    S_idx_3 = ekf_iod_DW.R + ekf_iod_B.distsCalibed[3];

    // '<S3>:1:65' K = P_pre*H'*S^-1;
    if (fabs(ekf_iod_B.distsCalibed[1]) > fabs(disp_pre_idx_0)) {
      ekf_iod_B.dist_pre = disp_pre_idx_0 / ekf_iod_B.distsCalibed[1];
      ekf_iod_B.scale = 1.0 / (ekf_iod_B.dist_pre * S_idx_3 -
        ekf_iod_B.distsCalibed[2]);
      disp_pre_idx_1 = S_idx_3 / ekf_iod_B.distsCalibed[1] * ekf_iod_B.scale;
      disp_pre_idx_2 = -ekf_iod_B.scale;
      S_idx_3 = -ekf_iod_B.distsCalibed[2] / ekf_iod_B.distsCalibed[1] *
        ekf_iod_B.scale;
      ekf_iod_B.scale *= ekf_iod_B.dist_pre;
    } else {
      ekf_iod_B.dist_pre = ekf_iod_B.distsCalibed[1] / disp_pre_idx_0;
      ekf_iod_B.scale = 1.0 / (S_idx_3 - ekf_iod_B.dist_pre *
        ekf_iod_B.distsCalibed[2]);
      disp_pre_idx_1 = S_idx_3 / disp_pre_idx_0 * ekf_iod_B.scale;
      disp_pre_idx_2 = -ekf_iod_B.dist_pre * ekf_iod_B.scale;
      S_idx_3 = -ekf_iod_B.distsCalibed[2] / disp_pre_idx_0 * ekf_iod_B.scale;
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 2; i_1++) {
        ekf_iod_B.H_c[i + 6 * i_1] = 0.0;
        for (i_0 = 0; i_0 < 6; i_0++) {
          ekf_iod_B.H_c[i + 6 * i_1] += ekf_iod_B.P_pre[6 * i_0 + i] *
            ekf_iod_B.H[(i_0 << 1) + i_1];
        }
      }
    }

    for (i = 0; i < 6; i++) {
      ekf_iod_B.b_K[i] = 0.0;
      ekf_iod_B.b_K[i] += ekf_iod_B.H_c[i] * disp_pre_idx_1;
      ekf_iod_B.b_K[i] += ekf_iod_B.H_c[i + 6] * disp_pre_idx_2;
      ekf_iod_B.b_K[i + 6] = 0.0;
      ekf_iod_B.b_K[i + 6] += ekf_iod_B.H_c[i] * S_idx_3;
      ekf_iod_B.b_K[i + 6] += ekf_iod_B.H_c[i + 6] * ekf_iod_B.scale;
    }

    // '<S3>:1:66' x_est = x_pre + K*innov;
    for (i = 0; i < 6; i++) {
      ekf_iod_B.x_est[i] = (ekf_iod_B.b_K[i + 6] * ekf_iod_B.innov2 +
                            ekf_iod_B.b_K[i] * ekf_iod_B.innov1) +
        ekf_iod_B.x_pre[i];
    }

    // '<S3>:1:67' P_est = (eye(6) - K*H)*P_pre;
    memset(&ekf_iod_B.F[0], 0, 36U * sizeof(real_T));
    for (i = 0; i < 6; i++) {
      ekf_iod_B.F[i + 6 * i] = 1.0;
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.F_m[i + 6 * i_1] = ekf_iod_B.F[6 * i_1 + i] - (ekf_iod_B.H
          [(i_1 << 1) + 1] * ekf_iod_B.b_K[i + 6] + ekf_iod_B.H[i_1 << 1] *
          ekf_iod_B.b_K[i]);
      }
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.P_est[i + 6 * i_1] = 0.0;
        for (i_0 = 0; i_0 < 6; i_0++) {
          ekf_iod_B.P_est[i + 6 * i_1] += ekf_iod_B.F_m[6 * i_0 + i] *
            ekf_iod_B.P_pre[6 * i_1 + i_0];
        }
      }
    }

    // '<S3>:1:68' P_est = .5*(P_est + P_est');
    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.F[i_1 + 6 * i] = (ekf_iod_B.P_est[6 * i + i_1] +
          ekf_iod_B.P_est[6 * i_1 + i]) * 0.5;
      }
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.P_est[i_1 + 6 * i] = ekf_iod_B.F[6 * i + i_1];
      }
    }
  } else if (!outlierR) {
    // '<S3>:1:70' elseif ~outlierR
    // '<S3>:1:71' H = H1;
    // '<S3>:1:72' innov = innov1;
    // '<S3>:1:73' S = S1;
    // '<S3>:1:75' K = P_pre*H'*S^-1;
    ekf_iod_B.dist_pre = 1.0 / ekf_iod_B.dist_pre;

    // '<S3>:1:76' x_est = x_pre + K*innov;
    for (i = 0; i < 6; i++) {
      ekf_iod_B.P_pre_k[i] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.P_pre_k[i] += ekf_iod_B.P_pre[6 * i_1 + i] * ekf_iod_B.H1[i_1];
      }

      ekf_iod_B.K[i] = ekf_iod_B.P_pre_k[i] * ekf_iod_B.dist_pre;
      ekf_iod_B.x_est[i] = ekf_iod_B.K[i] * ekf_iod_B.innov1 + ekf_iod_B.x_pre[i];
    }

    // '<S3>:1:77' P_est = (eye(6) - K*H)*P_pre;
    memset(&ekf_iod_B.F[0], 0, 36U * sizeof(real_T));
    for (i = 0; i < 6; i++) {
      ekf_iod_B.F[i + 6 * i] = 1.0;
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.F_m[i + 6 * i_1] = ekf_iod_B.F[6 * i_1 + i] - ekf_iod_B.K[i] *
          ekf_iod_B.H1[i_1];
      }
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.P_est[i + 6 * i_1] = 0.0;
        for (i_0 = 0; i_0 < 6; i_0++) {
          ekf_iod_B.P_est[i + 6 * i_1] += ekf_iod_B.F_m[6 * i_0 + i] *
            ekf_iod_B.P_pre[6 * i_1 + i_0];
        }
      }
    }

    // '<S3>:1:78' P_est = .5*(P_est + P_est');
    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.F[i_1 + 6 * i] = (ekf_iod_B.P_est[6 * i + i_1] +
          ekf_iod_B.P_est[6 * i_1 + i]) * 0.5;
      }
    }

    for (i = 0; i < 6; i++) {
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_iod_B.P_est[i_1 + 6 * i] = ekf_iod_B.F[6 * i + i_1];
      }
    }
  } else {
    // '<S3>:1:80' else
    // '<S3>:1:81' vmax = zeros(3, 1);
    // '<S3>:1:82' v_prev = x_prev(4:6);
    // '<S3>:1:83' for i = 1:3
    // '<S3>:1:84' vmax(i) = max(abs(v_prev(i)-sqrt(P_prev(i,i))), abs(v_prev(i)+sqrt(P_prev(i,i))));
    S_idx_3 = fabs(arg_x_prev[3] - sqrt(arg_P_prev[0]));
    disp_pre_idx_0 = fabs(arg_x_prev[3] + sqrt(arg_P_prev[0]));
    if ((S_idx_3 >= disp_pre_idx_0) || rtIsNaN(disp_pre_idx_0)) {
      disp_pre_idx_0 = S_idx_3;
    }

    // '<S3>:1:84' vmax(i) = max(abs(v_prev(i)-sqrt(P_prev(i,i))), abs(v_prev(i)+sqrt(P_prev(i,i))));
    S_idx_3 = fabs(arg_x_prev[4] - sqrt(arg_P_prev[7]));
    disp_pre_idx_1 = fabs(arg_x_prev[4] + sqrt(arg_P_prev[7]));
    if ((S_idx_3 >= disp_pre_idx_1) || rtIsNaN(disp_pre_idx_1)) {
      disp_pre_idx_1 = S_idx_3;
    }

    // '<S3>:1:84' vmax(i) = max(abs(v_prev(i)-sqrt(P_prev(i,i))), abs(v_prev(i)+sqrt(P_prev(i,i))));
    S_idx_3 = fabs(arg_x_prev[5] - sqrt(arg_P_prev[14]));
    disp_pre_idx_2 = fabs(arg_x_prev[5] + sqrt(arg_P_prev[14]));
    if ((S_idx_3 >= disp_pre_idx_2) || rtIsNaN(disp_pre_idx_2)) {
      disp_pre_idx_2 = S_idx_3;
    }

    // '<S3>:1:87' Q = Q + diag([vmax.^2 * deltat^2; zeros(3,1)]);
    ekf_iod_B.dist_pre = arg_deltat * arg_deltat;
    ekf_iod_B.x_pre[0] = disp_pre_idx_0 * disp_pre_idx_0 * ekf_iod_B.dist_pre;
    ekf_iod_B.x_pre[1] = disp_pre_idx_1 * disp_pre_idx_1 * ekf_iod_B.dist_pre;
    ekf_iod_B.x_pre[2] = disp_pre_idx_2 * disp_pre_idx_2 * ekf_iod_B.dist_pre;
    ekf_iod_B.x_pre[3] = 0.0;
    ekf_iod_B.x_pre[4] = 0.0;
    ekf_iod_B.x_pre[5] = 0.0;
    memset(&ekf_iod_B.F[0], 0, 36U * sizeof(real_T));
    for (i = 0; i < 6; i++) {
      ekf_iod_B.F[i + 6 * i] = ekf_iod_B.x_pre[i];
    }

    for (i = 0; i < 36; i++) {
      ekf_iod_B.P_est[i] += ekf_iod_B.F[i];
    }

    // Current prediction is the same but covariance is increased to account
    // for uncertainty of position after a longer time
    // '<S3>:1:91' x_est = x_prev;
    for (i = 0; i < 6; i++) {
      ekf_iod_B.x_est[i] = arg_x_prev[i];
    }

    // '<S3>:1:92' P_est = P_prev + Q;
    for (i = 0; i < 36; i++) {
      ekf_iod_B.P_est[i] += arg_P_prev[i];
    }
  }

  // Outport: '<Root>/x_est'
  for (i = 0; i < 6; i++) {
    arg_x_est[i] = ekf_iod_B.x_est[i];
  }

  // End of Outport: '<Root>/x_est'

  // Outport: '<Root>/outlierR' incorporates:
  //   MATLAB Function: '<S1>/ekf estimation'
  *arg_outlierR = outlierR;

  // Outport: '<Root>/P_est'
  memcpy(&arg_P_est[0], &ekf_iod_B.P_est[0], 36U * sizeof(real_T));

  // Outport: '<Root>/outlierH' incorporates:
  //   MATLAB Function: '<S1>/ekf estimation'
  *arg_outlierH = outlierL;
}

// Model initialize function
void ekfIodClass::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // Start for DataStoreMemory: '<S1>/ADMIT_RATIO_L'
  ekf_iod_DW.ADMIT_RATIO_L = ekf_iod_P.ADMIT_RATIO_L_InitialValue;

  // Start for DataStoreMemory: '<S1>/ADMIT_RATIO_R'
  ekf_iod_DW.ADMIT_RATIO_R = ekf_iod_P.ADMIT_RATIO_R_InitialValue;

  // Start for DataStoreMemory: '<S1>/R'
  ekf_iod_DW.R = ekf_iod_P.R_InitialValue;

  // Start for DataStoreMemory: '<S1>/acc_xy'
  ekf_iod_DW.acc_xy = ekf_iod_P.acc_xy_InitialValue;

  // Start for DataStoreMemory: '<S1>/acc_z'
  ekf_iod_DW.acc_z = ekf_iod_P.acc_z_InitialValue;
}

// Model terminate function
void ekfIodClass::terminate()
{
  // (no terminate code required)
}

// Constructor
ekfIodClass::ekfIodClass()
{
  P_ekf_iod_T ekf_iod_P_temp = {
    0.1,                               // Expression: 0.1

    //  Referenced by: '<S1>/ADMIT_RATIO_L'
    1.5,                               // Expression: 1.5

    //  Referenced by: '<S1>/ADMIT_RATIO_R'
    0.2,                               // Expression: 0.2

    //  Referenced by: '<S1>/R'
    5.0,                               // Expression: 5

    //  Referenced by: '<S1>/acc_xy'
    2.0                                // Expression: 2
    //  Referenced by: '<S1>/acc_z'
  };                                   // Modifiable parameters

  // Initialize tunable parameters
  ekf_iod_P = ekf_iod_P_temp;
}

// Destructor
ekfIodClass::~ekfIodClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL_ekf_iod_T * ekfIodClass::getRTM()
{
  return (&ekf_iod_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
