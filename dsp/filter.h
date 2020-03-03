/*
 * filter.h
 *
 *  Created on: 05/07/2018
 *      Author: Sergio Alejandro González
 *
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "mydefs.h"

#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4
#include "arm_math.h"
#endif /* ARM_MATH_CM4 */

// IIR coefficients definitions
// stable IIR coefficients for
// 30kHz sampling rate
//#define A_IIR	(0.995F)
//#define B_IIR	(0.005F)
//#define A_IIR	(0.999f)
//#define B_IIR	(0.001f)
#define A_IIR	(0.99999f)
#define B_IIR	(0.00001f)

// FIR coefficients definitions
// for 30kHz sampling rate
#define A_3sFIR		(2.0F/3.0F)
#define B_3sFIR		(1.0F/3.0F)

float32_t arm_Average_f32(uint32_t arr[], uint32_t size);
float32_t arm_3SamplesAverage_f32 (float32_t currentSample,
		float32_t * prvSamples,  uint32_t * idx);
//float32_t arm_3s_MovingAverage_f32 (float32_t currentSample,
//		float32_t * prvSamples,  uint32_t * idx);
float32_t arm_2SamplesIIR_f32 (float32_t currentSample,
		float32_t * prvSamples);

void promedio_system_vars(float32_t X11[],
		float32_t X22[], float32_t X33[], float32_t *out,
		uint32_t N_muestras);

//void promedio_system_vars(float X11[], float X22[], float X33[], float *out);

#endif /* FILTER_H_ */
