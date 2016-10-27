// 
// File: trilatCalib.cpp
//  
// Code generated for Simulink model 'trilatCalib'.
// 
// Model version                  : 1.353
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Sat Oct 24 16:47:17 2015
// 
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex
// Code generation objectives: 
//    1. Execution efficiency
//    2. Traceability
//    3. Safety precaution
// Validation result: Not run
// 


      #include "trilatCalib.h"

    
















  
  
  
        
    

  

  
  
    
          
    
    // Model step function
            void trilatCalibClass::step(real_T arg_ancs[12], real_T arg_dists[4], boolean_T arg_calibEnable, real_T arg_x_est[3])   
    {
            


      
      
            
      
        int32_T r1;
int32_T r2;
int32_T r3;
int32_T rtemp;
real_T rtb_distsCalibed_idx_0;
real_T rtb_distsCalibed_idx_1;
real_T rtb_distsCalibed_idx_2;
real_T rtb_distsCalibed_idx_3;

        
        

      

        
  



          
        
  



                                  /* MATLAB Function: '<S1>/Calibrator' incorporates:
 *  Inport: '<Root>/calibEnable'
 *  Inport: '<Root>/dists'
 */
/* MATLAB Function 'trilatCalib/Calibrator': '<S2>:1' */
/* '<S2>:1:3' distsCalibed = dists; */
rtb_distsCalibed_idx_0 = arg_dists[0];
rtb_distsCalibed_idx_1 = arg_dists[1];
rtb_distsCalibed_idx_2 = arg_dists[2];
rtb_distsCalibed_idx_3 = arg_dists[3];
/* '<S2>:1:4' if calibEnable */
if (arg_calibEnable) {
    /* '<S2>:1:5' for nodeId =1:length(dists) */
    /* '<S2>:1:6' if 0 < dists(nodeId) && dists(nodeId) < 1.5 */
    if ((0.0 < arg_dists[0]) && (arg_dists[0] < 1.5)) {
        /* '<S2>:1:7' distsCalibed(nodeId) = 1.0447 * dists(nodeId) - 0.1932; */
        rtb_distsCalibed_idx_0 = 1.0447 * arg_dists[0] - 0.1932;
    } else {
        /* '<S2>:1:8' else */
        /* '<S2>:1:9' if dists(nodeId) > 1.5 && dists(nodeId) <= 10 */
        if ((arg_dists[0] > 1.5) && (arg_dists[0] <= 10.0)) {
            /* '<S2>:1:10' distsCalibed(nodeId) = 1.0029 * dists(nodeId) - 0.0829; */
            rtb_distsCalibed_idx_0 = 1.0029 * arg_dists[0] - 0.0829;
        } else {
            /* '<S2>:1:11' else */
            /* '<S2>:1:12' distsCalibed(nodeId) = 0.9976 * dists(nodeId) - 0.0511; */
            rtb_distsCalibed_idx_0 = 0.9976 * arg_dists[0] - 0.0511;
        }
    }
    /* '<S2>:1:6' if 0 < dists(nodeId) && dists(nodeId) < 1.5 */
    if ((0.0 < arg_dists[1]) && (arg_dists[1] < 1.5)) {
        /* '<S2>:1:7' distsCalibed(nodeId) = 1.0447 * dists(nodeId) - 0.1932; */
        rtb_distsCalibed_idx_1 = 1.0447 * arg_dists[1] - 0.1932;
    } else {
        /* '<S2>:1:8' else */
        /* '<S2>:1:9' if dists(nodeId) > 1.5 && dists(nodeId) <= 10 */
        if ((arg_dists[1] > 1.5) && (arg_dists[1] <= 10.0)) {
            /* '<S2>:1:10' distsCalibed(nodeId) = 1.0029 * dists(nodeId) - 0.0829; */
            rtb_distsCalibed_idx_1 = 1.0029 * arg_dists[1] - 0.0829;
        } else {
            /* '<S2>:1:11' else */
            /* '<S2>:1:12' distsCalibed(nodeId) = 0.9976 * dists(nodeId) - 0.0511; */
            rtb_distsCalibed_idx_1 = 0.9976 * arg_dists[1] - 0.0511;
        }
    }
    /* '<S2>:1:6' if 0 < dists(nodeId) && dists(nodeId) < 1.5 */
    if ((0.0 < arg_dists[2]) && (arg_dists[2] < 1.5)) {
        /* '<S2>:1:7' distsCalibed(nodeId) = 1.0447 * dists(nodeId) - 0.1932; */
        rtb_distsCalibed_idx_2 = 1.0447 * arg_dists[2] - 0.1932;
    } else {
        /* '<S2>:1:8' else */
        /* '<S2>:1:9' if dists(nodeId) > 1.5 && dists(nodeId) <= 10 */
        if ((arg_dists[2] > 1.5) && (arg_dists[2] <= 10.0)) {
            /* '<S2>:1:10' distsCalibed(nodeId) = 1.0029 * dists(nodeId) - 0.0829; */
            rtb_distsCalibed_idx_2 = 1.0029 * arg_dists[2] - 0.0829;
        } else {
            /* '<S2>:1:11' else */
            /* '<S2>:1:12' distsCalibed(nodeId) = 0.9976 * dists(nodeId) - 0.0511; */
            rtb_distsCalibed_idx_2 = 0.9976 * arg_dists[2] - 0.0511;
        }
    }
    /* '<S2>:1:6' if 0 < dists(nodeId) && dists(nodeId) < 1.5 */
    if ((0.0 < arg_dists[3]) && (arg_dists[3] < 1.5)) {
        /* '<S2>:1:7' distsCalibed(nodeId) = 1.0447 * dists(nodeId) - 0.1932; */
        rtb_distsCalibed_idx_3 = 1.0447 * arg_dists[3] - 0.1932;
    } else {
        /* '<S2>:1:8' else */
        /* '<S2>:1:9' if dists(nodeId) > 1.5 && dists(nodeId) <= 10 */
        if ((arg_dists[3] > 1.5) && (arg_dists[3] <= 10.0)) {
            /* '<S2>:1:10' distsCalibed(nodeId) = 1.0029 * dists(nodeId) - 0.0829; */
            rtb_distsCalibed_idx_3 = 1.0029 * arg_dists[3] - 0.0829;
        } else {
            /* '<S2>:1:11' else */
            /* '<S2>:1:12' distsCalibed(nodeId) = 0.9976 * dists(nodeId) - 0.0511; */
            rtb_distsCalibed_idx_3 = 0.9976 * arg_dists[3] - 0.0511;
        }
    }
}
/* End of MATLAB Function: '<S1>/Calibrator' */

/* MATLAB Function: '<S1>/trilat' incorporates:
 *  Inport: '<Root>/ancs'
 */
/* MATLAB Function 'trilatCalib/trilat': '<S3>:1' */
/* '<S3>:1:2' a = ancs'; */
for (r1 = 0; r1 < 3; r1++) {
    trilatCalib_B.a[r1 << 2] = arg_ancs[r1];
    trilatCalib_B.a[1 + (r1 << 2)] = arg_ancs[r1 + 3];
    trilatCalib_B.a[2 + (r1 << 2)] = arg_ancs[r1 + 6];
    trilatCalib_B.a[3 + (r1 << 2)] = arg_ancs[r1 + 9];
}
/* '<S3>:1:3' A = 2*[... */
/* '<S3>:1:4'     a(2,:)-a(1,:);... */
/* '<S3>:1:5'     a(3,:)-a(2,:);... */
/* '<S3>:1:6'     a(4,:)-a(3,:)... */
/* '<S3>:1:7'     ]; */
trilatCalib_B.a_m[0] = trilatCalib_B.a[1] - trilatCalib_B.a[0];
trilatCalib_B.a_m[3] = trilatCalib_B.a[5] - trilatCalib_B.a[4];
trilatCalib_B.a_m[6] = trilatCalib_B.a[9] - trilatCalib_B.a[8];
trilatCalib_B.a_m[1] = trilatCalib_B.a[2] - trilatCalib_B.a[1];
trilatCalib_B.a_m[4] = trilatCalib_B.a[6] - trilatCalib_B.a[5];
trilatCalib_B.a_m[7] = trilatCalib_B.a[10] - trilatCalib_B.a[9];
trilatCalib_B.a_m[2] = trilatCalib_B.a[3] - trilatCalib_B.a[2];
trilatCalib_B.a_m[5] = trilatCalib_B.a[7] - trilatCalib_B.a[6];
trilatCalib_B.a_m[8] = trilatCalib_B.a[11] - trilatCalib_B.a[10];
for (r1 = 0; r1 < 3; r1++) {
    trilatCalib_B.A[3 * r1] = trilatCalib_B.a_m[3 * r1] * 2.0;
    trilatCalib_B.A[1 + 3 * r1] = trilatCalib_B.a_m[3 * r1 + 1] * 2.0;
    trilatCalib_B.A[2 + 3 * r1] = trilatCalib_B.a_m[3 * r1 + 2] * 2.0;
}
/* '<S3>:1:9' B = [... */
/* '<S3>:1:10'     d(1)^2 - d(2)^2 + a(2,:)*a(2,:)' - a(1,:)*a(1,:)';... */
/* '<S3>:1:11'     d(2)^2 - d(3)^2 + a(3,:)*a(3,:)' - a(2,:)*a(2,:)';... */
/* '<S3>:1:12'     d(3)^2 - d(4)^2 + a(4,:)*a(4,:)' - a(3,:)*a(3,:)'... */
/* '<S3>:1:13'     ]; */
trilatCalib_B.B[0] = (((trilatCalib_B.a[1] * trilatCalib_B.a[1] + trilatCalib_B.a[5] * trilatCalib_B.a[5]) + trilatCalib_B.a[9] * trilatCalib_B.a[9]) + (rtb_distsCalibed_idx_0 * rtb_distsCalibed_idx_0 - rtb_distsCalibed_idx_1 * rtb_distsCalibed_idx_1)) - ((trilatCalib_B.a[0] * trilatCalib_B.a[0] + trilatCalib_B.a[4] * trilatCalib_B.a[4]) + trilatCalib_B.a[8] * trilatCalib_B.a[8]);
trilatCalib_B.B[1] = (((trilatCalib_B.a[2] * trilatCalib_B.a[2] + trilatCalib_B.a[6] * trilatCalib_B.a[6]) + trilatCalib_B.a[10] * trilatCalib_B.a[10]) + (rtb_distsCalibed_idx_1 * rtb_distsCalibed_idx_1 - rtb_distsCalibed_idx_2 * rtb_distsCalibed_idx_2)) - ((trilatCalib_B.a[1] * trilatCalib_B.a[1] + trilatCalib_B.a[5] * trilatCalib_B.a[5]) + trilatCalib_B.a[9] * trilatCalib_B.a[9]);
trilatCalib_B.B[2] = (((trilatCalib_B.a[3] * trilatCalib_B.a[3] + trilatCalib_B.a[7] * trilatCalib_B.a[7]) + trilatCalib_B.a[11] * trilatCalib_B.a[11]) + (rtb_distsCalibed_idx_2 * rtb_distsCalibed_idx_2 - rtb_distsCalibed_idx_3 * rtb_distsCalibed_idx_3)) - ((trilatCalib_B.a[2] * trilatCalib_B.a[2] + trilatCalib_B.a[6] * trilatCalib_B.a[6]) + trilatCalib_B.a[10] * trilatCalib_B.a[10]);
/* '<S3>:1:14' p = A\B; */
r1 = 0;
r2 = 1;
r3 = 2;
rtb_distsCalibed_idx_0 = fabs(trilatCalib_B.A[0]);
rtb_distsCalibed_idx_1 = fabs(trilatCalib_B.A[1]);
if (rtb_distsCalibed_idx_1 > rtb_distsCalibed_idx_0) {
    rtb_distsCalibed_idx_0 = rtb_distsCalibed_idx_1;
    r1 = 1;
    r2 = 0;
}
if (fabs(trilatCalib_B.A[2]) > rtb_distsCalibed_idx_0) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
}
trilatCalib_B.A[r2] /= trilatCalib_B.A[r1];
trilatCalib_B.A[r3] /= trilatCalib_B.A[r1];
trilatCalib_B.A[3 + r2] -= trilatCalib_B.A[3 + r1] * trilatCalib_B.A[r2];
trilatCalib_B.A[3 + r3] -= trilatCalib_B.A[3 + r1] * trilatCalib_B.A[r3];
trilatCalib_B.A[6 + r2] -= trilatCalib_B.A[6 + r1] * trilatCalib_B.A[r2];
trilatCalib_B.A[6 + r3] -= trilatCalib_B.A[6 + r1] * trilatCalib_B.A[r3];
if (fabs(trilatCalib_B.A[3 + r3]) > fabs(trilatCalib_B.A[3 + r2])) {
    rtemp = r2;
    r2 = r3;
    r3 = rtemp;
}
trilatCalib_B.A[3 + r3] /= trilatCalib_B.A[3 + r2];
trilatCalib_B.A[6 + r3] -= trilatCalib_B.A[3 + r3] * trilatCalib_B.A[6 + r2];
rtb_distsCalibed_idx_0 = trilatCalib_B.B[r2] - trilatCalib_B.B[r1] * trilatCalib_B.A[r2];
rtb_distsCalibed_idx_1 = ((trilatCalib_B.B[r3] - trilatCalib_B.B[r1] * trilatCalib_B.A[r3]) - trilatCalib_B.A[3 + r3] * rtb_distsCalibed_idx_0) / trilatCalib_B.A[6 + r3];
rtb_distsCalibed_idx_0 -= trilatCalib_B.A[6 + r2] * rtb_distsCalibed_idx_1;
rtb_distsCalibed_idx_0 /= trilatCalib_B.A[3 + r2];
/* Outport: '<Root>/x_est' incorporates:
 *  MATLAB Function: '<S1>/trilat'
 */
arg_x_est[0] = ((trilatCalib_B.B[r1] - trilatCalib_B.A[6 + r1] * rtb_distsCalibed_idx_1) - trilatCalib_B.A[3 + r1] * rtb_distsCalibed_idx_0) / trilatCalib_B.A[r1];
arg_x_est[1] = rtb_distsCalibed_idx_0;
arg_x_est[2] = rtb_distsCalibed_idx_1;





        
  


          
      
      
        
  
  
    
    

      
        

    


    
    
  
  


      
      
              
        



      
        
  

    } 
    



  








  // Model initialize function
      void trilatCalibClass::initialize()
  {
            
      
      
        
    
    
    
        
    
    
        /* Registration code */
        
  
  



      
  
  

  

  





      


	/* initialize error status */
        rtmSetErrorStatus((&trilatCalib_M), (NULL));
    
      
      

        
  
  

  
  
     

  


    


  

    
      
        

  

  
      
          
    
    

    

    

      
    
    
        

  

      
        
  

  }








  

    
    // Model terminate function
        void trilatCalibClass::terminate()

    {
      
      /* (no terminate code required) */



          
  

    }
    



  
  
   


    // Constructor
     trilatCalibClass::trilatCalibClass()
    {
      
      


      
    }


      // Destructor
       trilatCalibClass::~trilatCalibClass()
      {
          /* Currently there is no destructor body generated.*/
      }
        // Real-Time Model get method
        RT_MODEL_trilatCalib_T * trilatCalibClass::getRTM() {
          return (&trilatCalib_M);
        }
      






// 
// File trailer for generated code.
// 
// [EOF]
// 

