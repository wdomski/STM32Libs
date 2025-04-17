/*
 * utils.h
 *
 *      Author: Wojciech Domski
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

#ifndef M_PI

#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962

#define M_PI_F		3.14159265358979323846f
#define M_PI_2_F		1.57079632679489661923f
#define M_PI_4_F		0.78539816339744830962f

#endif

int32_t abs_int32(int32_t val);

float abs_float(float val);

int32_t limit_int32(int32_t value, int32_t min, int32_t max);

float limit_float(float value, float min, float max);

int32_t map_int32(int32_t value, int32_t min, int32_t max, int32_t mapped_min,
		int32_t mapped_max);

float map_float(float value, float min, float max, float mapped_min,
		float mapped_max);

int32_t sign_int32(int32_t val);

float sign_float(float val);		

double rad2deg(double rad);

double deg2rad(double deg);

float rad2degf(float rad);

float deg2radf(float deg);

long double greatest_common_divider(long double a, long double b);

long double euler_function(long double p, long double q);

/**
 * \brief Efficient memory copy
 *
 * \param[in]  source 		input buffer
 * \param[out] destination	output buffer
 * \param[in]  length		number of bytes to copy
 */
void memoryncopy(uint32_t * source, uint32_t * destination, uint32_t length);

#define BIT_SET(x, y)		((x) | (1 << (y)))
#define BIT_RESET(x, y)		((x) & ~(1 << (y)))
#define BIT_TOGGLE(x, y)	((x) ^ (1 << (y)))

#define BIT_MASK_GET(x, mask, shift)				(((x) & ((mask) << (shift))) >> (shift))
#define BIT_MASK_SET(x, mask, shift, value)			do{																\
														(x) &= ~((mask) << (shift));								\
														(x) |= ((((value) & ((mask) << (shift)))) << (shift));		\
													} while(0);

#endif /* UTILS_H_ */
