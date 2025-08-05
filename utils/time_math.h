/*
 * time_math.h
 *
 * Author: Wojciech Domski <wojciech.domski@gmail.com>
 */

#ifndef TIME_MATH_H_
#define TIME_MATH_H_

#include <stdint.h>

uint32_t time_round_diff(uint32_t new_time, uint32_t last_time,
		uint32_t max_value);

uint32_t time_round_diff_update(uint32_t new_time, uint32_t *last_time,
		uint32_t max_value);

#endif /* TIME_MATH_H_ */
