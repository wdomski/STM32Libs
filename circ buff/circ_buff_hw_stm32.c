#include "circ_buff_hw.h"

#include "dma.h"

__weak unsigned int circ_buff_hw_get_current_dma_position(circ_buff_hw_t *circ_buff_hw, void * buffer_beginning, uint32_t buffer_length) {
	if (circ_buff_hw->dma){
		return buffer_length - __HAL_DMA_GET_COUNTER((DMA_HandleTypeDef* ) (circ_buff_hw->dma));
	}
	return 0;
}

__weak unsigned int circ_buff_hw_get_time(circ_buff_hw_t *hw) {
	if (hw == NULL) {
		return 0;
	}
	if (hw->tim == NULL) {
		return HAL_GetTick();
	}

	return __HAL_TIM_GET_COUNTER((TIM_HandleTypeDef *)(hw->tim));
}

__weak uint32_t circ_buff_hw_get_timer_max_value(circ_buff_hw_t * hw){
	return hw->timer_max_value;
}

__weak int circ_buff_hw_start_rx(circ_buff_hw_t *hw, uint8_t *buff,
		uint32_t size) {
	return HAL_UART_Receive_DMA(hw->comm, buff, size);
}

__weak int circ_buff_hw_stop_rx(circ_buff_hw_t *hw) {
	return HAL_UART_AbortReceive(hw->comm);
}

__weak int circ_buff_hw_start_tx(circ_buff_hw_t *hw, uint8_t *buff,
		uint32_t size) {
	hw->_busy_tx = 1;
	return HAL_UART_Transmit_DMA(hw->comm, buff, size);
}

__weak int circ_buff_hw_stop_tx(circ_buff_hw_t *hw) {
	hw->_busy_tx = 0;
	return HAL_UART_AbortTransmit(hw->comm);
}

__weak void circ_buff_hw_isr_tx_completed(circ_buff_hw_t *hw, void *comm)
{
	hw->_busy_tx = 0;
}