/*
 * File: integerSineZ.c
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

#include "integerSineZ.h"
#include "integerSineZ_private.h"

/* Block states (auto storage) */
D_Work_integerSineZ integerSineZ_DWork;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_integerSineZ integerSineZ_Y;

/* Real-time model */
RT_MODEL_integerSineZ integerSineZ_M_;
RT_MODEL_integerSineZ *integerSineZ_M = &integerSineZ_M_;

/* Model step function */
void integerSineZ_step(void)
{
  /* local block i/o variables */
  int16_T rtb_sig_y;

  {
    int16_T rtb_sig_z1;

    /* UnitDelay: '<Root>/UnitDelay_1' */
    rtb_sig_z1 = integerSineZ_DWork.UnitDelay_1_DSTATE;

    /* Sum: '<Root>/Sum' incorporates:
     *  Gain: '<Root>/Gain'
     *  UnitDelay: '<Root>/UnitDelay_2'
     */
    rtb_sig_y = ((int16_T)((int32_T)32188 * (int32_T)
      integerSineZ_DWork.UnitDelay_1_DSTATE >> 15) << 1U) -
      integerSineZ_DWork.UnitDelay_2_DSTATE;

    /* Outport: '<Root>/y' */
    integerSineZ_Y.y = rtb_sig_y;

    /* Update for UnitDelay: '<Root>/UnitDelay_2' */
    integerSineZ_DWork.UnitDelay_2_DSTATE = rtb_sig_z1;

    /* Update for UnitDelay: '<Root>/UnitDelay_1' */
    integerSineZ_DWork.UnitDelay_1_DSTATE = rtb_sig_y;
  }
}

/* Model initialize function */
void integerSineZ_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(integerSineZ_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&integerSineZ_DWork, 0,
                sizeof(D_Work_integerSineZ));

  /* external outputs */
  integerSineZ_Y.y = 0;

  /* InitializeConditions for UnitDelay: '<Root>/UnitDelay_2' */
  integerSineZ_DWork.UnitDelay_2_DSTATE = -10856;

  /* InitializeConditions for UnitDelay: '<Root>/UnitDelay_1' */
  integerSineZ_DWork.UnitDelay_1_DSTATE = -5526;
}

/* Model terminate function */
void integerSineZ_terminate(void)
{
  /* (no terminate code required) */
}

/* File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
