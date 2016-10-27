//
// File: nlsTRI345.c
//
// MATLAB Coder version            : 2.6
// C/C++ source code generated on  : 03-Aug-2016 19:31:22
//

// Include files
#include "nlsTRI345.h"

// Function Declarations
static real_T b_norm(const real_T x[3]);
static void diag(const real_T v_data[], const int32_T v_size[1], real_T d_data[],
                 int32_T d_size[2]);
static void eml_xgemm(int32_T n, const real_T A[9], const real_T B_data[],
                      real_T C_data[]);
static void iniEstimate(uint8_T numAvailble, const real_T anchorPos_data[],
  const int32_T anchorPos_size[2], const real_T Range_data[], real_T *x_TRI,
  real_T *y_TRI, real_T *z_TRI);
static void mpower(const real_T a[9], real_T c[9]);
static real_T norm(const real_T x[3]);

// Function Definitions

//
// Arguments    : const real_T x[3]
// Return Type  : real_T
//
static real_T b_norm(const real_T x[3])
{
  real_T y;
  real_T scale;
  int32_T k;
  real_T absxk;
  real_T t;
  y = 0.0;
  scale = 2.2250738585072014E-308;
  for (k = 0; k < 3; k++) {
    absxk = fabs(x[k]);
    if (absxk > scale) {
      t = scale / absxk;
      y = 1.0 + y * t * t;
      scale = absxk;
    } else {
      t = absxk / scale;
      y += t * t;
    }
  }

  return scale * sqrt(y);
}

//
// Arguments    : const real_T v_data[]
//                const int32_T v_size[1]
//                real_T d_data[]
//                int32_T d_size[2]
// Return Type  : void
//
static void diag(const real_T v_data[], const int32_T v_size[1], real_T d_data[],
                 int32_T d_size[2])
{
  int32_T loop_ub;
  int32_T i2;
  d_size[0] = (int8_T)v_size[0];
  d_size[1] = (int8_T)v_size[0];
  loop_ub = (int8_T)v_size[0] * (int8_T)v_size[0];
  for (i2 = 0; i2 < loop_ub; i2++) {
    d_data[i2] = 0.0;
  }

  for (loop_ub = 0; loop_ub + 1 <= v_size[0]; loop_ub++) {
    d_data[loop_ub + (int8_T)v_size[0] * loop_ub] = v_data[loop_ub];
  }
}

//
// Arguments    : int32_T n
//                const real_T A[9]
//                const real_T B_data[]
//                real_T C_data[]
// Return Type  : void
//
static void eml_xgemm(int32_T n, const real_T A[9], const real_T B_data[],
                      real_T C_data[])
{
  int32_T c;
  int32_T cr;
  int32_T ic;
  int32_T br;
  int32_T ar;
  int32_T ib;
  int32_T ia;
  if (n == 0) {
  } else {
    c = 3 * (n - 1);
    for (cr = 0; cr <= c; cr += 3) {
      for (ic = cr + 1; ic <= cr + 3; ic++) {
        C_data[ic - 1] = 0.0;
      }
    }

    br = 0;
    for (cr = 0; cr <= c; cr += 3) {
      ar = -1;
      for (ib = br; ib + 1 <= br + 3; ib++) {
        if (B_data[ib] != 0.0) {
          ia = ar;
          for (ic = cr; ic + 1 <= cr + 3; ic++) {
            ia++;
            C_data[ic] += B_data[ib] * A[ia];
          }
        }

        ar += 3;
      }

      br += 3;
    }
  }
}

//
// =====================================
//  anchorPos = [x1 x2 x3 x4 x5;
//               y1 y2 y3 y4 y5;
//               z1 z2 z3 z4 z5]
// =====================================
//  a1 = 0;a2 = 0;a3 = 0;
//  b1 = 0;b2 = 0;b3 = 0;
//  c1 = 0;c2 = 0;c3 = 0;
//
//  d1 = 0;d2 = 0;d3 = 0;
//
//  D1 = 0;D2 = 0;D3 = 0;
//
//  ex = zeros(1,3);
//  ey = zeros(1,3);
//  ez = zeros(1,3);
//  d = 0;
//  i = 0;
//  j = 0;
//  DELTA = 0;
//  z_TRI1 = 0;
//  z_TRI2 = 0;
//  x = 0;
//  y = 0;
//  z = 0;
//
//  X1 = zeros(1,3);
//  X2 = zeros(1,3);
// Arguments    : uint8_T numAvailble
//                const real_T anchorPos_data[]
//                const int32_T anchorPos_size[2]
//                const real_T Range_data[]
//                real_T *x_TRI
//                real_T *y_TRI
//                real_T *z_TRI
// Return Type  : void
//
static void iniEstimate(uint8_T numAvailble, const real_T anchorPos_data[],
  const int32_T anchorPos_size[2], const real_T Range_data[], real_T *x_TRI,
  real_T *y_TRI, real_T *z_TRI)
{
  real_T ex[3];
  int32_T i1;
  real_T d;
  real_T c3;
  real_T z;
  real_T ey[3];
  real_T anchorPos[3];
  real_T x;
  real_T ez[3];
  real_T X1[3];
  real_T a1;
  real_T a2;
  real_T a3;
  real_T b1;
  real_T b2;
  real_T b3;
  real_T D1;
  real_T D2;
  real_T DELTA;

  // 'nlsTRI345:107' x_TRI = 0;
  *x_TRI = 0.0;

  // 'nlsTRI345:108' y_TRI = 0;
  *y_TRI = 0.0;

  // 'nlsTRI345:109' z_TRI = 1;
  *z_TRI = 1.0;

  // 'nlsTRI345:111' if numAvailble == 3
  if (numAvailble == 3) {
    // 'nlsTRI345:112' ex = (anchorPos(:,2) - anchorPos(:,1))';
    for (i1 = 0; i1 < 3; i1++) {
      ex[i1] = anchorPos_data[i1 + anchorPos_size[0]] - anchorPos_data[i1];
    }

    // 'nlsTRI345:113' ex = ex / norm(ex);
    d = norm(ex);

    // 'nlsTRI345:114' i = ex * (anchorPos(:,3) - anchorPos(:,1));
    c3 = 0.0;
    for (i1 = 0; i1 < 3; i1++) {
      z = ex[i1] / d;
      c3 += z * (anchorPos_data[i1 + (anchorPos_size[0] << 1)] -
                 anchorPos_data[i1]);
      ex[i1] = z;
    }

    // 'nlsTRI345:115' ey = (anchorPos(:,3) - anchorPos(:,1))' - i * ex;
    for (i1 = 0; i1 < 3; i1++) {
      ey[i1] = (anchorPos_data[i1 + (anchorPos_size[0] << 1)] -
                anchorPos_data[i1]) - c3 * ex[i1];
    }

    // 'nlsTRI345:116' ey = ey / norm(ey);
    d = norm(ey);

    // 'nlsTRI345:117' d = norm(anchorPos(:,2) - anchorPos(:,1));
    for (i1 = 0; i1 < 3; i1++) {
      anchorPos[i1] = anchorPos_data[i1 + anchorPos_size[0]] - anchorPos_data[i1];
      ey[i1] /= d;
    }

    d = b_norm(anchorPos);

    // 'nlsTRI345:118' j = ey * (anchorPos(:,3) - anchorPos(:,1));
    z = 0.0;
    for (i1 = 0; i1 < 3; i1++) {
      z += ey[i1] * (anchorPos_data[i1 + (anchorPos_size[0] << 1)] -
                     anchorPos_data[i1]);
    }

    // 'nlsTRI345:119' x = Range(1)^2 - Range(2)^2 + d^2;
    // 'nlsTRI345:120' x = x / (2*d);
    x = ((Range_data[0] * Range_data[0] - Range_data[1] * Range_data[1]) + d * d)
      / (2.0 * d);

    // 'nlsTRI345:121' y = (Range(1)^2 - Range(3)^2 +i^2 + j^2)/(2*j) - i*x/j;
    d = (((Range_data[0] * Range_data[0] - Range_data[2] * Range_data[2]) + c3 *
          c3) + z * z) / (2.0 * z) - c3 * x / z;

    // 'nlsTRI345:122' z = sqrt(Range(1)^2 - x^2 - y^2);
    z = sqrt((Range_data[0] * Range_data[0] - x * x) - d * d);

    // 'nlsTRI345:123' ez = [ex(1,2)*ey(1,3)-ey(1,2)*ex(1,3),ex(1,3)*ey(1,1)-ex(1,1)*ey(1,3),ex(1,1)*ey(1,2)-ey(1,1)*ex(1,2)]; 
    ez[0] = ex[1] * ey[2] - ey[1] * ex[2];
    ez[1] = ex[2] * ey[0] - ex[0] * ey[2];
    ez[2] = ex[0] * ey[1] - ey[0] * ex[1];

    // 'nlsTRI345:125' X1 = anchorPos(:,1)' + x*ex + y*ey + z*ez;
    for (i1 = 0; i1 < 3; i1++) {
      X1[i1] = ((anchorPos_data[i1] + x * ex[i1]) + d * ey[i1]) + z * ez[i1];

      // 'nlsTRI345:126' X2 = anchorPos(:,1)' + x*ex + y*ey - z*ez;
      ex[i1] = ((anchorPos_data[i1] + x * ex[i1]) + d * ey[i1]) - z * ez[i1];
    }

    // 'nlsTRI345:128' if (X1(1,3) <= 0) && (X2(1,3) > 0)
    if ((X1[2] <= 0.0) && (ex[2] > 0.0)) {
      // 'nlsTRI345:129' x_TRI = X1(1,1);
      *x_TRI = X1[0];

      // 'nlsTRI345:130' y_TRI = X1(1,2);
      *y_TRI = X1[1];

      // 'nlsTRI345:131' z_TRI = X1(1,3);
      *z_TRI = X1[2];
    }

    // 'nlsTRI345:134' if (X1(1,3) > 0) && (X2(1,3) <= 0)
    if ((X1[2] > 0.0) && (ex[2] <= 0.0)) {
      // 'nlsTRI345:135' x_TRI = X2(1,1);
      *x_TRI = ex[0];

      // 'nlsTRI345:136' y_TRI = X2(1,2);
      *y_TRI = ex[1];

      // 'nlsTRI345:137' z_TRI = X2(1,3);
      *z_TRI = ex[2];
    }
  }

  // 'nlsTRI345:142' if numAvailble > 3
  if (numAvailble > 3) {
    // 'nlsTRI345:143' a1 = anchorPos(1,2)-anchorPos(1,1);
    a1 = anchorPos_data[anchorPos_size[0]] - anchorPos_data[0];

    // 'nlsTRI345:144' a2 = anchorPos(1,3)-anchorPos(1,1);
    a2 = anchorPos_data[anchorPos_size[0] << 1] - anchorPos_data[0];

    // 'nlsTRI345:145' a3 = anchorPos(1,4)-anchorPos(1,1);
    a3 = anchorPos_data[anchorPos_size[0] * 3] - anchorPos_data[0];

    // 'nlsTRI345:147' b1 = anchorPos(2,2)-anchorPos(2,1);
    b1 = anchorPos_data[1 + anchorPos_size[0]] - anchorPos_data[1];

    // 'nlsTRI345:148' b2 = anchorPos(2,3)-anchorPos(2,1);
    b2 = anchorPos_data[1 + (anchorPos_size[0] << 1)] - anchorPos_data[1];

    // 'nlsTRI345:149' b3 = anchorPos(2,4)-anchorPos(2,1);
    b3 = anchorPos_data[1 + anchorPos_size[0] * 3] - anchorPos_data[1];

    // 'nlsTRI345:151' c1 = anchorPos(3,2)-anchorPos(3,1);
    z = anchorPos_data[2 + anchorPos_size[0]] - anchorPos_data[2];

    // 'nlsTRI345:152' c2 = anchorPos(3,3)-anchorPos(3,1);
    x = anchorPos_data[2 + (anchorPos_size[0] << 1)] - anchorPos_data[2];

    // 'nlsTRI345:153' c3 = anchorPos(3,4)-anchorPos(3,1);
    c3 = anchorPos_data[2 + anchorPos_size[0] * 3] - anchorPos_data[2];

    // 'nlsTRI345:155' D1 = 0.5*(Range(1)^2 - Range(2)^2 + norm(anchorPos(:,2)-anchorPos(:,1))^2); 
    for (i1 = 0; i1 < 3; i1++) {
      anchorPos[i1] = anchorPos_data[i1 + anchorPos_size[0]] - anchorPos_data[i1];
      X1[i1] = anchorPos_data[i1 + (anchorPos_size[0] << 1)] - anchorPos_data[i1];
      ex[i1] = anchorPos_data[i1 + anchorPos_size[0] * 3] - anchorPos_data[i1];
    }

    d = b_norm(anchorPos);
    D1 = 0.5 * ((Range_data[0] * Range_data[0] - Range_data[1] * Range_data[1])
                + d * d);

    // 'nlsTRI345:156' D2 = 0.5*(Range(1)^2 - Range(3)^2 + norm(anchorPos(:,3)-anchorPos(:,1))^2); 
    d = b_norm(X1);
    D2 = 0.5 * ((Range_data[0] * Range_data[0] - Range_data[2] * Range_data[2])
                + d * d);

    // 'nlsTRI345:157' D3 = 0.5*(Range(1)^2 - Range(4)^2 + norm(anchorPos(:,4)-anchorPos(:,1))^2); 
    d = b_norm(ex);
    d = 0.5 * ((Range_data[0] * Range_data[0] - Range_data[3] * Range_data[3]) +
               d * d);

    // 'nlsTRI345:159' DELTA = a1*b2*c3 + a2*b3*c1 + a3*b1*c2 - a1*b3*c2 - a2*b1*c3 - a3*b2*c1; 
    DELTA = ((((a1 * b2 * c3 + a2 * b3 * z) + a3 * b1 * x) - a1 * b3 * x) - a2 *
             b1 * c3) - a3 * b2 * z;

    // 'nlsTRI345:160' if DELTA ~= 0
    if (DELTA != 0.0) {
      // 'nlsTRI345:161' DELTA = 1/DELTA;
      DELTA = 1.0 / DELTA;
    }

    // 'nlsTRI345:163' x_TRI = anchorPos(1,1) + DELTA*(D1*b2*c3 + D2*b3*c1 + D3*b1*c2 - D1*b3*c2 - D2*b1*c3 - D3*b2*c1); 
    *x_TRI = anchorPos_data[0] + DELTA * (((((D1 * b2 * c3 + D2 * b3 * z) + d *
      b1 * x) - D1 * b3 * x) - D2 * b1 * c3) - d * b2 * z);

    // 'nlsTRI345:164' y_TRI = anchorPos(2,1) + DELTA*(a1*D2*c3 + a2*D3*c1 + a3*D1*c2 - a1*D3*c2 - a2*D1*c3 - a3*D2*c1); 
    *y_TRI = anchorPos_data[1] + DELTA * (((((a1 * D2 * c3 + a2 * d * z) + a3 *
      D1 * x) - a1 * d * x) - a2 * D1 * c3) - a3 * D2 * z);

    // 'nlsTRI345:165' z_TRI = anchorPos(3,1) + DELTA*(a1*b2*D3 + a2*b3*D1 + a3*b1*D2 - a1*b3*D2 - a2*b1*D3 - a3*b2*D1); 
    *z_TRI = anchorPos_data[2] + DELTA * (((((a1 * b2 * d + a2 * b3 * D1) + a3 *
      b1 * D2) - a1 * b3 * D2) - a2 * b1 * d) - a3 * b2 * D1);
  }
}

//
// Arguments    : const real_T a[9]
//                real_T c[9]
// Return Type  : void
//
static void mpower(const real_T a[9], real_T c[9])
{
  real_T x[9];
  int32_T p1;
  int32_T p2;
  int32_T p3;
  real_T absx11;
  real_T absx21;
  real_T absx31;
  int32_T itmp;
  real_T y;
  memcpy(&x[0], &a[0], 9U * sizeof(real_T));
  p1 = 0;
  p2 = 3;
  p3 = 6;
  absx11 = fabs(a[0]);
  absx21 = fabs(a[1]);
  absx31 = fabs(a[2]);
  if ((absx21 > absx11) && (absx21 > absx31)) {
    p1 = 3;
    p2 = 0;
    x[0] = a[1];
    x[1] = a[0];
    x[3] = a[4];
    x[4] = a[3];
    x[6] = a[7];
    x[7] = a[6];
  } else {
    if (absx31 > absx11) {
      p1 = 6;
      p3 = 0;
      x[0] = a[2];
      x[2] = a[0];
      x[3] = a[5];
      x[5] = a[3];
      x[6] = a[8];
      x[8] = a[6];
    }
  }

  absx21 = x[1] / x[0];
  x[1] /= x[0];
  absx11 = x[2] / x[0];
  x[2] /= x[0];
  x[4] -= absx21 * x[3];
  x[5] -= absx11 * x[3];
  x[7] -= absx21 * x[6];
  x[8] -= absx11 * x[6];
  if (fabs(x[5]) > fabs(x[4])) {
    itmp = p2;
    p2 = p3;
    p3 = itmp;
    x[1] = absx11;
    x[2] = absx21;
    absx11 = x[4];
    x[4] = x[5];
    x[5] = absx11;
    absx11 = x[7];
    x[7] = x[8];
    x[8] = absx11;
  }

  absx31 = x[5];
  y = x[4];
  absx21 = x[5] / x[4];
  x[8] -= absx21 * x[7];
  absx11 = (absx21 * x[1] - x[2]) / x[8];
  absx21 = -(x[1] + x[7] * absx11) / x[4];
  c[p1] = ((1.0 - x[3] * absx21) - x[6] * absx11) / x[0];
  c[p1 + 1] = absx21;
  c[p1 + 2] = absx11;
  absx11 = -(absx31 / y) / x[8];
  absx21 = (1.0 - x[7] * absx11) / x[4];
  c[p2] = -(x[3] * absx21 + x[6] * absx11) / x[0];
  c[p2 + 1] = absx21;
  c[p2 + 2] = absx11;
  absx11 = 1.0 / x[8];
  absx21 = -x[7] * absx11 / x[4];
  c[p3] = -(x[3] * absx21 + x[6] * absx11) / x[0];
  c[p3 + 1] = absx21;
  c[p3 + 2] = absx11;
}

//
// Arguments    : const real_T x[3]
// Return Type  : real_T
//
static real_T norm(const real_T x[3])
{
  real_T y;
  real_T scale;
  int32_T k;
  real_T absxk;
  real_T t;
  y = 0.0;
  scale = 2.2250738585072014E-308;
  for (k = 0; k < 3; k++) {
    absxk = fabs(x[k]);
    if (absxk > scale) {
      t = scale / absxk;
      y = 1.0 + y * t * t;
      scale = absxk;
    } else {
      t = absxk / scale;
      y += t * t;
    }
  }

  return scale * sqrt(y);
}

//
// nlsTRI345 Computes Nonlinear Least Squares solution for [numAvailble x numAvailble] ('numAvailble' range equations, 3 unknowns)
//  X h_matrix: seed locations
//  eStop: computation will stop when delta(norm(Xh) is less than eStop, or
//  iterNumMax: computation will stop when number of iterations j >= iterNumMax
//  varianceR: distance measurements variance [numAvailble*numAvailble]
//  range: [numAvailble x 1]
//  j: iterative times
//  x,y,z: anchors' position
//  Copyright @2016 IoT Lab, Kexin Guo
// Arguments    : uint8_T numAvailble
//                const real_T anchorPos_data[]
//                const int32_T anchorPos_size[2]
//                const real_T Range_data[]
//                const int32_T Range_size[2]
//                const real_T w_data[]
//                const int32_T w_size[1]
//                real_T eStop
//                uint32_T iterNumMax
//                const real_T seed[3]
//                boolean_T use_seed
//                real_T X[3]
//                real_T *iterTimes
//                real_T *eTest
// Return Type  : void
//
void nlsTRI345(uint8_T numAvailble, const real_T anchorPos_data[], const int32_T
               anchorPos_size[2], const real_T Range_data[], const int32_T
               Range_size[2], const real_T w_data[], const int32_T w_size[1],
               real_T eStop, uint32_T iterNumMax, const real_T seed[3],
               boolean_T use_seed, real_T X[3], real_T *iterTimes, real_T *eTest)
{
  int32_T loop_ub;
  int32_T i0;
  real_T J_data[765];
  real_T F_data[255];
  real_T rangePredict_data[255];
  real_T z_TRI;
  real_T y_TRI;
  real_T x_TRI;
  int32_T W_size[2];
  real_T W_data[25];
  uint8_T i;
  real_T a;
  real_T b_a;
  real_T c_a;
  int32_T cr;
  real_T a_data[765];
  int32_T y_size_idx_1;
  real_T y_data[15];
  int32_T br;
  int32_T ar;
  int32_T ic;
  int32_T ib;
  int32_T ia;
  real_T y[9];
  real_T b_y[9];
  real_T b_y_data[765];
  real_T H[3];
  (void)Range_size;

  // 'nlsTRI345:13' variance_range = 0.01;
  //  W = eye(numAvailble) * (1/variance_range);
  // 'nlsTRI345:15' J = zeros(numAvailble,3);
  loop_ub = numAvailble * 3;
  for (i0 = 0; i0 < loop_ub; i0++) {
    J_data[i0] = 0.0;
  }

  // 'nlsTRI345:16' F = zeros(numAvailble,1);
  loop_ub = numAvailble;
  for (i0 = 0; i0 < loop_ub; i0++) {
    F_data[i0] = 0.0;
  }

  // 'nlsTRI345:17' rangePredict = zeros(numAvailble,1);
  loop_ub = numAvailble;
  for (i0 = 0; i0 < loop_ub; i0++) {
    rangePredict_data[i0] = 0.0;
  }

  // =====================================
  //  anchorPos = [x1 x2 x3 x4 x5;
  //               y1 y2 y3 y4 y5;
  //               z1 z2 z3 z4 z5]
  // =====================================
  //  x_ini = 0;
  //  y_ini = 0;
  //  z_ini = 0;
  //
  //  x_TRI = x_ini;
  //  y_TRI = y_ini;
  //  z_TRI = z_ini;
  // 'nlsTRI345:33' if use_seed
  if (use_seed) {
    // 'nlsTRI345:34' [x_TRI, y_TRI, z_TRI] = iniEstimate(numAvailble,anchorPos,Range); 
    iniEstimate(numAvailble, anchorPos_data, anchorPos_size, Range_data, &x_TRI,
                &y_TRI, &z_TRI);
  } else {
    // 'nlsTRI345:35' else
    // 'nlsTRI345:36' x_TRI = seed(1);
    x_TRI = seed[0];

    // 'nlsTRI345:37' y_TRI = seed(2);
    y_TRI = seed[1];

    // 'nlsTRI345:38' z_TRI = seed(3);
    z_TRI = seed[2];
  }

  // 'nlsTRI345:41' iterTimes = 0;
  *iterTimes = 0.0;

  // 'nlsTRI345:42' eTest = 100;
  *eTest = 100.0;

  // 'nlsTRI345:43' W = diag(w);
  diag(w_data, w_size, W_data, W_size);

  // 'nlsTRI345:44' while (eTest > eStop) && (iterTimes < iterNumMax)
  while ((*eTest > eStop) && ((uint32_T)*iterTimes < iterNumMax)) {
    // 'nlsTRI345:46' for i = 1:numAvailble
    for (i = 1; i <= numAvailble; i++) {
      // 'nlsTRI345:48' rangePredict(i,1) = sqrt((x_TRI-anchorPos(1,i))^2 + (y_TRI-anchorPos(2,i))^2 + (z_TRI-anchorPos(3,i))^2); 
      a = x_TRI - anchorPos_data[anchorPos_size[0] * (i - 1)];
      b_a = y_TRI - anchorPos_data[1 + anchorPos_size[0] * (i - 1)];
      c_a = z_TRI - anchorPos_data[2 + anchorPos_size[0] * (i - 1)];
      rangePredict_data[i - 1] = sqrt((a * a + b_a * b_a) + c_a * c_a);

      // 'nlsTRI345:50' J(i,1) = (x_TRI - anchorPos(1,i))/rangePredict(i,1);
      J_data[i - 1] = (x_TRI - anchorPos_data[anchorPos_size[0] * (i - 1)]) /
        rangePredict_data[i - 1];

      // 'nlsTRI345:51' J(i,2) = (y_TRI - anchorPos(2,i))/rangePredict(i,1);
      J_data[(i + numAvailble) - 1] = (y_TRI - anchorPos_data[1 +
        anchorPos_size[0] * (i - 1)]) / rangePredict_data[i - 1];

      // 'nlsTRI345:52' J(i,3) = (z_TRI - anchorPos(3,i))/rangePredict(i,1);
      J_data[(i + (numAvailble << 1)) - 1] = (z_TRI - anchorPos_data[2 +
        anchorPos_size[0] * (i - 1)]) / rangePredict_data[i - 1];

      // 'nlsTRI345:54' F(i,1) = Range(i) - rangePredict(i,1);
      F_data[i - 1] = Range_data[i - 1] - rangePredict_data[i - 1];
    }

    //      if all(~isnan(W(:)))
    //          H = ((J'*W*J)^-1)*J'*W*F;
    //      else
    // 'nlsTRI345:60' H = ((J'*W*J)^-1)*J'*W*F;
    loop_ub = numAvailble;
    for (i0 = 0; i0 < loop_ub; i0++) {
      for (cr = 0; cr < 3; cr++) {
        a_data[cr + 3 * i0] = J_data[i0 + numAvailble * cr];
      }
    }

    if ((numAvailble == 1) || (W_size[0] == 1)) {
      y_size_idx_1 = W_size[1];
      for (i0 = 0; i0 < 3; i0++) {
        loop_ub = W_size[1];
        for (cr = 0; cr < loop_ub; cr++) {
          y_data[i0 + 3 * cr] = 0.0;
          br = numAvailble;
          for (ar = 0; ar < br; ar++) {
            y_data[i0 + 3 * cr] += a_data[i0 + 3 * ar] * W_data[ar + W_size[0] *
              cr];
          }
        }
      }
    } else {
      y_size_idx_1 = (uint8_T)W_size[1];
      loop_ub = 3 * (uint8_T)W_size[1];
      for (i0 = 0; i0 < loop_ub; i0++) {
        y_data[i0] = 0.0;
      }

      if (W_size[1] == 0) {
      } else {
        loop_ub = 3 * (W_size[1] - 1);
        for (cr = 0; cr <= loop_ub; cr += 3) {
          for (ic = cr; ic + 1 <= cr + 3; ic++) {
            y_data[ic] = 0.0;
          }
        }

        br = 0;
        for (cr = 0; cr <= loop_ub; cr += 3) {
          ar = 0;
          i0 = br + numAvailble;
          for (ib = br; ib + 1 <= i0; ib++) {
            if (W_data[ib] != 0.0) {
              ia = ar;
              for (ic = cr; ic + 1 <= cr + 3; ic++) {
                ia++;
                y_data[ic] += W_data[ib] * a_data[ia - 1];
              }
            }

            ar += 3;
          }

          br += numAvailble;
        }
      }
    }

    if ((y_size_idx_1 == 1) || (numAvailble == 1)) {
      for (i0 = 0; i0 < 3; i0++) {
        for (cr = 0; cr < 3; cr++) {
          y[i0 + 3 * cr] = 0.0;
          for (ar = 0; ar < y_size_idx_1; ar++) {
            y[i0 + 3 * cr] += y_data[i0 + 3 * ar] * J_data[ar + numAvailble * cr];
          }
        }
      }
    } else {
      memset(&y[0], 0, 9U * sizeof(real_T));
      for (cr = 0; cr < 8; cr += 3) {
        for (ic = cr; ic + 1 <= cr + 3; ic++) {
          y[ic] = 0.0;
        }
      }

      br = 0;
      for (cr = 0; cr < 8; cr += 3) {
        ar = 0;
        i0 = br + y_size_idx_1;
        for (ib = br; ib + 1 <= i0; ib++) {
          if (J_data[ib] != 0.0) {
            ia = ar;
            for (ic = cr; ic + 1 <= cr + 3; ic++) {
              ia++;
              y[ic] += J_data[ib] * y_data[ia - 1];
            }
          }

          ar += 3;
        }

        br += y_size_idx_1;
      }
    }

    memcpy(&b_y[0], &y[0], 9U * sizeof(real_T));
    mpower(b_y, y);
    loop_ub = numAvailble;
    for (i0 = 0; i0 < loop_ub; i0++) {
      for (cr = 0; cr < 3; cr++) {
        a_data[cr + 3 * i0] = J_data[i0 + numAvailble * cr];
      }
    }

    loop_ub = 3 * numAvailble;
    for (i0 = 0; i0 < loop_ub; i0++) {
      b_y_data[i0] = 0.0;
    }

    eml_xgemm(numAvailble, y, a_data, b_y_data);
    if ((numAvailble == 1) || (W_size[0] == 1)) {
      y_size_idx_1 = W_size[1];
      for (i0 = 0; i0 < 3; i0++) {
        loop_ub = W_size[1];
        for (cr = 0; cr < loop_ub; cr++) {
          y_data[i0 + 3 * cr] = 0.0;
          br = numAvailble;
          for (ar = 0; ar < br; ar++) {
            y_data[i0 + 3 * cr] += b_y_data[i0 + 3 * ar] * W_data[ar + W_size[0]
              * cr];
          }
        }
      }
    } else {
      y_size_idx_1 = (uint8_T)W_size[1];
      loop_ub = 3 * (uint8_T)W_size[1];
      for (i0 = 0; i0 < loop_ub; i0++) {
        y_data[i0] = 0.0;
      }

      if (W_size[1] == 0) {
      } else {
        loop_ub = 3 * (W_size[1] - 1);
        for (cr = 0; cr <= loop_ub; cr += 3) {
          for (ic = cr; ic + 1 <= cr + 3; ic++) {
            y_data[ic] = 0.0;
          }
        }

        br = 0;
        for (cr = 0; cr <= loop_ub; cr += 3) {
          ar = 0;
          i0 = br + numAvailble;
          for (ib = br; ib + 1 <= i0; ib++) {
            if (W_data[ib] != 0.0) {
              ia = ar;
              for (ic = cr; ic + 1 <= cr + 3; ic++) {
                ia++;
                y_data[ic] += W_data[ib] * b_y_data[ia - 1];
              }
            }

            ar += 3;
          }

          br += numAvailble;
        }
      }
    }

    if ((y_size_idx_1 == 1) || (numAvailble == 1)) {
      for (i0 = 0; i0 < 3; i0++) {
        H[i0] = 0.0;
        for (cr = 0; cr < y_size_idx_1; cr++) {
          H[i0] += y_data[i0 + 3 * cr] * F_data[cr];
        }
      }
    } else {
      for (ic = 0; ic < 3; ic++) {
        H[ic] = 0.0;
      }

      ar = 0;
      for (ib = 0; ib + 1 <= y_size_idx_1; ib++) {
        if (F_data[ib] != 0.0) {
          ia = ar;
          for (ic = 0; ic < 3; ic++) {
            ia++;
            H[ic] += F_data[ib] * y_data[ia - 1];
          }
        }

        ar += 3;
      }
    }

    //      end
    // 'nlsTRI345:63' x_TRI = x_TRI + H(1);
    x_TRI += H[0];

    // 'nlsTRI345:64' y_TRI = y_TRI + H(2);
    y_TRI += H[1];

    // 'nlsTRI345:65' z_TRI = z_TRI + H(3);
    z_TRI += H[2];

    // 'nlsTRI345:67' iterTimes = iterTimes + 1;
    (*iterTimes)++;

    // 'nlsTRI345:69' eTest = norm(H);
    *eTest = b_norm(H);
  }

  // 'nlsTRI345:72' X = [x_TRI; y_TRI; z_TRI];
  X[0] = x_TRI;
  X[1] = y_TRI;
  X[2] = z_TRI;
}

//
// Arguments    : void
// Return Type  : void
//
void nlsTRI345_initialize(void)
{
}

//
// Arguments    : void
// Return Type  : void
//
void nlsTRI345_terminate(void)
{
  // (no terminate code required)
}

//
// File trailer for nlsTRI345.c
//
// [EOF]
//
