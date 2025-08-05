#include "time_math.h"

uint32_t time_round_diff(uint32_t new_time, uint32_t last_time,
		uint32_t max_value) {
	uint32_t diff;
	if (new_time >= last_time) {
		diff = new_time - last_time;
	} else {
		diff = max_value - last_time + new_time;
	}
	return diff;
}

uint32_t time_round_diff_update(uint32_t new_time, uint32_t *last_time,
		uint32_t max_value) {
	uint32_t diff;
	if (new_time >= *last_time) {
		diff = new_time - *last_time;
	} else {
		diff = max_value - *last_time + new_time;
	}
	*last_time = new_time;
	return diff;
}
