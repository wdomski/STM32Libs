#include <string.h>

#include "fifo/fifo.h"
#include "fifo/fifo_hw_rp2040.h"

void fifo_init(fifo_t *q, uint16_t element_size, uint16_t element_count, void *buffer)
{
    q->buffer = (uint8_t *)buffer;
    q->element_size = element_size;
    q->element_count = element_count;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->lock = fifo_hw_lock_create();
}

bool fifo_try_add(fifo_t *q, const void *data)
{
    bool ok = false;
    uint32_t saved = fifo_hw_lock(q->lock);
    if (q->count < q->element_count)
    {
        memcpy(q->buffer + (uint32_t)q->tail * q->element_size, data, q->element_size);
        q->tail = (q->tail + 1) % q->element_count;
        q->count++;
        ok = true;
    }
    fifo_hw_unlock(q->lock, saved);
    return ok;
}

bool fifo_try_remove(fifo_t *q, void *data)
{
    bool ok = false;
    uint32_t saved = fifo_hw_lock(q->lock);
    if (q->count > 0)
    {
        memcpy(data, q->buffer + (uint32_t)q->head * q->element_size, q->element_size);
        q->head = (q->head + 1) % q->element_count;
        q->count--;
        ok = true;
    }
    fifo_hw_unlock(q->lock, saved);
    return ok;
}

bool fifo_try_peek(fifo_t *q, void *data)
{
    bool ok = false;
    uint32_t saved = fifo_hw_lock(q->lock);
    if (q->count > 0)
    {
        memcpy(data, q->buffer + (uint32_t)q->head * q->element_size, q->element_size);
        ok = true;
    }
    fifo_hw_unlock(q->lock, saved);
    return ok;
}