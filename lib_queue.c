/**
 * file:    lib_queue.c
 * author:  wallying@foxmail.com
 * date:    2019-12-16
 **/

#include "lib_queue.h"


#define LIBQ_DBG_EN (1)


void queue_init(queue_t *queue, void *buf, unsigned int len, unsigned int size)
{
    if (queue == NULL) {
        return;
    }

    queue->buf = buf;
    queue->len = len;
    queue->size = size;
    queue->widx = 0;
    queue->ridx = 0;
    queue->full = 0;
}


void queue_clear(queue_t *queue)
{
    if (queue == NULL) {
        return;
    }

    queue->widx = 0;
    queue->ridx = 0;
    queue->full = 0;
}


int queue_is_empty(const queue_t *queue)
{
    if (queue == NULL) {
        return 0;
    }

    if ((queue->widx == queue->ridx) && (queue->full == 0)) {
        return 1;
    }

    return 0;

//    return (queue->ridx == queue->widx) ? 1 : 0;
}


int queue_is_full(const queue_t *queue)
{
    if (queue == NULL) {
        return 0;
    }

    return queue->full;

//  return (queue->ridx == (queue->widx + 1) % queue->len) ? 1 : 0;
}


unsigned int queue_item_len(const queue_t *queue)
{
    if (queue == NULL) {
        return 0;
    }

    if (queue->full) {
        return queue->len;
    } else {
        return (queue->widx + queue->len - queue->ridx) % queue->len;
    }

//  return (queue->widx + queue->len - queue->ridx) % queue->len;
}


unsigned int queue_free_len(const queue_t *queue)
{
    if (queue == NULL) {
        return 0;
    }

    if ((queue->widx == queue->ridx) && (queue->full == 0)) {
        return queue->len;
    } else {
        return (queue->ridx + queue->len - queue->widx) % queue->len;
    }

//  return (queue->ridx + queue->len - 1 - queue->widx) % queue->len;
}


#if (LIBQ_DBG_EN)
static void *mem_set(void *buf, int val, unsigned int cnt)
{
    void *ptr = buf;
    while (cnt--) {
        *(char *)buf++ = (char)val;
    }
    return ptr;
}
#endif
static void *mem_cpy(void *dst, const void *src, unsigned int cnt)
{
    void *ptr = dst;
    while (cnt--) {
        *(char *)dst++ = *(char *)src++;
    }
    return ptr;
}


//向queue队尾写入len个数据项。返回实际写入的数据项个数。
unsigned int queue_write(queue_t *queue, void *buf, unsigned int len)
{
    if ((queue == NULL) || (buf == NULL) || (len == 0)) {
        return 0x00;
    }

    unsigned int cnt = 0;

    for (cnt = 0; cnt < len; cnt++) {
        /* circle queue is full */
        if (queue->full) {
            //if (queue->ridx == (queue->widx + 1) % queue->len) {
            break;
        } else {
            mem_cpy(queue->buf + queue->widx * queue->size,
                    buf + cnt * queue->size,
                    queue->size);
            queue->widx = (queue->widx + 1) % queue->len;

            if (queue->ridx == queue->widx) {
                queue->full = 1;
            }
        }
    }

    return cnt;
}


//向queue队尾rewind写入len个数据项。返回实际写入的数据项个数。
unsigned int queue_write_rewind(queue_t *queue, void *buf, unsigned int len)
{
    if ((queue == NULL) || (buf == NULL) || (len == 0)) {
        return 0x00;
    }

    unsigned int cnt = 0;

    for (cnt = 0; cnt < len; cnt++) {
        /* circle queue is full */
        if (queue->full) {
            //if (queue->ridx == (queue->widx + 1) % queue->len) {
#if (LIBQ_DBG_EN)
            mem_set(queue->buf + queue->ridx * queue->size,
                    0x00,
                    queue->size);
#endif
            queue->ridx = (queue->ridx + 1) % queue->len;
        }
        mem_cpy(queue->buf + queue->widx * queue->size,
                buf + cnt * queue->size,
                queue->size);
        queue->widx = (queue->widx + 1) % queue->len;

        if (queue->ridx == queue->widx) {
            queue->full = 1;
        }
    }

    return cnt;
}


//向queue队首写入len个数据项。返回实际写入的数据项个数。
unsigned int queue_push(queue_t *queue, void *buf, unsigned int len)
{
    if ((queue == NULL) || (buf == NULL) || (len == 0)) {
        return 0x00;
    }

    unsigned int cnt = 0;

    for (cnt = 0; cnt < len; cnt++) {
        /* circle queue is full */
        if (queue->full) {
            //if (queue->ridx == (queue->widx + 1) % queue->len) {
            break;
        } else {
            queue->ridx = (queue->ridx + queue->len - 1) % queue->len;
            mem_cpy(queue->buf + queue->ridx * queue->size,
                    buf + cnt * queue->size,
                    queue->size);
            if (queue->ridx == queue->widx) {
                queue->full = 1;
            }
        }
    }

    return cnt;
}



//向queue队首rewind写入len个数据项。返回实际写入的数据项个数。
unsigned int queue_push_rewind(queue_t *queue, void *buf, unsigned int len)
{
    if ((queue == NULL) || (buf == NULL) || (len == 0)) {
        return 0x00;
    }

    unsigned int cnt = 0;

    for (cnt = 0; cnt < len; cnt++) {
        /* circle queue is full */
        if (queue->full) {
            //if (queue->ridx == (queue->widx + 1) % queue->len) {
            break;
        } else {
            queue->ridx = (queue->ridx + queue->len - 1) % queue->len;
            mem_cpy(queue->buf + queue->ridx * queue->size,
                    buf + cnt * queue->size,
                    queue->size);
            if (queue->ridx == queue->widx) {
                queue->full = 1;
            }
        }
    }

    return cnt;
}


//从queue中(队首)读取len个数据项到buf中。返回实际读取的数据项个数。
unsigned int queue_read(queue_t *queue, void *buf, unsigned int len)
{
    if ((queue == NULL) || (buf == NULL) || (len == 0)) {
        return 0x00;
    }

    unsigned int cnt = 0;

    for (cnt = 0; cnt < len; cnt++) {
        /* circle queue is empty */
        if ((queue->ridx == queue->widx) && (queue->full == 0)) {
        //if (queue->ridx == queue->widx) {
            break;
        } else {
            mem_cpy(buf + cnt * queue->size,
                    queue->buf + queue->ridx * queue->size,
                    queue->size);
#if (LIBQ_DBG_EN)
            mem_set(queue->buf + queue->ridx * queue->size,
                    0x00,
                    queue->size);
#endif
            queue->ridx = (queue->ridx + 1) % queue->len;
            queue->full = 0;
        }
    }

    return cnt;
}


