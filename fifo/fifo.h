#ifndef FIFO_FIFO_H
#define FIFO_FIFO_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Minimal, portable, lock-protected ring-buffer FIFO.
 *
 * Carries arbitrary fixed-size elements (e.g. structs) via `element_size`.
 * Only non-blocking try_* operations are provided. Cross-core / IRQ safety is
 * delegated to the hardware layer (fifo_hw_rp2040.*), so porting only touches
 * that layer.
 *
 * Storage is caller-provided: `buffer` must hold element_size * element_count
 * bytes and stay valid for the FIFO lifetime.
 */
typedef struct
{
    uint8_t *buffer;
    uint16_t element_size;
    uint16_t element_count;
    volatile uint16_t head;   /* next read slot  */
    volatile uint16_t tail;   /* next write slot */
    volatile uint16_t count;  /* elements in use */
    void *lock;               /* opaque hardware lock handle */
} fifo_t;

/* Initialize q with caller-owned storage for element_count fixed-size elements. */
void fifo_init(fifo_t *q, uint16_t element_size, uint16_t element_count, void *buffer);

/* Add one element. Returns false without modifying q when the FIFO is full. */
bool fifo_try_add(fifo_t *q, const void *data);

/* Remove the oldest element. Returns false without writing data when empty. */
bool fifo_try_remove(fifo_t *q, void *data);

/* Copy the oldest element without removing it. Returns false when empty. */
bool fifo_try_peek(fifo_t *q, void *data);

#endif // FIFO_FIFO_H