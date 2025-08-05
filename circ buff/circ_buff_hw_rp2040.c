#include "stddef.h"
#include "circ_buff_hw.h"

#include "hardware/timer.h"
#include "hardware/dma.h"

__weak unsigned int circ_buff_hw_get_current_dma_position(circ_buff_hw_t *circ_buff_hw, void * buffer_beginning, uint32_t buffer_length) {
	return (uint32_t)dma_channel_hw_addr((int)circ_buff_hw->dma)->write_addr - (uint32_t)buffer_beginning;
}

__weak unsigned int circ_buff_hw_get_time(circ_buff_hw_t *hw) {
	return time_us_32();
}

__weak uint32_t circ_buff_hw_get_timer_max_value(circ_buff_hw_t * hw){
	return hw->timer_max_value;
}

__weak int circ_buff_hw_start_rx(circ_buff_hw_t *hw, uint8_t *buff,
		uint32_t size) {
	dma_channel_start((uint)hw->dma);
	return 0;
}

__weak int circ_buff_hw_stop_rx(circ_buff_hw_t *hw) {
	dma_channel_abort((uint)hw->dma);
	return 0;
}

__weak int circ_buff_hw_start_tx(circ_buff_hw_t *hw, uint8_t *buff,
		uint32_t size) {
	hw->_busy_tx = 1;
	dma_channel_set_read_addr((uint)hw->dma, buff, false);
    dma_channel_set_trans_count((uint)hw->dma, size, false);    
	dma_channel_start((uint)hw->dma);
	return 0;
}

__weak int circ_buff_hw_stop_tx(circ_buff_hw_t *hw) {
	hw->_busy_tx = 0;
	dma_channel_abort((uint)hw->dma);
	return 0;
}

__weak void circ_buff_hw_isr_tx_completed(circ_buff_hw_t *hw, void *comm)
{
	dma_hw->ints0 = 1u << (int)hw->dma;
	hw->_busy_tx = 0;
}

__weak void circ_buff_hw_reset_transfer_count(circ_buff_hw_t *hw)
{
	dma_channel_set_trans_count((uint)hw->dma, UINT32_MAX, false);
}