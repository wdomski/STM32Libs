/*
 * Author:  Wojciech Domski
 * Email:   Wojciech.Domski@gmail.com
 * Webpage: www.Domski.pl
 * Blog:    www.blog.Domski.pl
 *
 * Copyright 2018 Wojciech Domski
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

#define PID_PROPORTIONAL_OFF	0.0f
#define PID_INTEGRAL_OFF		0.0f
#define PID_DERIVATIVE_OFF		0.0f

typedef struct {
	int32_t p;
	int32_t i;
	int32_t d;

	int32_t p_val;
	int32_t i_val;
	int32_t d_val;

	int32_t p_max;
	int32_t i_max;
	int32_t d_max;

	int32_t p_min;
	int32_t i_min;
	int32_t d_min;

	uint8_t f;
	uint32_t power;

	int32_t dv;
	int32_t mv;

	int32_t e_last;
	int32_t sum;

	int32_t total_max;
	int32_t total_min;

	int32_t control;

	int32_t dt_ms;
} pid_i32_t;

typedef struct {
	float p;
	float i;
	float d;

	float p_val;
	float i_val;
	float d_val;

	float p_max;
	float i_max;
	float d_max;

	float p_min;
	float i_min;
	float d_min;

	float dv;
	float mv;

	float e_last;
	float sum;

	float total_max;
	float total_min;

	float control;

	float dt_s;
} pid_f32_t;

int pid_i32_init(pid_i32_t * pid, float p, float i, float d, uint8_t f, int32_t dt_ms);

int32_t pid_i32_calc(pid_i32_t * pid, int32_t mv, int32_t dv);

int32_t pid_i32_scale(pid_i32_t * pid, float v);

int pid_i32_reset_int(pid_i32_t * pid);

int pid_f32_init(pid_f32_t * pid, float p, float i, float d, float dt_s);

float pid_f32_calc(pid_f32_t * pid, float mv, float dv);

int pid_f32_reset_int(pid_f32_t * pid);

#endif /* PID_H_ */
