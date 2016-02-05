#ifndef PUBSUB_H
#define PUBSUB_H

#include "pubsub_topics.h"
#include "stddef.h"
#include "cmsis_os.h"
/**
 * pubsub is an extension of the CMSIS osMail module
 * that handles the multiplexing and discovery for topics in sumisu
 * unlike mailboxes, it is nonblocking and do not guarantee delivery
 */

typedef struct{
    osMailQId parent;
    ps_topic_t topic;
    void * data;
    size_t sz;
}ps_message_t;

typedef struct ps_channel  ps_channel_t;

/*
 * called by osKernelInit, once per app
 */
osStatus ps_init(void);
/*
 * anyone can publish anything from anywhere
 * sends a message to the channel, nonblocking
 * warning: this does not guarantee message delivery due to:
 * 1. late subscription
 * 2. slow subscriber
 * 3. wrong message type sent
 */
osStatus ps_publish(ps_topic_t topic, const void * data, size_t sz);
/*
 * since messages are buffered on the subscriber side,
 * subscriber must call ps_recv on a channel created by ps_subscribe
 * then free the message after use otherwise it'll cause memory leak
 */
ps_channel_t * ps_subscribe(ps_topic_t topic);
ps_message_t * ps_recv(const ps_channel_t * channel, uint32_t millisec, osStatus * opt_stat);
osStatus ps_free_message(ps_message_t * message);
/**
 * adds an additional topic to the channel, 
 * messages are differentiated by the topic field in ps_message
 */
ps_channel_t * ps_add_topic(ps_channel_t * ch, ps_topic_t topic);
/**
 * flush channel cleans up the channel from all messages
 * useful when listening again after some time
 */
void ps_flush_channel(ps_channel_t * channel);

#endif
