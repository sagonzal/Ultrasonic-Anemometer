/*
 * filter.c
 *
 *  Created on: 05/07/2018
 *      Author: Sergio Alejandro González
 */
#include "filter.h"


///!
/// @brief
///
__ramfunc float32_t arm_Average_f32(uint32_t arr[], uint32_t size)
{
	uint32_t i;
	float32_t avg;
	float32_t sum = 0;

	for (i = 0; i < size; ++i) {
		sum += arr[i];
	}

	avg = sum / size;

	return avg;
}

///!
/// @brief
///
__ramfunc float32_t arm_3SamplesAverage_f32 (float32_t currentSample, float32_t * prvSamples,  uint32_t * idx)
{
    static float32_t avg = 0.0f;

    if (*idx == 0)
    	avg = A_3sFIR*currentSample+B_3sFIR*prvSamples[*idx+2]+B_3sFIR*prvSamples[*idx+1]-B_3sFIR*prvSamples[*idx];
    else if (*idx == 1)
    	avg = A_3sFIR*currentSample+B_3sFIR*prvSamples[*idx-1]+B_3sFIR*prvSamples[*idx+1]-B_3sFIR*prvSamples[*idx];
    else if (*idx == 2)
    	avg = A_3sFIR*currentSample+B_3sFIR*prvSamples[*idx-1]+B_3sFIR*prvSamples[*idx-2]-B_3sFIR*prvSamples[*idx];

    prvSamples[*idx] = currentSample;
    *idx=*idx+1;
    if (*idx == 3)
        *idx = 0;

    return avg;
}

///!
/// @brief
///
inline float32_t arm_2SamplesIIR_f32 (float32_t currentSample, float32_t * prvSamples)
{
	static float32_t avg = 0.0f;

	avg = B_IIR*currentSample + A_IIR*( * prvSamples);
	*prvSamples = avg;

	return avg;
}

///!
/// @brief
///
void promedio_system_vars(float32_t X11[], float32_t X22[], float32_t X33[], float32_t *out, uint32_t N_muestras)
{
	static float32_t promedio_1 = 0.0f;
	static float32_t promedio_2 = 0.0f;
	static float32_t promedio_3 = 0.0f;
	static uint16_t i = 0;

    for (i = 0; i < N_muestras; i++) {
        promedio_1 += X11[i];
        promedio_2 += X22[i];
        promedio_3 += X33[i];
    }

    out[0] = promedio_1 / N_muestras;
    out[1] = promedio_2 / N_muestras;
    out[2] = promedio_3 / N_muestras;

}
