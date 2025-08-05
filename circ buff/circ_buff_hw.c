#include "stddef.h"
#include "circ_buff_hw.h"

int circ_buff_hw_init(circ_buff_hw_t *circ_buff_hw, void *dma, void *comm)
{
	circ_buff_hw->comm = comm;
	circ_buff_hw->dma = dma;
	circ_buff_hw->tim = NULL;
	circ_buff_hw->_busy_tx = 0;
	circ_buff_hw->timer_max_value = 0xFFFFFFFF;

	return 0;
}

void circ_buff_hw_set_timer(circ_buff_hw_t *circ_buff_hw, void *tim)
{
	circ_buff_hw->tim = tim;
}

uint8_t circ_buff_hw_busy_tx(circ_buff_hw_t *hw)
{
	return hw->_busy_tx;
}
