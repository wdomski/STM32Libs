#ifndef FIFO_FIFO_HW_RP2040_H
#define FIFO_FIFO_HW_RP2040_H

#include <stdint.h>

/*
 * RP2040/RP2350 hardware abstraction for fifo.c. Hardware spinlocks provide
 * cross-core mutual exclusion.
 */

/* Allocate a striped hardware spinlock for one FIFO instance. */
void *fifo_hw_lock_create(void);

/* Acquire lock and return the interrupt state required by fifo_hw_unlock(). */
uint32_t fifo_hw_lock(void *lock);

/* Release lock and restore the interrupt state returned by fifo_hw_lock(). */
void fifo_hw_unlock(void *lock, uint32_t saved);

#endif // FIFO_FIFO_HW_RP2040_H