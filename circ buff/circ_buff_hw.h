#ifndef CIRC_BUFF_HW_HEADER
#define CIRC_BUFF_HW_HEADER

#include <stdint.h>

#if defined(__weak)
#else
#define __weak __attribute__((weak))
#endif

typedef struct {
	void * comm;			// communication structure for peripheral
	void * dma;				// dma structure
	void * tim;				// timer to get current ticks
	uint32_t timer_max_value; // max value for timer in ticks

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
 */
int circ_buff_hw_init(circ_buff_hw_t * circ_buff_hw, void * dma, void * comm);

/**
 * Set timer
 *
 * \param	circ_buff_hw		Hardware structure.
 * \param	tim 				Pointer to a timer
 */
void circ_buff_hw_set_timer(circ_buff_hw_t * circ_buff_hw, void * tim);

/**
 * Return current position from DMA
 *
 * \param	circ_buff_hw		Hardware structure
 * \param	buffer_beginning	Buffer beginning
 * \param	buffer_length		Buffer length
 *
 * \return	> 0					Position
 * \return	0					Position or hardware structure not initialized
 */
unsigned int circ_buff_hw_get_current_dma_position(circ_buff_hw_t * circ_buff_hw, void * buffer_beginning, uint32_t buffer_length);

/**
 * Returns current time in unit scaled with hw->ticks_us.
 * By default the factor is set to 1.
 *
 * \param	circ_buff_hw		Hardware structure
 *
 * \return	>=0					Time.
 */
unsigned int circ_buff_hw_get_time(circ_buff_hw_t * hw);

/**
 * Returns current time in unit scaled with hw->ticks_us.
 * By default the factor is set to 1.
 *
 * \param	circ_buff_hw		Hardware structure
 *
 * \return	>=0					Time.
 */
uint32_t circ_buff_hw_get_timer_max_value(circ_buff_hw_t * hw);

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
 * Stop receiving in DMA mode.
 *
 * \param	hw		Hardware structure
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_hw_stop_rx(circ_buff_hw_t * hw);

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
 * Stop transmitting in DMA mode.
 *
 * \param	hw		Hardware structure
 *
 * \return	0		Success
 * \return	!0		Something went wrong.
 */
int circ_buff_hw_stop_tx(circ_buff_hw_t * hw);

/**
 * Interrupt handler for busy tx.
 * It needs to be placed in interrupt handler of
 * transmitter complete signal.
 *
 * \param	hw		Hardware structure
 * \param	comm	Pointer to communication interface, e.g. UART
 */
void circ_buff_hw_isr_tx_completed(circ_buff_hw_t * hw, void * comm);

/**
 * Returns busy state of transmitter
 *
 * \param	hw		Hardware structure
 *
 * \return	0		Ongoing transmission
 * 			1		Busy
 */
uint8_t circ_buff_hw_busy_tx(circ_buff_hw_t * hw);

/**
 * @brief Resets number of transfers for DMA. 
 * Only valid for RP2040 implementation.
 * 
 * RP2040 DMA has a number of transfers register. It is 4-byte long 
 * and this function puts maximum value into the register. 
 * After reaching zero DMA will stop. This function can reset this counter.
 * The initial number of transfers is 2^32-1, thus giving ~4GB of data.
 * 
 * @param hw 	Hardware structure
 */
void circ_buff_hw_reset_transfer_count(circ_buff_hw_t *hw);

#endif
