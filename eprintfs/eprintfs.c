/*
 * eprintfs.c
 *
 *  Created on: 24 Mar 2022
 *      Author: Wojciech Domski
 */

#include <stdarg.h>
#include <assert.h>

#define NULL 0

#include "eprintfs.h"

const char *flags = "-+ #0";
const char *width = "0123456789*";

inline int eabs(int val)
{
	if (val < 0)
	{
		return -val;
	}
	return val;
}

inline float efabs(float val)
{
	if (val < 0.0f)
	{
		return -val;
	}
	return val;
}

int estrlen(const char *str)
{
	int len;
	assert(str != NULL);
	for (len = 0; *str != '\0'; ++str, ++len)
	{
	}
	return len;
}

int estrncpy(const char *str, char *out, unsigned int len)
{
	unsigned int i;
	assert(str != NULL);
	assert(out != NULL);
	for (i = 0; *str != '\0' && i < len; ++str, ++out, ++i)
	{
		*out = *str;
	}
	*out = '\0';
	return i;
}

char *eitoa(long int val, char *str, int base)
{
	char buffer[65];
	int i;
	unsigned long int uval;
	char *str_ret = str;
	assert(str != NULL);
	if (base == 10 && val < 0)
	{
		*str++ = '-';
		val = val * -1;
	}
	uval = val;
	for (i = 0; i < 65; ++i)
	{
		buffer[i] = uval % base;
		uval = uval / base;
		if (uval == 0)
		{
			break;
		}
	}
	for (; i >= 0; --i)
	{
		if (buffer[i] < 10)
		{
			*str++ = buffer[i] + '0';
		}
		else
		{
			*str++ = buffer[i] - 10 + 'a';
		}
	}
	*str = '\0';
	return str_ret;
}

int eatoi(char *str, int base, char **stop)
{
	int val = 0;
	int negative_val = 1;
	int digit;
	assert(str != NULL);
	assert((base >= 2 && base <= 10) || (base == 16));
	if (*str == '-')
	{
		negative_val = -1;
		++str;
	}
	else if (*str == '+')
	{
		++str;
	}
	for (int i = 0; *str != '\0'; ++i, ++str)
	{
		if (base != 16)
		{
			if (*str >= '0' && *str <= '9')
			{
				digit = *str - '0';
			}
			else
			{
				break;
			}
		}
		else
		{
			if (*str >= '0' && *str <= '9')
			{
				digit = *str - '0';
			}
			else if (*str >= 'a' && *str <= 'f')
			{
				digit = *str - 'a' + 10;
			}
			else if (*str >= 'A' && *str <= 'F')
			{
				digit = *str - 'A' + 10;
			}
			else
			{
				break;
			}
		}
		val = val * base + digit;
	}
	if (stop != NULL)
	{
		*stop = str;
	}
	return val * negative_val;
}

float eatof(char *str, char **stop)
{
	float val = 0.0f;
	int fraction;
	int small_negative_value = 0;

	assert(str != NULL);
	char *local_stop;
	val = eatoi(str, 10, &local_stop);
	if(val == 0 && str[0] == '-'){
		small_negative_value = 1;
	}
	if (*local_stop == '.')
	{
		str = local_stop + 1;
		fraction = eatoi(str, 10, &local_stop);
		int power_val = local_stop - str;
		int divider = epower(10, power_val);
		float part = (float)fraction / divider;
		if (val < 0.0f)
		{
			part *= -1.0f;
		}
		val += part;
	}
	if (small_negative_value)
	{
		val *= -1.0f;
	}
	if (stop != NULL)
	{
		*stop = local_stop;
	}
	return val;
}

int estrncmp(const char *str1, const char *str2, unsigned int num)
{
	assert(str1 != NULL);
	assert(str2 != NULL);
	char v;
	for (unsigned int i = 0; i < num; ++i)
	{
		v = *str1++ - *str2++;
		if (v != 0)
		{
			return v;
		}
	}
	return 0;
}

unsigned int epower(unsigned int base, unsigned int power)
{
	unsigned int ret = 1;
	for (unsigned int i = 0; i < power; ++i)
	{
		ret = ret * base;
	}
	return ret;
}

unsigned int edigits(int number)
{
	unsigned int ret = 1;
	if (number < 0)
	{
		number *= -1;
	}
	for (unsigned int i = 0; number > 9; ++i)
	{
		++ret;
		number /= 10;
	}
	return ret;
}

char ehex_to_char(char hex, char upper)
{
	hex &= 0x0F;
	if (hex >= 0 && hex <= 9)
	{
		return hex + '0';
	}
	else if (hex >= 10 && hex <= 15)
	{
		if (upper == 0)
		{
			return hex - 10 + 'a';
		}
		else
		{
			return hex - 10 + 'A';
		}
	}
	return ' ';
}

int parse_format(const char *format);

char *move_to_end(char *str, int *len)
{
	assert(str != NULL);
	assert(len != NULL);
	*len = 0;
	while (*str != '\0')
	{
		++*len;
		++str;
	}

	return str;
}

int esprintf(char *input_str, const char *format, ...)
{
	char *str = input_str;
	int len_format = estrlen(format);
	int len_str = 0;
	int local_len;
	char pad_with_zeros;
	char pad_character;
	int pad_numbers;
	char precision_number;
	char buffer[65];

	va_list vl;

	va_start(vl, format);

	for (int i = 0; i < len_format; ++i)
	{
		if (format[i] != '%')
		{
			*str++ = format[i];
			++len_str;
		}
		else
		{
			++i;
			if (i >= len_format)
			{
				break;
			}
			if (format[i] == '%')
			{
				*str++ = '%';
				++len_str;
				continue;
			}

			pad_with_zeros = 0;
			pad_character = '0';
			pad_numbers = 0;

			if (format[i] == '0')
			{
				pad_with_zeros = 1;
				++i;
				if (i >= len_format)
				{
					break;
				}
			}

			// get numbers
			for (; i < len_format;)
			{
				if (format[i] >= '0' && format[i] <= '9')
				{
					pad_numbers = pad_numbers * 10 + (format[i] - '0');
					++i;
					if (pad_with_zeros == 0)
					{
						pad_with_zeros = 1;
						pad_character = ' ';
					}
				}
				else if (format[i] == '*')
				{
					// TODO: number is equal to the preceding argument
				}
				else
				{
					break;
				}
			}

			precision_number = 8;
			if (format[i] == '.')
			{
				precision_number = 0;
				++i;
				if (i >= len_format)
				{
					break;
				}
				if (format[i] >= '0' && format[i] <= '9')
				{
					precision_number = (format[i] - '0');
					++i;
					if (i >= len_format)
					{
						break;
					}
				}
				else if (format[i] >= '*')
				{
					// TODO: number is equal to the preceding argument
				}
			}

			// TODO: process length
			if (format[i] == 'l' || format[i] == 'h')
			{
				++i;
				if (format[i] == 'l' || format[i] == 'h')
				{
					++i;
				}
			}

			if (format[i] == 'd' || format[i] == 'i')
			{
				int val = va_arg(vl, int);
				if (pad_with_zeros == 0)
				{
					eitoa(val, str, 10);
					str = move_to_end(str, &local_len);
					len_str += local_len;
				}
				else
				{
					int negative = 0;
					if (val < 0)
					{
						negative = 1;
					}
					eitoa(eabs(val), buffer, 10);
					local_len = estrlen(buffer);
					int number_of_zeros_to_add = pad_numbers - local_len;
					if (negative == 1)
					{
						*str++ = '-';
						++len_str;
					}
					if (number_of_zeros_to_add > 0)
					{
						for (int j = 0; j < number_of_zeros_to_add; ++j)
						{
							*str++ = pad_character;
						}
						len_str += number_of_zeros_to_add;
					}
					estrncpy(buffer, str, local_len);

					str = move_to_end(str, &local_len);
					len_str += local_len;
				}
			}

			if (format[i] == 'f')
			{
				float val = va_arg(vl, double);
				float fraction = efabs(val) - eabs(val);

				//compensate for negative value when integer part is 0
				if ((val < 0.0f) && ((long int)val == 0))
				{
					*(str++) = '-';
					++len_str;
				}
				eitoa(val, str, 10);
				str = move_to_end(str, &local_len);
				len_str += local_len;
				if (precision_number > 0)
				{
					*str++ = '.';
					++len_str;

					for (int j = 0; j < precision_number; ++j)
					{
						fraction = 10.0f * fraction;
						char digit = ((int)fraction % 10) + '0';
						*str++ = digit;
					}
					len_str += precision_number;
				}
			}

			if (format[i] == 'x' || format[i] == 'X')
			{
				char upper = 0;
				if (format[i] == 'X')
				{
					upper = 1;
				}
				int val = va_arg(vl, int);
				int non_zero = 0;
				char digit;
				for (int j = 28; j >= 0;)
				{
					digit = val >> j & 0x0F;
					if (digit & 0x0F || non_zero > 0)
					{
						*str++ = ehex_to_char(digit, upper);
						++len_str;
						non_zero = 1;
					}
					j = j - 4;
				}
				if (non_zero == 0)
				{
					*str++ = '0';
					++len_str;
				}
			}

			if (format[i] == 'c')
			{
				char val = va_arg(vl, int);
				*str++ = val;
				++len_str;
			}
		}
	}

	*str = '\0';

	va_end(vl);

	return len_str;
}

int esscanf(char *input_str, const char *format, ...)
{
	char *str = input_str;
	int len_format = estrlen(format);
	char parameters_number = 0;
	char *output;

	va_list vl;

	va_start(vl, format);

	for (int i = 0; (i < len_format) && (*str != '\0'); ++i)
	{
		if (format[i] != '%')
		{
			if (*str != format[i])
			{
				return parameters_number;
			}
			++str;
		}
		else
		{
			++i;
			if (format[i] == '%')
			{
				if (*str != format[i])
				{
					return parameters_number;
				}
				++str;
				continue;
			}

			// TODO: process length
			if (format[i] == 'l' || format[i] == 'h')
			{
				++i;
				if (format[i] == 'l' || format[i] == 'h')
				{
					++i;
				}
			}

			if (format[i] == 'd' || format[i] == 'i')
			{
				int *val = va_arg(vl, int *);
				*val = eatoi(str, 10, &output);
				str = output;
				++parameters_number;
			}

			if (format[i] == 'f')
			{
				float *val = va_arg(vl, float *);
				*val = eatof(str, &output);
				str = output;
				++parameters_number;
			}

			if (format[i] == 'x' || format[i] == 'X')
			{
				int *val = va_arg(vl, int *);
				*val = eatoi(str, 16, &output);
				str = output;
				++parameters_number;
			}

			if (format[i] == 'c')
			{
				char *val = va_arg(vl, char *);
				*val = *str;
				++str;
				++parameters_number;
			}
		}
	}

	va_end(vl);

	return parameters_number;
}

const char *estrchr(const char *str, int character)
{
	char ch = character & 0xFF;
	const char *ptr = NULL;
	for (;;)
	{
		if (*str == ch)
		{
			ptr = str;
			break;
		}
		if (*str == '\0')
		{
			break;
		}
		++str;
	}
	return ptr;
}

char *estrstr(char *str1, const char *str2)
{
	char *match = NULL;
	for (int i = 0;; ++i)
	{
		for (int j = 0;; ++j)
		{
			if (str2[j] == '\0')
			{
				return &str1[i];
			}

			if (str1[i + j] == '\0')
			{
				return NULL;
			}
			
			if (str1[i + j] != str2[j])
			{
				break;
			}
		}
	}

	return match;
}

char *efind_char_right(char *str, int len, const char sign, int * position){
	for(int i = len-1; i >= 0; --i){
		if(str[i] == sign){
			if(position){
				*position = i;
			}
			return &str[i];
		}
	}
	return NULL;
}
