#include "fifo/fifo_hw_rp2040.h"

#include "pico.h"
#include "hardware/sync.h"

__weak void *fifo_hw_lock_create(void)
{
    return (void *)spin_lock_instance(next_striped_spin_lock_num());
}

__weak uint32_t fifo_hw_lock(void *lock)
{
    return spin_lock_blocking((spin_lock_t *)lock);
}

__weak void fifo_hw_unlock(void *lock, uint32_t saved)
{
    spin_unlock((spin_lock_t *)lock, saved);
}