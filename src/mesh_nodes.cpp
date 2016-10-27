//
// File: mesh_nodes.c
//
// MATLAB Coder version            : 2.6
// C/C++ source code generated on  : 25-Jan-2016 06:09:13
//

// Include files
#include "mesh_nodes.h"

// Function Declarations
static unsigned int div_nzp_u32_round(unsigned int numerator, unsigned int
  denominator);

// Function Definitions

//
// Arguments    : unsigned int numerator
//                unsigned int denominator
// Return Type  : unsigned int
//
static unsigned int div_nzp_u32_round(unsigned int numerator, unsigned int
  denominator)
{
  unsigned int quotient;
  quotient = numerator / denominator;
  numerator %= denominator;
  if (numerator > 2147483647U) {
    quotient++;
  } else {
    numerator <<= 1U;
    if (numerator >= denominator) {
      quotient++;
    }
  }

  return quotient;
}

//
// Arguments    : const unsigned char u[6]
//                unsigned char uCount
//                const unsigned char a[6]
//                unsigned char aCount
//                const unsigned char s[2]
//                unsigned char unichan
//                unsigned char mode
//                unsigned char smU[576]
//                unsigned char smA[576]
//                unsigned char *trueMapSize
// Return Type  : void
//
void mesh_nodes(const unsigned char u[6], unsigned char uCount, const unsigned
                char a[6], unsigned char aCount, const unsigned char s[2],
                unsigned char unichan, unsigned char mode, unsigned char smU[576],
                unsigned char smA[576], unsigned char *trueMapSize)
{
  int i;
  unsigned char conU[32];
  unsigned char conA[32];
  unsigned char k;
  unsigned char r;
  int count;
  unsigned char gk;
  unsigned char gklast;
  boolean_T ancFree;
  unsigned char n;

  // 'mesh_nodes:4' u = u';
  // 'mesh_nodes:5' a = a';
  // 'mesh_nodes:7' smU = uint8(zeros(32, 3, 6));
  for (i = 0; i < 576; i++) {
    smU[i] = 0;

    // 'mesh_nodes:8' smA = uint8(zeros(32, 3, 6));
    smA[i] = 0;
  }

  // 'mesh_nodes:9' mapSize = 32;
  // 'mesh_nodes:11' if mode == 'u'
  if (mode == 117) {
    // Distributed slotmap should have the size of (anchors*mobile nodes + 1sync) 
    // 'mesh_nodes:13' trueMapSize = uCount*aCount + 1;
    *trueMapSize = (unsigned char)((unsigned char)((unsigned int)uCount * aCount)
      + 1U);

    //      %Create slot matrices for uavs
    //      smU = uint8(zeros(mapSize, 3, nU));
    //      smA = uint8(zeros(mapSize, 3, nA));
    // All anchors are replicated by the number of mobile nodes and concantenated 
    // to the initial slot of to anchors ranging to each other.
    // 'mesh_nodes:21' conU = zeros(mapSize, 1);
    for (i = 0; i < 32; i++) {
      conU[i] = 0;

      // 'mesh_nodes:22' conA = zeros(mapSize, 1);
      conA[i] = 0;
    }

    // 'mesh_nodes:24' conU(1) = s(1);
    conU[0] = s[0];

    // 'mesh_nodes:25' conA(1) = s(2);
    conA[0] = s[1];

    // 'mesh_nodes:27' for k = 1:trueMapSize-1
    for (k = 0; (unsigned char)(k - 255U) <= (unsigned char)(*trueMapSize + 255U);
         k++) {
      // 'mesh_nodes:28' conU(k+1) = u(mod(k-1, uCount) + 1);
      if (uCount == 0) {
        r = k;
      } else {
        r = (unsigned char)((unsigned int)k - (unsigned char)((unsigned int)k /
          uCount * uCount));
      }

      conU[(unsigned char)(k - 254U) - 1] = u[(unsigned char)(r + 1U) - 1];
    }

    // 'mesh_nodes:31' indA = 1;
    i = 0;

    // 'mesh_nodes:32' count = 0;
    count = 0;

    // 'mesh_nodes:33' for k = 2:trueMapSize
    for (k = 2; k <= *trueMapSize; k++) {
      // 'mesh_nodes:34' conA(k) = a(indA);
      conA[k - 1] = a[i];

      // 'mesh_nodes:35' count = count + 1;
      count++;

      // 'mesh_nodes:36' if count >= uCount
      if (count >= uCount) {
        // 'mesh_nodes:37' indA = indA+1;
        i++;

        // 'mesh_nodes:38' count = 0;
        count = 0;
      }
    }

    // Populating the slotmap of the uavs
    // 'mesh_nodes:43' for k = 1:uCount
    for (k = 1; k <= uCount; k++) {
      // 'mesh_nodes:44' smU(:, 1, k) = conU;
      // 'mesh_nodes:45' smU(:, 2, k) = conA;
      // 'mesh_nodes:46' smU(:, 3, k) = unichan;
      for (i = 0; i < 32; i++) {
        smU[i + 96 * (k - 1)] = conU[i];
        smU[32 + (i + 96 * (k - 1))] = conA[i];
        smU[64 + (i + 96 * (k - 1))] = unichan;
      }
    }

    // Populating the slotmap of the anchors
    // 'mesh_nodes:50' for k = 1:aCount
    for (k = 1; k <= aCount; k++) {
      // 'mesh_nodes:51' smA(:, 1, k) = conU;
      // 'mesh_nodes:52' smA(:, 2, k) = conA;
      // 'mesh_nodes:53' smA(:, 3, k) = unichan;
      for (i = 0; i < 32; i++) {
        smA[i + 96 * (k - 1)] = conU[i];
        smA[32 + (i + 96 * (k - 1))] = conA[i];
        smA[64 + (i + 96 * (k - 1))] = unichan;
      }
    }
  } else {
    // 'mesh_nodes:56' else
    // Find the least common multiple of the number of anchors and uavs
    // 'mesh_nodes:59' trueMapSize = lcm(uCount, aCount) + 1;
    if (aCount == 0) {
      gk = uCount;
    } else {
      gk = aCount;
      for (r = (unsigned char)((unsigned int)uCount - (unsigned char)(aCount *
             ((unsigned int)uCount / aCount))); r != 0; r = (unsigned char)
           ((unsigned int)gklast - (unsigned char)(r * ((unsigned int)gklast / r))))
      {
        gklast = gk;
        gk = r;
      }
    }

    gk = (unsigned char)((unsigned int)(unsigned char)div_nzp_u32_round(uCount,
      gk) * aCount);
    *trueMapSize = (unsigned char)(gk + 1U);

    //      %Create slot matrices for uavs
    //      smU = uint8(zeros(mapSize + 1, 3, nU));
    // Concantenate a long vector of anchors to put to the slotmap
    // 'mesh_nodes:65' conA = uint8(zeros(mapSize, 1));
    for (i = 0; i < 32; i++) {
      conA[i] = 0;
    }

    // 'mesh_nodes:66' for k = 1:trueMapSize-1
    for (k = 1; k <= (unsigned char)((unsigned char)(gk + 1U) + 255U); k++) {
      // 'mesh_nodes:67' conA(k) = a(mod(k - 1, aCount) + 1);
      gklast = (unsigned char)(k - 1);
      if (aCount == 0) {
      } else {
        gklast = (unsigned char)((unsigned int)gklast - (unsigned char)
          ((unsigned int)gklast / aCount * aCount));
      }

      conA[k - 1] = a[(unsigned char)(gklast + 1U) - 1];
    }

    // Populating the slotmap of the uavs
    // 'mesh_nodes:70' for k = 1:uCount
    for (k = 1; k <= uCount; k++) {
      // 'mesh_nodes:71' smU(1, :, k) = [s(1) s(2) 6];
      smU[96 * (k - 1)] = s[0];
      smU[32 + 96 * (k - 1)] = s[1];
      smU[64 + 96 * (k - 1)] = 6;

      // 'mesh_nodes:73' for j = 1:trueMapSize-1
      for (gklast = 1; gklast <= (unsigned char)((unsigned char)(gk + 1U) + 255U);
           gklast++) {
        // 'mesh_nodes:74' smU(1+j, 1, k) = u(k);
        smU[gklast + 96 * (k - 1)] = u[k - 1];

        // 'mesh_nodes:75' smU(1+j, 2, k) = conA(j);
        smU[32 + (gklast + 96 * (k - 1))] = conA[gklast - 1];

        // 'mesh_nodes:76' smU(1+j, 3, k) = k;
        smU[64 + (gklast + 96 * (k - 1))] = k;
      }

      // Rotate conA for one unit
      // 'mesh_nodes:82' tempA = conA(1);
      r = conA[0];

      // 'mesh_nodes:83' for j = 1 : trueMapSize-1
      for (gklast = 1; gklast <= (unsigned char)((unsigned char)(gk + 1U) + 255U);
           gklast++) {
        // 'mesh_nodes:84' conA(j) = conA(j+1);
        conA[gklast - 1] = conA[gklast];
      }

      // 'mesh_nodes:86' conA(trueMapSize-1) = tempA;
      conA[(unsigned char)((unsigned char)(gk + 1U) - 1U) - 1] = r;
    }

    //      %Create slot matrices for anchors
    //      smA = uint8(zeros(mapSize + 1,3, nU));
    // Populating the slotmap of the anchors
    // 'mesh_nodes:94' for k = 1:aCount
    for (k = 1; k <= aCount; k++) {
      // 'mesh_nodes:95' smA(1, :, k) = [s(1) s(2) 6];
      smA[96 * (k - 1)] = s[0];
      smA[32 + 96 * (k - 1)] = s[1];
      smA[64 + 96 * (k - 1)] = 6;

      // 'mesh_nodes:96' for j = 1:trueMapSize-1
      for (gklast = 1; gklast <= (unsigned char)((unsigned char)(gk + 1U) + 255U);
           gklast++) {
        // 'mesh_nodes:97' smA(1+j, 2, k) = a(k);
        smA[((unsigned char)(gklast - 255U) + 96 * (k - 1)) + 31] = a[k - 1];
      }

      // 'mesh_nodes:99' for m = 2: trueMapSize
      for (gklast = 2; gklast <= (unsigned char)(gk + 1U); gklast++) {
        // 'mesh_nodes:100' ancFree = true;
        ancFree = true;

        // 'mesh_nodes:101' for n = 1:uCount
        r = 1;
        for (n = 1; n <= uCount; n++) {
          r = n;

          // 'mesh_nodes:102' if smU(m, 2, n) == smA(m, 2, k)
          if (smU[(gklast + 96 * (n - 1)) + 31] == smA[(gklast + 96 * (k - 1)) +
              31]) {
            // 'mesh_nodes:103' smA(m, 1, k) = smU(m, 1, n)
            smA[(gklast + 96 * (k - 1)) - 1] = smU[(gklast + 96 * (n - 1)) - 1];

            // 'mesh_nodes:104' smA(m, 3, k) = smU(m, 3, n);
            smA[(gklast + 96 * (k - 1)) + 63] = smU[(gklast + 96 * (n - 1)) + 63];

            // 'mesh_nodes:105' ancFree = false;
            ancFree = false;
          }
        }

        // 'mesh_nodes:108' if ancFree
        if (ancFree) {
          // 'mesh_nodes:109' smA(m, :, k) = smU(m, :, n);
          for (i = 0; i < 3; i++) {
            smA[((gklast + (i << 5)) + 96 * (k - 1)) - 1] = smU[((gklast + (i <<
              5)) + 96 * (r - 1)) - 1];
          }
        }
      }
    }
  }
}

//
// Arguments    : void
// Return Type  : void
//
void mesh_nodes_initialize(void)
{
}

//
// Arguments    : void
// Return Type  : void
//
void mesh_nodes_terminate(void)
{
  // (no terminate code required)
}

//
// File trailer for mesh_nodes.c
//
// [EOF]
//
