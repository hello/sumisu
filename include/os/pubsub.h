#ifndef PUBSUB_H
#define PUBSUB_H

#include "pubsub_channels.h"
#include "stddef.h"
#include "cmsis_os.h"
/**
 * pubsub is an extension of the CMSIS osMail module
 * that handles multiplexing and discovery for sumisu
 */

typedef struct{
    osMailQId parent;
    const void * data;
    size_t sz;
}ps_message_t;
typedef void(*async_on_message)(const void * data, size_t sz);
typedef struct ps_channel_t  ps_channel_t;

osStatus ps_init(void);
/*
 * sends a message to the channel
 */
osStatus ps_publish(ps_channel_type channel, void * data, size_t sz);
/*
 * sends a message to the channel with specific timeout
 */
osStatus ps_publish_timeout(ps_channel_type channel, void * data, size_t sz, uint32_t millisec);
/*
 * if opt_cb is supplied, it'll be called asynchronously on a publish event
 * if NULL is passed as opt_cb, then it'll run in receiver mode where the subscriber 
 * gets the message by calling ps_listen()
 */
ps_channel_t * ps_subscribe(ps_channel_type channel, async_on_message opt_cb);

/**
 * the following pair is only ran in synchronous mode
 */
osStatus ps_listen(const ps_channel_t * channel, ps_message_t ** out_message, uint32_t millisec);
osStatus ps_free_message(ps_message_t * message);

#endif
