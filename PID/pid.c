/*
 * Author:  Wojciech Domski
 * Email:   Wojciech.Domski@gmail.com
 * Webpage: www.Domski.pl
 * Blog:    www.blog.Domski.pl
 *
 * Copyright 2018 Wojciech Domski
 */

#include <stddef.h>
#include "pid.h"

int pid_i32_init(pid_i32_t * pid, float p, float i, float d, uint8_t f,
		int32_t dt_ms) {

	uint32_t k;

	if (pid == NULL) {
		return -1;
	}

	pid->power = 1;
	for (k = 0; k < f; ++k) {
		pid->power = pid->power * 2;
	}
	pid->f = f;

	pid->p = (int32_t) (p * pid->power);
	pid->i = (int32_t) (i * pid->power);
	pid->d = (int32_t) (d * pid->power);

	pid->p_val = 0;
	pid->i_val = 0;
	pid->d_val = 0;

	pid->p_max = INT32_MAX;
	pid->p_min = INT32_MIN;

	pid->i_max = INT32_MAX;
	pid->i_min = INT32_MIN;

	pid->d_max = INT32_MAX;
	pid->d_min = INT32_MIN;

	pid->e_last = 0;
	pid->sum = 0;

	pid->total_max = INT32_MAX;
	pid->total_min = INT32_MIN;

	pid->dt_ms = dt_ms;

	return 0;
}

int32_t pid_i32_calc(pid_i32_t * pid, int32_t mv, int32_t dv) {
	int32_t p, i, d, e, total;

	pid->mv = mv;
	pid->dv = dv;

	e = dv - mv;
	p = pid->p * e;
	if (p > pid->p_max)
		p = pid->p_max;
	else if (p < pid->p_min)
		p = pid->p_min;

	//p = p >> pid->f;
	pid->p_val = p >> pid->f;

	i = pid->sum;
	i += e * pid->i * pid->dt_ms / 1000;
	if (i > pid->i_max)
		i = pid->i_max;
	else if (i < pid->i_min)
		i = pid->i_min;
	pid->sum = i;

	//i = i >> pid->f;
	pid->i_val = i >> pid->f;

	d = (pid->d * (e - pid->e_last) * 1000) / pid->dt_ms;
	if (d > pid->d_max)
		d = pid->d_max;
	else if (d < pid->d_min)
		d = pid->d_min;

	//d = d >> pid->f;
	pid->d_val = d >> pid->f;

	total = p + i + d;
	//total += pid->control;
	if (total > pid->total_max)
		total = pid->total_max;
	else if (total < pid->total_min)
		total = pid->total_min;
	pid->control = total >> pid->f;
	pid->e_last = e;

	return pid->control;
}

int32_t pid_i32_scale(pid_i32_t * pid, float v) {
	return v * pid->power;
}

int pid_i32_reset_int(pid_i32_t * pid) {
	if (pid == NULL) {
		return -1;
	}

	pid->sum = 0;

	return 0;
}

int pid_f32_init(pid_f32_t * pid, float p, float i, float d, float dt_s) {

	if (pid == NULL) {
		return -1;
	}

	pid->p = p;
	pid->i = i;
	pid->d = d;

	pid->p_val = 0.0f;
	pid->i_val = 0.0f;
	pid->d_val = 0.0f;

	pid->p_max = INT32_MAX;
	pid->p_min = INT32_MIN;

	pid->i_max = INT32_MAX;
	pid->i_min = INT32_MIN;

	pid->d_max = INT32_MAX;
	pid->d_min = INT32_MIN;

	pid->e_last = 0.0f;
	pid->sum = 0.0f;

	pid->total_max = INT32_MAX;
	pid->total_min = INT32_MIN;

	pid->dt_s = dt_s;

	return 0;
}

float pid_f32_calc(pid_f32_t * pid, float mv, float dv) {
	float p, i, d, e, total;

	pid->mv = mv;
	pid->dv = dv;

	e = dv - mv;
	p = pid->p * e;
	if (p > pid->p_max)
		p = pid->p_max;
	else if (p < pid->p_min)
		p = pid->p_min;

	i = pid->sum;
	i += e * pid->i * pid->dt_s;
	if (i > pid->i_max)
		i = pid->i_max;
	else if (i < pid->i_min)
		i = pid->i_min;
	pid->sum = i;

	d = (pid->d * (e - pid->e_last)) / pid->dt_s;
	if (d > pid->d_max)
		d = pid->d_max;
	else if (d < pid->d_min)
		d = pid->d_min;

	total = p + i + d;
	//total += pid->control;
	if (total > pid->total_max)
		total = pid->total_max;
	else if (total < pid->total_min)
		total = pid->total_min;
	pid->control = total;
	pid->e_last = e;

	return pid->control;
}

int pid_f32_reset_int(pid_f32_t * pid) {
	if (pid == NULL) {
		return -1;
	}

	pid->sum = 0.0f;

	return 0;
}
