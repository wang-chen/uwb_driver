//
// File: mesh_nodes.h
//
// MATLAB Coder version            : 2.6
// C/C++ source code generated on  : 25-Jan-2016 06:09:13
//
#ifndef __MESH_NODES_H__
#define __MESH_NODES_H__

// Include files
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "mesh_nodes_types.h"

// Function Declarations
extern void mesh_nodes(const unsigned char u[6], unsigned char uCount, const
  unsigned char a[6], unsigned char aCount, const unsigned char s[2], unsigned
  char unichan, unsigned char mode, unsigned char smU[576], unsigned char smA
  [576], unsigned char *trueMapSize);
extern void mesh_nodes_initialize(void);
extern void mesh_nodes_terminate(void);

#endif

//
// File trailer for mesh_nodes.h
//
// [EOF]
//
