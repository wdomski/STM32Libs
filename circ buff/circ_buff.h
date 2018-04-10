/**
 * Author:    Wojciech Domski
 * Web:       domski.pl
 * Blog:      blog.domski.pl
 * Contact:   Wojciech.Domski@gmail.com
 */

#ifndef CIRC_BUFF_HEADER
#define CIRC_BUFF_HEADER

#include "circ_buff_hw.h"

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t * buffer;				//buffer; the same buffer as for DMA (if used)
	uint32_t buffer_length;				//buffer length in bytes
	uint32_t position;					//index for moving around on buffer
	uint32_t last_position;				//last position of index

	circ_buff_hw_t * hw;			//hardware structure

	uint32_t idle_time;					//used to get full frame
									//when time gap between transmission is grater
									//than this value allow read operation
									//use always with:
									//int circ_buff_new_data(circ_buff_t * circ_buff)
									//function
									//
									//if = 0 then feature is turned off

	//private fields
	uint32_t _idle_time_last_position;	//variable used to hold latest position for idle time
	uint32_t _idle_time_last_time;		//variable used to hold latest read time
} circ_buff_t;

/**
 * Initialization function for circular buffer.
 *
 * \param	circ_buff		Pointer to circular buffer.
 * \param	hw				Pointer to hardware abstraction for circular buffer.
 * \param	buffer			Pointer to buffer.
 * \param	buflen			Buffer length.
 * \param	idle			Idle time. A period of time expressed in ms to
 * 							wait before announcing that there is some data.
 * 							Useful for ongoing transmissions to read complete
 * 							message.
 * 							To disable pass 0, it will immediately
 * 							notice if there is new data.
 */
int circ_buff_init(circ_buff_t * circ_buff, circ_buff_hw_t * hw, void * buffer,
		uint32_t buflen, uint32_t idle);

/**
 * Returns relative position to DMA buffer.
 *
 * \param	circ_buff		Pointer to circular buffer.
 *
 * \return	Position
 */
int circ_buff_get_current_position(circ_buff_t * circ_buff);

/**
 * Checks if there is new data.
 * Implements idle time to prevent reading incomplete buffer
 * when there is ongoing transmission.
 *
 * \param	circ_buff		Pointer to circular buffer.
 *
 * \return	0				Old data.
 * 			1				New data.
 */
int circ_buff_new_data(circ_buff_t * circ_buff);

/**
 * Returns the number of bytes to be read.
 *
 * \param	circ_buff		Pointer to circular buffer.
 *
 * \return	0				No data to read.
 * 			>0				Returned data indicates the number
 * 							of bytes to be read.
 */
int circ_buff_new_data_amount(circ_buff_t * circ_buff);

/**
 *
 *
 * \param	circ_buff		Pointer to circular buffer.
 */
int circ_buff_get_position(circ_buff_t * circ_buff);

/**
 * Returns position.
 * Returns current index in buffer.
 *
 * \param	circ_buff		Pointer to circular buffer.
 *
 * \return					Current index in buffer.
 */
int circ_buff_ignore_new_data(circ_buff_t * circ_buff);

/**
 * Reads data to buffer.
 *
 * Reads data from temporary buffer which is held in
 * structure to output buffer.
 *
 * It ends output buffer with extra '\0' character.
 *
 * \param	circ_buff		Pointer to circular buffer.
 * \param	out				Output buffer.
 * \param	len				Read exactly len bytes.
 * 							If len == 0 then read as many bytes as
 * 							there are available.
 *
 * \return					Number of bytes read.
 */
int circ_buff_read(circ_buff_t * circ_buff, uint8_t * out, uint32_t len);

/**
 * Writes data to circular buffer.
 *
 * \param	circ_buff		Pointer to circular buffer.
 * \param	in				Input buffer.
 * \param	len				Number of bytes to read.
 *
 * \return	0				Everything is good.
 * \return	-1				Bad input buffer.
 * \return	-2				Input buffer is too long.
 */
int circ_buff_write(circ_buff_t * circ_buff, uint8_t * in, uint32_t len);

/**
 * Start receiving in DMA mode.
 *
 * \param	cb		Pointer to circular buffer structure
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_start_rx(circ_buff_t * cb);

/**
 * Start transmitting in DMA mode.
 *
 * \param	cb		Pointer to circular buffer structure
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_start_tx(circ_buff_t * cb);

/**
 * Start transmitting of external buffer in DMA mode.
 *
 * \param	cb		Pointer to circular buffer structure
 * \param	buff	Buffer
 * \param	size	Buffer size
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_start_tx_buff(circ_buff_t * cb, uint8_t * buff, uint32_t size);

#endif
