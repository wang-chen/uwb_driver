//
// File: ekf_mf.cpp
//
// Code generated for Simulink model 'ekf_mf'.
//
// Model version                  : 1.328
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Thu Oct 22 18:28:08 2015
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objectives:
//    1. Execution efficiency
//    2. Traceability
//    3. Safety precaution
// Validation result: Not run
//
#include "ekf_mf.h"

extern real_T rt_roundd(real_T u);

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

// Function for MATLAB Function: '<S1>/Median Filter'
void ekf_mfClass::ekf_mf_eml_sort(const real_T x_data[], const int32_T x_sizes[2],
  real_T y_data[], int32_T y_sizes[2])
{
  int32_T ix;
  int32_T c_k;
  int32_T i;
  int32_T i2;
  int32_T j;
  int32_T pEnd;
  int32_T q;
  int32_T qEnd;
  int32_T kEnd;
  int8_T b_idx_0;
  b_idx_0 = (int8_T)x_sizes[1];
  i2 = (int8_T)x_sizes[1];
  y_sizes[0] = x_sizes[0];
  y_sizes[1] = x_sizes[1];
  ix = 0;
  for (pEnd = 0; pEnd < x_sizes[1]; pEnd++) {
    ekf_mf_B.vwork_data[pEnd] = x_data[ix];
    ix++;
  }

  if (b_idx_0 != 0) {
    for (pEnd = 1; pEnd <= b_idx_0; pEnd++) {
      ekf_mf_B.iidx_data[pEnd - 1] = pEnd;
    }

    for (pEnd = 1; pEnd <= b_idx_0 - 1; pEnd += 2) {
      if (!(ekf_mf_B.vwork_data[pEnd - 1] <= ekf_mf_B.vwork_data[pEnd])) {
        ekf_mf_B.iidx_data[pEnd - 1] = pEnd + 1;
        ekf_mf_B.iidx_data[pEnd] = pEnd;
      }
    }

    for (i = 0; i < i2; i++) {
      ekf_mf_B.idx0_data[i] = 1;
    }

    i = 2;
    while (i < b_idx_0) {
      i2 = i << 1;
      j = 1;
      pEnd = 1 + i;
      while (pEnd < b_idx_0 + 1) {
        ix = j;
        q = pEnd;
        qEnd = j + i2;
        if (qEnd > b_idx_0 + 1) {
          qEnd = b_idx_0 + 1;
        }

        c_k = 0;
        kEnd = qEnd - j;
        while (c_k + 1 <= kEnd) {
          if (ekf_mf_B.vwork_data[ekf_mf_B.iidx_data[ix - 1] - 1] <=
              ekf_mf_B.vwork_data[ekf_mf_B.iidx_data[q - 1] - 1]) {
            ekf_mf_B.idx0_data[c_k] = ekf_mf_B.iidx_data[ix - 1];
            ix++;
            if (ix == pEnd) {
              while (q < qEnd) {
                c_k++;
                ekf_mf_B.idx0_data[c_k] = ekf_mf_B.iidx_data[q - 1];
                q++;
              }
            }
          } else {
            ekf_mf_B.idx0_data[c_k] = ekf_mf_B.iidx_data[q - 1];
            q++;
            if (q == qEnd) {
              while (ix < pEnd) {
                c_k++;
                ekf_mf_B.idx0_data[c_k] = ekf_mf_B.iidx_data[ix - 1];
                ix++;
              }
            }
          }

          c_k++;
        }

        for (pEnd = 0; pEnd + 1 <= kEnd; pEnd++) {
          ekf_mf_B.iidx_data[(j + pEnd) - 1] = ekf_mf_B.idx0_data[pEnd];
        }

        j = qEnd;
        pEnd = qEnd + i;
      }

      i = i2;
    }
  }

  ix = 0;
  for (pEnd = 0; pEnd < x_sizes[1]; pEnd++) {
    y_data[ix] = ekf_mf_B.vwork_data[ekf_mf_B.iidx_data[pEnd] - 1];
    ix++;
  }
}

real_T rt_roundd(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

// Model step function
void ekf_mfClass::step(real_T arg_dists[4], real_T arg_deltat, real_T arg_imu,
  uint8_T arg_nodeId, real_T arg_calibEnable, real_T arg_medBuffSize, real_T
  arg_x_est[6], real_T arg_distMf[4])
{
  static const int8_T b[6] = { 0, 0, 0, 0, 0, 1 };

  static const int8_T c[6] = { 0, 0, 0, 0, 1, 0 };

  static const int8_T d[6] = { 0, 0, 0, 1, 0, 0 };

  int32_T i;
  int32_T i_0;
  int32_T i_1;
  real_T dis_pre_vect_idx_2;
  uint8_T tmp;
  UNUSED_PARAMETER(arg_imu);

  // MATLAB Function: '<S1>/EKF prediction' incorporates:
  //   Inport: '<Root>/deltat'
  //   UnitDelay: '<S1>/Unit Delay1'
  //   UnitDelay: '<S1>/Unit Delay2'

  // MATLAB Function 'ekf_mf/EKF prediction': '<S2>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6
  //  x_est: 6 by 1, pos & vel; P_est: 6 by 6, covariance at last update;
  //  deltat: time difference between 2 effective rangings or estimations
  //  parameters acc_xy & acc_z for producing Q
  //  acceleration magnitude of x-y
  //  acceleration magnitude of z
  // '<S2>:1:12' A = [...
  // '<S2>:1:13'     1       0    	0   	deltat  0       0;...
  // '<S2>:1:14'     0       1   	0   	0       deltat  0;...
  // '<S2>:1:15'     0       0       1       0       0       deltat;...
  // '<S2>:1:16'     0       0       0       1       0       0;...
  // '<S2>:1:17'     0       0       0       0       1       0;...
  // '<S2>:1:18'     0       0       0       0       0       1];
  ekf_mf_B.A[0] = 1.0;
  ekf_mf_B.A[6] = 0.0;
  ekf_mf_B.A[12] = 0.0;
  ekf_mf_B.A[18] = arg_deltat;
  ekf_mf_B.A[24] = 0.0;
  ekf_mf_B.A[30] = 0.0;
  ekf_mf_B.A[1] = 0.0;
  ekf_mf_B.A[7] = 1.0;
  ekf_mf_B.A[13] = 0.0;
  ekf_mf_B.A[19] = 0.0;
  ekf_mf_B.A[25] = arg_deltat;
  ekf_mf_B.A[31] = 0.0;
  ekf_mf_B.A[2] = 0.0;
  ekf_mf_B.A[8] = 0.0;
  ekf_mf_B.A[14] = 1.0;
  ekf_mf_B.A[20] = 0.0;
  ekf_mf_B.A[26] = 0.0;
  ekf_mf_B.A[32] = arg_deltat;
  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.A[3 + 6 * i_0] = d[i_0];
  }

  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.A[4 + 6 * i_0] = c[i_0];
  }

  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.A[5 + 6 * i_0] = b[i_0];
  }

  // '<S2>:1:20' Q = [deltat^4/4*acc_xy^2           0                       0               deltat^3/2*acc_xy^2             0                       0;... 
  // '<S2>:1:21'     0                     deltat^4/4*acc_xy^2              0                       0               deltat^3/2*acc_xy^2             0;... 
  // '<S2>:1:22'     0                              0               deltat^4/4*acc_z^2              0                       0               deltat^3/2*acc_z^2;... 
  // '<S2>:1:23'     deltat^3/2*acc_xy^2            0                       0               deltat^2*acc_xy^2             0                       0;... 
  // '<S2>:1:24'     0                     deltat^3/2*acc_xy^2              0                       0               deltat^2*acc_xy^2               0;... 
  // '<S2>:1:25'     0                              0               deltat^3/2*acc_z^2              0                       0               deltat^2*acc_z^2]; 
  // '<S2>:1:27' x_pre = A*x_est;
  // '<S2>:1:28' P_pre = A*P_est*A' + Q;
  for (i_0 = 0; i_0 < 6; i_0++) {
    for (i = 0; i < 6; i++) {
      ekf_mf_B.A_m[i_0 + 6 * i] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_mf_B.A_m[i_0 + 6 * i] += ekf_mf_B.A[6 * i_1 + i_0] *
          ekf_mf_DW.UnitDelay2_DSTATE[6 * i + i_1];
      }
    }
  }

  for (i_0 = 0; i_0 < 6; i_0++) {
    for (i = 0; i < 6; i++) {
      ekf_mf_B.A_c[i_0 + 6 * i] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_mf_B.A_c[i_0 + 6 * i] += ekf_mf_B.A_m[6 * i_1 + i_0] * ekf_mf_B.A[6 *
          i_1 + i];
      }
    }
  }

  ekf_mf_B.dv0[0] = pow(arg_deltat, 4.0) / 4.0 * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[6] = 0.0;
  ekf_mf_B.dv0[12] = 0.0;
  ekf_mf_B.dv0[18] = pow(arg_deltat, 3.0) / 2.0 * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[24] = 0.0;
  ekf_mf_B.dv0[30] = 0.0;
  ekf_mf_B.dv0[1] = 0.0;
  ekf_mf_B.dv0[7] = pow(arg_deltat, 4.0) / 4.0 * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[13] = 0.0;
  ekf_mf_B.dv0[19] = 0.0;
  ekf_mf_B.dv0[25] = pow(arg_deltat, 3.0) / 2.0 * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[31] = 0.0;
  ekf_mf_B.dv0[2] = 0.0;
  ekf_mf_B.dv0[8] = 0.0;
  ekf_mf_B.dv0[14] = pow(arg_deltat, 4.0) / 4.0 * (ekf_mf_DW.acc_z *
    ekf_mf_DW.acc_z);
  ekf_mf_B.dv0[20] = 0.0;
  ekf_mf_B.dv0[26] = 0.0;
  ekf_mf_B.dv0[32] = pow(arg_deltat, 3.0) / 2.0 * (ekf_mf_DW.acc_z *
    ekf_mf_DW.acc_z);
  ekf_mf_B.dv0[3] = pow(arg_deltat, 3.0) / 2.0 * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[9] = 0.0;
  ekf_mf_B.dv0[15] = 0.0;
  ekf_mf_B.dv0[21] = arg_deltat * arg_deltat * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[27] = 0.0;
  ekf_mf_B.dv0[33] = 0.0;
  ekf_mf_B.dv0[4] = 0.0;
  ekf_mf_B.dv0[10] = pow(arg_deltat, 3.0) / 2.0 * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[16] = 0.0;
  ekf_mf_B.dv0[22] = 0.0;
  ekf_mf_B.dv0[28] = arg_deltat * arg_deltat * (ekf_mf_DW.acc_xy *
    ekf_mf_DW.acc_xy);
  ekf_mf_B.dv0[34] = 0.0;
  ekf_mf_B.dv0[5] = 0.0;
  ekf_mf_B.dv0[11] = 0.0;
  ekf_mf_B.dv0[17] = pow(arg_deltat, 3.0) / 2.0 * (ekf_mf_DW.acc_z *
    ekf_mf_DW.acc_z);
  ekf_mf_B.dv0[23] = 0.0;
  ekf_mf_B.dv0[29] = 0.0;
  ekf_mf_B.dv0[35] = arg_deltat * arg_deltat * (ekf_mf_DW.acc_z *
    ekf_mf_DW.acc_z);
  for (i_0 = 0; i_0 < 6; i_0++) {
    for (i = 0; i < 6; i++) {
      ekf_mf_B.P_pre[i + 6 * i_0] = ekf_mf_B.A_c[6 * i_0 + i] + ekf_mf_B.dv0[6 *
        i_0 + i];
    }
  }

  // Attempt to make the covariance matrix symetric in hope that it would stay
  // positive definite.
  // P_pre = (P_pre + P_pre')*0.5;
  //  for i = 1:6
  //      if P_pre(i, i) <= 0
  //          P_pre = P_pre_1;
  //          break;
  //      end
  //  end
  //  P_pre_1 = P_pre;
  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.x_pre[i_0] = 0.0;
    for (i = 0; i < 6; i++) {
      ekf_mf_B.x_pre[i_0] += ekf_mf_B.A[6 * i + i_0] *
        ekf_mf_DW.UnitDelay1_DSTATE[i];
    }
  }

  // End of MATLAB Function: '<S1>/EKF prediction'

  // MATLAB Function: '<S1>/Median Filter' incorporates:
  //   Inport: '<Root>/calibEnable'
  //   Inport: '<Root>/dists'
  //   Inport: '<Root>/medBuffSize'
  //   Inport: '<Root>/nodeId'

  // MATLAB Function 'ekf_mf/Median Filter': '<S4>:1'
  // '<S4>:1:3' distsCalibed = dists;
  ekf_mf_B.distsCalibed[0] = arg_dists[0];
  ekf_mf_B.distsCalibed[1] = arg_dists[1];
  ekf_mf_B.distsCalibed[2] = arg_dists[2];
  ekf_mf_B.distsCalibed[3] = arg_dists[3];

  // '<S4>:1:4' if calibEnable == 1
  if (arg_calibEnable == 1.0) {
    // '<S4>:1:5' if 0 < dists(nodeId) && dists(nodeId) < 1.5
    if ((0.0 < arg_dists[arg_nodeId - 1]) && (arg_dists[arg_nodeId - 1] < 1.5))
    {
      // '<S4>:1:6' distsCalibed(nodeId) = 1.0447 * dists(nodeId) - 0.1932;
      ekf_mf_B.distsCalibed[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] * 1.0447
        - 0.1932;
    } else {
      // '<S4>:1:7' else
      // '<S4>:1:8' if dists(nodeId) > 1.5 && dists(nodeId) <= 10
      if ((arg_dists[arg_nodeId - 1] > 1.5) && (arg_dists[arg_nodeId - 1] <=
           10.0)) {
        // '<S4>:1:9' distsCalibed(nodeId) = 1.0029 * dists(nodeId) - 0.0829;
        ekf_mf_B.distsCalibed[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
          1.0029 - 0.0829;
      } else {
        // '<S4>:1:10' else
        // '<S4>:1:11' distsCalibed(nodeId) = 0.9976 * dists(nodeId) - 0.0511;
        ekf_mf_B.distsCalibed[arg_nodeId - 1] = arg_dists[arg_nodeId - 1] *
          0.9976 - 0.0511;
      }
    }
  }

  // '<S4>:1:17' initDists(nodeId, 2:9) = initDists(nodeId, 1:8);
  for (i_0 = 0; i_0 < 8; i_0++) {
    ekf_mf_B.dv1[i_0] = ekf_mf_DW.initDists[((i_0 << 2) + arg_nodeId) - 1];
  }

  for (i_0 = 0; i_0 < 8; i_0++) {
    ekf_mf_DW.initDists[(arg_nodeId + ((1 + i_0) << 2)) - 1] = ekf_mf_B.dv1[i_0];
  }

  // '<S4>:1:18' initDists(nodeId, 1) = distsCalibed(nodeId);
  ekf_mf_DW.initDists[arg_nodeId - 1] = ekf_mf_B.distsCalibed[arg_nodeId - 1];

  // '<S4>:1:19' buff = initDists(nodeId, 1:medBuffSize);
  // '<S4>:1:20' orderedBuff = sort(buff);
  if (1.0 > arg_medBuffSize) {
    i = 0;
  } else {
    i = (int32_T)arg_medBuffSize;
  }

  ekf_mf_B.tmp_sizes[0] = 1;
  ekf_mf_B.tmp_sizes[1] = i;
  for (i_0 = 0; i_0 < i; i_0++) {
    ekf_mf_B.tmp_data[i_0] = ekf_mf_DW.initDists[((i_0 << 2) + arg_nodeId) - 1];
  }

  ekf_mf_eml_sort(ekf_mf_B.tmp_data, ekf_mf_B.tmp_sizes,
                  ekf_mf_B.orderedBuff_data, ekf_mf_B.orderedBuff_sizes);

  // '<S4>:1:21' distsCalibed(nodeId) = orderedBuff(uint8(medBuffSize/2));
  ekf_mf_B.dis_pre_vect_idx_0 = rt_roundd(arg_medBuffSize / 2.0);
  if (ekf_mf_B.dis_pre_vect_idx_0 < 256.0) {
    if (ekf_mf_B.dis_pre_vect_idx_0 >= 0.0) {
      tmp = (uint8_T)ekf_mf_B.dis_pre_vect_idx_0;
    } else {
      tmp = 0U;
    }
  } else {
    tmp = MAX_uint8_T;
  }

  ekf_mf_B.distsCalibed[arg_nodeId - 1] = ekf_mf_B.orderedBuff_data[tmp - 1];

  // End of MATLAB Function: '<S1>/Median Filter'

  // MATLAB Function: '<S1>/EKF update' incorporates:
  //   Inport: '<Root>/nodeId'

  // MATLAB Function 'ekf_mf/EKF update': '<S3>:1'
  //  x_pre: 6 by 1, [pos vel]; P_pre: 6 by 6, predicted x and covariance
  //  x_upd: 6 by 1, pos & vel; P_upd: 6 by 6, updated x and covariance;
  // '<S3>:1:9' dis_pre_vect = ancs(:,nodeId) - x_pre(1:3);
  ekf_mf_B.dis_pre_vect_idx_0 = ekf_mf_DW.ancs[(arg_nodeId - 1) * 3] -
    ekf_mf_B.x_pre[0];
  ekf_mf_B.dis_pre_vect_idx_1 = ekf_mf_DW.ancs[(arg_nodeId - 1) * 3 + 1] -
    ekf_mf_B.x_pre[1];
  dis_pre_vect_idx_2 = ekf_mf_DW.ancs[(arg_nodeId - 1) * 3 + 2] -
    ekf_mf_B.x_pre[2];

  // '<S3>:1:10' dist_pre = sqrt(dis_pre_vect'*dis_pre_vect);
  dis_pre_vect_idx_2 = sqrt((ekf_mf_B.dis_pre_vect_idx_0 *
    ekf_mf_B.dis_pre_vect_idx_0 + ekf_mf_B.dis_pre_vect_idx_1 *
    ekf_mf_B.dis_pre_vect_idx_1) + dis_pre_vect_idx_2 * dis_pre_vect_idx_2);

  // '<S3>:1:11' H = zeros(1,6);
  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.H[i_0] = 0.0;
  }

  // H is
  // '<S3>:1:12' H(1:3) = (x_pre(1:3) - ancs(:,nodeId))/dist_pre;
  ekf_mf_B.H[0] = (ekf_mf_B.x_pre[0] - ekf_mf_DW.ancs[(arg_nodeId - 1) * 3]) /
    dis_pre_vect_idx_2;
  ekf_mf_B.H[1] = (ekf_mf_B.x_pre[1] - ekf_mf_DW.ancs[(arg_nodeId - 1) * 3 + 1])
    / dis_pre_vect_idx_2;
  ekf_mf_B.H[2] = (ekf_mf_B.x_pre[2] - ekf_mf_DW.ancs[(arg_nodeId - 1) * 3 + 2])
    / dis_pre_vect_idx_2;

  // '<S3>:1:13' K = P_pre*H'*(H*P_pre*H'+ R)^-1;
  ekf_mf_B.dis_pre_vect_idx_0 = 0.0;
  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.K[i_0] = 0.0;
    for (i = 0; i < 6; i++) {
      ekf_mf_B.K[i_0] += ekf_mf_B.P_pre[6 * i_0 + i] * ekf_mf_B.H[i];
    }

    ekf_mf_B.dis_pre_vect_idx_0 += ekf_mf_B.K[i_0] * ekf_mf_B.H[i_0];
    ekf_mf_B.rtb_P_pre_k[i_0] = 0.0;
    for (i = 0; i < 6; i++) {
      ekf_mf_B.rtb_P_pre_k[i_0] += ekf_mf_B.P_pre[6 * i + i_0] * ekf_mf_B.H[i];
    }
  }

  ekf_mf_B.dis_pre_vect_idx_0 = 1.0 / (ekf_mf_B.dis_pre_vect_idx_0 + ekf_mf_DW.R);

  // '<S3>:1:15' x_upd = x_pre + K*(dists(nodeId) - dist_pre);
  dis_pre_vect_idx_2 = ekf_mf_B.distsCalibed[arg_nodeId - 1] -
    dis_pre_vect_idx_2;
  for (i_0 = 0; i_0 < 6; i_0++) {
    ekf_mf_B.dis_pre_vect_idx_1 = ekf_mf_B.rtb_P_pre_k[i_0] *
      ekf_mf_B.dis_pre_vect_idx_0;
    ekf_mf_B.K[i_0] = ekf_mf_B.dis_pre_vect_idx_1;
    ekf_mf_B.x_pre[i_0] += ekf_mf_B.dis_pre_vect_idx_1 * dis_pre_vect_idx_2;
  }

  // '<S3>:1:16' P_upd = (eye(6) - K*H)*P_pre;
  for (i_0 = 0; i_0 < 36; i_0++) {
    ekf_mf_B.I[i_0] = 0;
  }

  for (i = 0; i < 6; i++) {
    ekf_mf_B.I[i + 6 * i] = 1;
  }

  // Outport: '<Root>/distMf'
  // Attempt to make the covariance matrix symetric in hope that it would stay
  // positive definite
  // P_upd = (P_upd + P_upd')*0.5;
  arg_distMf[0] = ekf_mf_B.distsCalibed[0];
  arg_distMf[1] = ekf_mf_B.distsCalibed[1];
  arg_distMf[2] = ekf_mf_B.distsCalibed[2];
  arg_distMf[3] = ekf_mf_B.distsCalibed[3];
  for (i = 0; i < 6; i++) {
    // Outport: '<Root>/x_est'
    arg_x_est[i] = ekf_mf_B.x_pre[i];

    // Update for UnitDelay: '<S1>/Unit Delay1'
    ekf_mf_DW.UnitDelay1_DSTATE[i] = ekf_mf_B.x_pre[i];
  }

  // MATLAB Function: '<S1>/EKF update'
  for (i_0 = 0; i_0 < 6; i_0++) {
    for (i = 0; i < 6; i++) {
      ekf_mf_B.A[i_0 + 6 * i] = (real_T)ekf_mf_B.I[6 * i + i_0] - ekf_mf_B.K[i_0]
        * ekf_mf_B.H[i];
    }
  }

  // Update for UnitDelay: '<S1>/Unit Delay2' incorporates:
  //   MATLAB Function: '<S1>/EKF update'

  for (i_0 = 0; i_0 < 6; i_0++) {
    for (i = 0; i < 6; i++) {
      ekf_mf_DW.UnitDelay2_DSTATE[i_0 + 6 * i] = 0.0;
      for (i_1 = 0; i_1 < 6; i_1++) {
        ekf_mf_DW.UnitDelay2_DSTATE[i_0 + 6 * i] += ekf_mf_B.A[6 * i_1 + i_0] *
          ekf_mf_B.P_pre[6 * i + i_1];
      }
    }
  }

  // End of Update for UnitDelay: '<S1>/Unit Delay2'
}

// Model initialize function
void ekf_mfClass::initialize()
{
  // Registration code

  // initialize error status
  rtmSetErrorStatus((&ekf_mf_M), (NULL));

  // states (dwork)
  (void) memset((void *)&ekf_mf_DW, 0,
                sizeof(DW_ekf_mf_T));

  // Start for DataStoreMemory: '<S1>/P_pre_1'
  memcpy(&ekf_mf_DW.P_pre_1[0], &ekf_mf_P.P_0[0], 36U * sizeof(real_T));

  // Start for DataStoreMemory: '<S1>/R'
  ekf_mf_DW.R = ekf_mf_P.R_InitialValue;

  // Start for DataStoreMemory: '<S1>/acc_xy'
  ekf_mf_DW.acc_xy = ekf_mf_P.acc_xy_InitialValue;

  // Start for DataStoreMemory: '<S1>/acc_z'
  ekf_mf_DW.acc_z = ekf_mf_P.acc_z_InitialValue;

  // Start for DataStoreMemory: '<S1>/ancs'
  memcpy(&ekf_mf_DW.ancs[0], &ekf_mf_P.ancs[0], 12U * sizeof(real_T));

  // Start for DataStoreMemory: '<S1>/initDists'
  memcpy(&ekf_mf_DW.initDists[0], &ekf_mf_P.initDists_InitialValue[0], 36U *
         sizeof(real_T));

  {
    int32_T i;

    // InitializeConditions for UnitDelay: '<S1>/Unit Delay1'
    for (i = 0; i < 6; i++) {
      ekf_mf_DW.UnitDelay1_DSTATE[i] = ekf_mf_P.ekf_mf_x_hat0[i];
    }

    // End of InitializeConditions for UnitDelay: '<S1>/Unit Delay1'

    // InitializeConditions for UnitDelay: '<S1>/Unit Delay2'
    memcpy(&ekf_mf_DW.UnitDelay2_DSTATE[0], &ekf_mf_P.P_0[0], 36U * sizeof
           (real_T));
  }
}

// Model terminate function
void ekf_mfClass::terminate()
{
  // (no terminate code required)
}

// Constructor
ekf_mfClass::ekf_mfClass()
{
  P_ekf_mf_T ekf_mf_P_temp = {
    //  Variable: P_0
    //  Referenced by:
    //    '<S1>/P_pre_1'
    //    '<S1>/Unit Delay2'

    { 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },

    //  Variable: ancs
    //  Referenced by: '<S1>/ancs'

    { -3.0, -3.0, -1.78, 3.0, -3.0, -0.1, 3.0, 3.0, -1.31, -3.0, 3.0, -0.21 },

    //  Mask Parameter: ekf_mf_x_hat0
    //  Referenced by: '<S1>/Unit Delay1'

    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
    0.2,                               // Expression: 0.2
                                       //  Referenced by: '<S1>/R'

    5.0,                               // Expression: 5
                                       //  Referenced by: '<S1>/acc_xy'

    2.0,                               // Expression: 2
                                       //  Referenced by: '<S1>/acc_z'


    //  Expression: [dists0 dists0 dists0 dists0 dists0 dists0 dists0 dists0 dists0]
    //  Referenced by: '<S1>/initDists'

    { 4.25779, 4.51474, 4.31816, 4.33893, 4.25779, 4.51474, 4.31816, 4.33893,
      4.25779, 4.51474, 4.31816, 4.33893, 4.25779, 4.51474, 4.31816, 4.33893,
      4.25779, 4.51474, 4.31816, 4.33893, 4.25779, 4.51474, 4.31816, 4.33893,
      4.25779, 4.51474, 4.31816, 4.33893, 4.25779, 4.51474, 4.31816, 4.33893,
      4.25779, 4.51474, 4.31816, 4.33893 }
  };                                   // Modifiable parameters

  // Initialize tunable parameters
  ekf_mf_P = ekf_mf_P_temp;
}

// Destructor
ekf_mfClass::~ekf_mfClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL_ekf_mf_T * ekf_mfClass::getRTM()
{
  return (&ekf_mf_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
