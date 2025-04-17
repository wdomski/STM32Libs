#include "utils.h"

int32_t abs_int32(int32_t val) {
	if (val >= 0) {
		return val;
	}
	return -val;
}

float abs_float(float val) {
	if (val >= 0.0f) {
		return val;
	}
	return -val;
}

int32_t limit_int32(int32_t value, int32_t min, int32_t max) {
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

float limit_float(float value, float min, float max) {
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

int32_t map_int32(int32_t value, int32_t min, int32_t max, int32_t mapped_min,
		int32_t mapped_max) {
	int32_t range = max - min;
	int32_t mapped_range = mapped_max - mapped_min;
	int32_t mapped_value = (value - min) * mapped_range / range;
	return mapped_value + mapped_min;
}

float map_float(float value, float min, float max, float mapped_min,
		float mapped_max) {
	float range = max - min;
	float mapped_range = mapped_max - mapped_min;
	float mapped_value = (value - min) * mapped_range / range;
	return mapped_value + mapped_min;
}

int32_t sign_int32(int32_t val) {
	if (val > 0) {
		return 1;
	} else if (val < 0) {
		return -1;
	}
	return 0;
}

float sign_float(float val) {
	if (val > 0.0f) {
		return 1.0f;
	} else if (val < 0.0f) {
		return -1.0f;
	}
	return 0.0f;
}

double rad2deg(double rad) {
	return rad * 180.0 / M_PI;
}

double deg2rad(double deg) {
	return deg * M_PI / 180.0;
}

float rad2degf(float rad) {
	return rad * 180.0 / M_PI_F;
}

float deg2radf(float deg) {
	return deg * M_PI_F / 180.0;
}

long double greatest_common_divider(long double a, long double b)
{
	while (a != b)
	{
		if (a > b)
		{
			a = a - b;
		}
		else
		{
			b = b - a;
		}
	}
	return a;
}

long double euler_function(long double p, long double q)
{
	return (p - 1.0) * (q - 1.0);
}

void memoryncopy(uint32_t * source, uint32_t * destination, uint32_t length){
	uint32_t quotient = length >> 2;
	uint8_t reminder = length & 0x03;
	for(uint32_t i = 0; i < quotient; ++i)
	{
		*destination = *source;
		++destination;
		++source;
	}
	for(uint8_t i = 0; i < reminder; ++i){
		*((uint8_t*)destination+i) = *((uint8_t*)source+i);
	}
}
