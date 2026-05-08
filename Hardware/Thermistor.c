#include "stm32f10x.h"
#include "AD.h"
#include <math.h>

#define FILTER_SIZE 32  // sliding average window size

void Thermistor_Init(void)
{
	AD_Init();
}

/* Convert a raw ADC reading to Celsius using the B-parameter equation */
static float ADC_to_Celsius(uint16_t adcVal)
{
	if (adcVal >= 4095) return -99.0f;
	if (adcVal == 0)    return  99.0f;

	float R_NTC   = 10000.0f * (float)adcVal / (4095.0f - (float)adcVal);
	float T0      = 298.15f;
	float R0      = 10000.0f;
	float B       = 3435.0f;
	float T_kelvin = 1.0f / (1.0f / T0 + logf(R_NTC / R0) / B);

	return T_kelvin - 273.15f;
}

/* Return a smoothed temperature using a sliding average over FILTER_SIZE samples */
float Get_Temperature(void)
{
	static float buf[FILTER_SIZE] = {0};
	static uint8_t idx = 0;
	static uint8_t filled = 0;

	buf[idx] = ADC_to_Celsius(AD_GetValue());
	idx = (idx + 1) % FILTER_SIZE;
	if (filled < FILTER_SIZE) filled++;

	float sum = 0.0f;
	uint8_t i;
	for (i = 0; i < filled; i++) sum += buf[i];

	return sum / filled;
}
