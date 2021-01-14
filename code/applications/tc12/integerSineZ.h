/*
 * File: integerSineZ.h
 *
 * Real-Time Workshop code generated for Simulink model integerSineZ.
 *
 * Model version                        : 1.12
 * Real-Time Workshop file version      : 7.3  (R2009a)  15-Jan-2009
 * Real-Time Workshop file generated on : Wed Jun 12 21:11:49 2013
 * TLC version                          : 7.3 (Jan 18 2009)
 * C/C++ source code generated on       : Wed Jun 12 21:11:49 2013
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_integerSineZ_h_
#define RTW_HEADER_integerSineZ_h_
#ifndef integerSineZ_COMMON_INCLUDES_
# define integerSineZ_COMMON_INCLUDES_
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* integerSineZ_COMMON_INCLUDES_ */

#include "integerSineZ_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  struct {
    void *LoggedData;
  } Scope_PWORK;                       /* '<Root>/Scope' */

  int16_T UnitDelay_2_DSTATE;          /* '<Root>/UnitDelay_2' */
  int16_T UnitDelay_1_DSTATE;          /* '<Root>/UnitDelay_1' */
} D_Work_integerSineZ;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  int16_T y;                           /* '<Root>/y' */
} ExternalOutputs_integerSineZ;

/* Real-time Model Data Structure */
struct RT_MODEL_integerSineZ {
  const char_T * volatile errorStatus;
};

/* Block states (auto storage) */
extern D_Work_integerSineZ integerSineZ_DWork;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_integerSineZ integerSineZ_Y;

/* Model entry point functions */
extern void integerSineZ_initialize(void);
extern void integerSineZ_step(void);
extern void integerSineZ_terminate(void);

/* Real-time Model object */
extern RT_MODEL_integerSineZ *integerSineZ_M;

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
 * '<Root>' : integerSineZ
 */
#endif                                 /* RTW_HEADER_integerSineZ_h_ */

/* File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
