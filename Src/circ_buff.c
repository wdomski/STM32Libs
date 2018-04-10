/**
 * Author:    Wojciech Domski
 * Web:       domski.pl
 * Blog:      blog.domski.pl
 * Contact:   Wojciech.Domski@gmail.com
 */

#include "circ_buff.h"
#include "circ_buff_hw.h"

int circ_buff_init(circ_buff_t * circ_buff, circ_buff_hw_t * hw, void * buffer,
		uint32_t buflen, uint32_t idle) {
	circ_buff->last_position = 0;
	circ_buff->position = 0;
	circ_buff->_idle_time_last_position = 0;
	circ_buff->_idle_time_last_time = circ_buff_hw_get_time();
	//bad hw structure
	if (hw == NULL)
		return -1;

	circ_buff->hw = hw;
	//bad buffer
	if (buffer == NULL)
		return -2;

	circ_buff->buffer = (uint8_t *) buffer;
	circ_buff->buffer_length = buflen;
	circ_buff->idle_time = idle;
	return 0;
}

int circ_buff_get_current_position(circ_buff_t * circ_buff) {
	if (circ_buff->hw->dma) {
		circ_buff->position = circ_buff->buffer_length
				- circ_buff_hw_get_current_dma_position(circ_buff->hw);
	}

	return circ_buff->position;
}

int circ_buff_new_data(circ_buff_t * circ_buff) {
	circ_buff_get_current_position(circ_buff);

	//idle time implementation
	if (circ_buff->idle_time > 0) {
		if (circ_buff->_idle_time_last_position != circ_buff->position) {
			circ_buff->_idle_time_last_position = circ_buff->position;
			//update time
			circ_buff->_idle_time_last_time = circ_buff_hw_get_time();

			return 0;
		} else {
			if (circ_buff_hw_get_time() - circ_buff->_idle_time_last_time
					>= circ_buff->idle_time) {
				if (circ_buff->position != circ_buff->last_position) {
					return 1;
				}
			}
			return 0;
		}
	}

	if (circ_buff->position != circ_buff->last_position)
		return 1;

	return 0;
}

int circ_buff_new_data_amount(circ_buff_t * circ_buff) {
	int new_data;
	int size;
	size = 0;

	new_data = circ_buff_new_data(circ_buff);
	if (new_data) {
		if (circ_buff->position > circ_buff->last_position) {
			size = circ_buff->position - circ_buff->last_position;
		} else {
			size = circ_buff->buffer_length - circ_buff->last_position;
			size += circ_buff->position;
		}
	}

	return size;
}

int circ_buff_get_position(circ_buff_t * circ_buff) {
	return circ_buff->position;
}

int circ_buff_ignore_new_data(circ_buff_t * circ_buff) {
	circ_buff_get_current_position(circ_buff);
	circ_buff->last_position = circ_buff->position;
	return 0;
}

int circ_buff_read(circ_buff_t * circ_buff, uint8_t * out, uint32_t len) {
	int i;
	int size;
	int offset = 0;

	if (len == 0)
		len = UINT32_MAX;

	//TODO conduct tests to verify if limited length works
	//it can be verified quickly on small dma buffer
	if (circ_buff->position > circ_buff->last_position) {
		for (i = 0;
				(i < circ_buff->position - circ_buff->last_position)
						&& (i < len); ++i)
			out[i] = circ_buff->buffer[circ_buff->last_position + i];

		size = circ_buff->position - circ_buff->last_position;
	} else {
		for (i = 0;
				(i < circ_buff->buffer_length - circ_buff->last_position)
						&& (i < len); ++i)
			out[i] = circ_buff->buffer[circ_buff->last_position + i];
		offset = i;
		for (i = 0; (i < circ_buff->position) && (offset + i < len); ++i)
			out[offset + i] = circ_buff->buffer[i];
		size = circ_buff->buffer_length - circ_buff->last_position
				+ circ_buff->position;
	}
	//increment only as much as read
	circ_buff->last_position = circ_buff->last_position + size;
	//in case the buffer was winded back to the beginning
	circ_buff->last_position = circ_buff->last_position
			% circ_buff->buffer_length;
	//add \0 at the end
	out[offset + i] = '\0';

	return size;
}

int circ_buff_write(circ_buff_t * circ_buff, uint8_t * in, uint32_t len) {
	uint32_t i;

	if (in == NULL)
		return -1;

	if (len > circ_buff->buffer_length)
		return -2;

	for (i = 0; i < len; ++i) {
		circ_buff->buffer[circ_buff->position] = in[i];

		++circ_buff->position;
		if (circ_buff->position == circ_buff->buffer_length)
			circ_buff->position = 0;
	}

	return 0;
}

int circ_buff_start_rx(circ_buff_t * cb) {
	return circ_buff_hw_start_rx(cb->hw, cb->buffer, cb->buffer_length);
}

int circ_buff_start_tx(circ_buff_t * cb) {
	return circ_buff_hw_start_tx(cb->hw, cb->buffer, cb->buffer_length);
}

int circ_buff_start_tx_buff(circ_buff_t * cb, uint8_t * buff, uint32_t size) {
	return circ_buff_hw_start_tx(cb->hw, buff, size);
}

