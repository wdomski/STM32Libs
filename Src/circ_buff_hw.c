/**
 * Author:    Wojciech Domski
 * Web:       domski.pl
 * Blog:      blog.domski.pl
 * Contact:   Wojciech.Domski@gmail.com
 */

#include "circ_buff_hw.h"
#include "dma.h"

int circ_buff_hw_init(circ_buff_hw_t * circ_buff_hw, void * dma, void * comm) {

	if (dma == NULL)
		return -1;

	if (comm == NULL)
		return -2;

	circ_buff_hw->comm = comm;
	circ_buff_hw->dma = dma;
	circ_buff_hw->_busy_tx = 0;

	return 0;
}

int circ_buff_hw_get_current_dma_position(circ_buff_hw_t * circ_buff_hw) {
	if (circ_buff_hw->dma)
		return __HAL_DMA_GET_COUNTER((DMA_HandleTypeDef * ) (circ_buff_hw->dma));

	return -1;
}

int circ_buff_hw_get_time(void) {
	return HAL_GetTick();
}

int circ_buff_hw_start_rx(circ_buff_hw_t * hw, uint8_t * buff, uint32_t size) {
	return HAL_UART_Receive_DMA(hw->comm, buff, size);
}

int circ_buff_hw_start_tx(circ_buff_hw_t * hw, uint8_t * buff, uint32_t size) {
	hw->_busy_tx = 1;
	return HAL_UART_Transmit_DMA(hw->comm, buff, size);
}

void circ_buff_hw_isr_tx_completed(circ_buff_hw_t * hw, void * tim) {
	if (hw != NULL) {
		if (hw->comm == tim) {
			hw->_busy_tx = 0;
		}
	}
}

uint8_t circ_buff_hw_busy_tx(circ_buff_hw_t * hw) {
	if (hw != NULL)
		return hw->_busy_tx;
	return 0;
}
