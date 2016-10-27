// 
// File: ekf_mf.h
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



  #ifndef RTW_HEADER_ekf_mf_h_
  #define RTW_HEADER_ekf_mf_h_

#include "rtwtypes.h"

#include <math.h>

#include <string.h>

#include <stddef.h>
    #ifndef ekf_mf_COMMON_INCLUDES_
  # define ekf_mf_COMMON_INCLUDES_
    #include "rtwtypes.h"
  #endif /* ekf_mf_COMMON_INCLUDES_ */
  






    /* Macros for accessing real-time model data structure */
    

    #ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm) ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val) ((rtm)->errorStatus = (val))
#endif


  
   
    
      


          /* Forward declaration for rtModel */
      typedef struct tag_RTM_ekf_mf_T RT_MODEL_ekf_mf_T;





            /* Block signals (auto storage) */
                  
          
  
     typedef struct  {
  
            
                    
          
                real_T A[36]; 
                          
        

          
          
                    
          
                real_T P_pre[36]; /* '<S1>/EKF prediction' */
                          
        

          
          
                    
          
                real_T A_m[36]; 
                          
        

          
          
                    
          
                real_T A_c[36]; 
                          
        

          
          
                    
          
                real_T dv0[36]; 
                          
        

          
          
                    
          
                real_T orderedBuff_data[9]; 
                          
        

          
          
                    
          
                real_T tmp_data[9]; 
                          
        

          
          
                    
          
                real_T vwork_data[9]; 
                          
        

          
          
                    
          
                real_T dv1[8]; 
                          
        

          
          
                    
          
                real_T H[6]; 
                          
        

          
          
                    
          
                real_T K[6]; 
                          
        

          
          
                    
          
                real_T x_pre[6]; /* '<S1>/EKF prediction' */
                          
        

          
          
                    
          
                real_T rtb_P_pre_k[6]; 
                          
        

          
          
                    
          
                int8_T I[36]; 
                          
        

          
          
                    
          
                int32_T iidx_data[9]; 
                          
        

          
          
                    
          
                int32_T idx0_data[9]; 
                          
        

          
          
                    
          
                real_T distsCalibed[4]; /* '<S1>/Median Filter' */
                          
        

          
          
                    
          
                real_T dis_pre_vect_idx_0; 
                          
        

          
          
                    
          
                real_T dis_pre_vect_idx_1; 
                          
        

          
          
                    
          
                int32_T orderedBuff_sizes[2]; 
                          
        

          
          
                    
          
                int32_T tmp_sizes[2]; 
                          
        

          

  
    } B_ekf_mf_T;
  

        
              
          /* Block states (auto storage) for system '<Root>' */
              
        
  
     typedef struct  {
  
            
                    
          
                real_T UnitDelay1_DSTATE[6]; /* '<S1>/Unit Delay1' */
                          
        

          
          
                    
          
                real_T UnitDelay2_DSTATE[36]; /* '<S1>/Unit Delay2' */
                          
        

          
          
                    
          
                real_T P_pre_1[36]; /* '<S1>/P_pre_1' */
                          
        

          
          
                    
          
                real_T R; /* '<S1>/R' */
                          
        

          
          
                    
          
                real_T acc_xy; /* '<S1>/acc_xy' */
                          
        

          
          
                    
          
                real_T acc_z; /* '<S1>/acc_z' */
                          
        

          
          
                    
          
                real_T ancs[12]; /* '<S1>/ancs' */
                          
        

          
          
                    
          
                real_T initDists[36]; /* '<S1>/initDists' */
                          
        

          

  
    } DW_ekf_mf_T;
  

      
              
            /* Parameters (auto storage) */
      
        struct P_ekf_mf_T_ {
                      
             real_T P_0[36]; /* Variable: P_0
    * Referenced by:
*   '<S1>/P_pre_1'
*   '<S1>/Unit Delay2'
   */
            
             real_T ancs[12]; /* Variable: ancs
    * Referenced by: '<S1>/ancs'
   */
            
             real_T ekf_mf_x_hat0[6]; /* Mask Parameter: ekf_mf_x_hat0
    * Referenced by: '<S1>/Unit Delay1'
   */
            
             real_T R_InitialValue; /* Expression: 0.2
    * Referenced by: '<S1>/R'
   */
            
             real_T acc_xy_InitialValue; /* Expression: 5
    * Referenced by: '<S1>/acc_xy'
   */
            
             real_T acc_z_InitialValue; /* Expression: 2
    * Referenced by: '<S1>/acc_z'
   */
            
             real_T initDists_InitialValue[36]; /* Expression: [dists0 dists0 dists0 dists0 dists0 dists0 dists0 dists0 dists0]
    * Referenced by: '<S1>/initDists'
   */
  

        };
      
      
      
            /* Parameters (auto storage) */
       typedef struct P_ekf_mf_T_ P_ekf_mf_T;

        /* Real-time Model Data Structure */
      struct tag_RTM_ekf_mf_T {
        const char_T * volatile errorStatus;

    };

      
        




            #ifdef __cplusplus
        extern "C" {
        #endif
        #ifdef __cplusplus
        }
        #endif
    




    /* Class declaration for model ekf_mf */
    class ekf_mfClass {
      
      /* public data and function members */
      
      public:
      
            /* Tunable parameters */
      P_ekf_mf_T ekf_mf_P; 
/* Block signals */
B_ekf_mf_T ekf_mf_B;
/* Block states */
DW_ekf_mf_T ekf_mf_DW;

      
    /* Model entry point functions */

    /* model initialize function */
    void initialize();
    

            /* model step function */
            void step(real_T arg_dists[4], real_T arg_deltat, real_T arg_imu, uint8_T arg_nodeId, real_T arg_calibEnable, real_T arg_medBuffSize, real_T arg_x_est[6], real_T arg_distMf[4]);
    
      /* model terminate function */
      void terminate();
    
    
    
    
    
    /* Constructor */
    ekf_mfClass();
    
      /* Destructor */
      ~ekf_mfClass();


      
/* Real-Time Model get method */
RT_MODEL_ekf_mf_T * getRTM();

      

      
      /* private data and function members */
      
      private:
      
      /* Real-Time Model */RT_MODEL_ekf_mf_T ekf_mf_M;

              /* private member function(s) for subsystem '<Root>' */
                    
              void ekf_mf_eml_sort(const real_T x_data[], const int32_T x_sizes[2], real_T y_data[], int32_T y_sizes[2]);
      

        
  
  
  
  
  


      
    };
    
    





      
  /*-
   * The generated code includes comments that allow you to trace directly 
   * back to the appropriate location in the model.  The basic format
   * is <system>/block_name, where system is the system number (uniquely
   * assigned by Simulink) and block_name is the name of the block.
   *
   * Use the MATLAB hilite_system command to trace the generated code back
   * to the model.  For example,
   *
   * hilite_system('<S3>')    - opens system 3
   * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
   *
   * Here is the system hierarchy for this model
   *
    * '<Root>' : 'ekf_mf'
      * '<S1>'   : 'ekf_mf/ekf_mf'
      * '<S2>'   : 'ekf_mf/ekf_mf/EKF prediction'
      * '<S3>'   : 'ekf_mf/ekf_mf/EKF update'
      * '<S4>'   : 'ekf_mf/ekf_mf/Median Filter'
   */



  #endif /* RTW_HEADER_ekf_mf_h_ */

// 
// File trailer for generated code.
// 
// [EOF]
// 

