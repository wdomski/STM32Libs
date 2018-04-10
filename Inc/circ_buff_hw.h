/**
 * Author:    Wojciech Domski
 * Web:       domski.pl
 * Blog:      blog.domski.pl
 * Contact:   Wojciech.Domski@gmail.com
 */

#ifndef CIRC_BUFF_HW_HEADER
#define CIRC_BUFF_HW_HEADER

#include <stdint.h>

typedef struct {
	void * comm;			//communication structure for peripheral
	void * dma;				//dma structure

	volatile uint8_t _busy_tx;	//used for checking if there is ongoing tx transmission
} circ_buff_hw_t;

/**
 * Initialization function for hardware circular buffer
 *
 * \param	circ_buff_hw		Hardware structure.
 * \param	dma					Pointer to dma structure.
 * \param	comm				Pointer to communication interface.
 *
 * \return	0					Everything ok.
 * \return	-1					Bad dma structure.
 * \return	-2					Bad comm structure.
 */
int circ_buff_hw_init(circ_buff_hw_t * circ_buff_hw, void * dma, void * comm);

/**
 * Return current position from DMA
 *
 * \param	circ_buff_hw		Hardware structure
 *
 * \return	>=0					Position
 * \return	-1					Hardware structure not initialized
 */
int circ_buff_hw_get_current_dma_position(circ_buff_hw_t * circ_buff_hw);

/**
 * Returns current time in ms
 *
 * \return	>=0					Time expressed in ms.
 */
int circ_buff_hw_get_time(void);

/**
 * Start receiving in DMA mode.
 *
 * \param	hw		Hardware structure
 * \param	buff	Buffer
 * \param	size	Buffer size
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_hw_start_rx(circ_buff_hw_t * hw, uint8_t * buff, uint32_t size);

/**
 * Start transmitting in DMA mode.
 *
 * \param	hw		Hardware structure
 * \param	buff	Buffer
 * \param	size	Buffer size
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_hw_start_tx(circ_buff_hw_t * hw, uint8_t * buff, uint32_t size);

/**
 * Interrupt handler for busy tx.
 * It needs to be placed in interrupt handler of
 * transmitter complete signal.
 *
 * \param	hw		Hardware structure
 */
void circ_buff_hw_isr_tx_completed(circ_buff_hw_t * hw, void * tim);

/**
 * Returns busy state of transmitter
 *
 * \param	hw		Hardware structure
 *
 * \return	0		Ongoing transmission
 * 			1		Busy
 */
uint8_t circ_buff_hw_busy_tx(circ_buff_hw_t * hw);

#endif
