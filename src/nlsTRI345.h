//
// File: nlsTRI345.h
//
// MATLAB Coder version            : 2.6
// C/C++ source code generated on  : 03-Aug-2016 19:31:22
//
#ifndef __NLSTRI345_H__
#define __NLSTRI345_H__

// Include files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rtwtypes.h"
#include "nlsTRI345_types.h"

// Function Declarations
extern void nlsTRI345(uint8_T numAvailble, const real_T anchorPos_data[], const
                      int32_T anchorPos_size[2], const real_T Range_data[],
                      const int32_T Range_size[2], const real_T w_data[], const
                      int32_T w_size[1], real_T eStop, uint32_T iterNumMax,
                      const real_T seed[3], boolean_T use_seed, real_T X[3],
                      real_T *iterTimes, real_T *eTest);
extern void nlsTRI345_initialize(void);
extern void nlsTRI345_terminate(void);

#endif

//
// File trailer for nlsTRI345.h
//
// [EOF]
//
