#include "pubsub.h"
#include "util.h"
#include "io.h"
#include "heap.h"
#include <string.h>

/**
 * this directly affects the throughput of the pubsub system
 */
#define DEFAULT_PS_QUEUE_DEPTH 10


struct ps_channel_t{
    osMailQId q;
    ps_channel_t * next;
};

static ps_channel_t * _new_channel(size_t depth);
/**
 * TODO have better data structure
 */
static osMutexId _channel_lock;
static ps_channel_t * _channels[PS_TOPIC_SIZE];

static osStatus _channel_list_init(void){
    osMutexDef_t mdef = (osMutexDef_t){0};
    _channel_lock = osMutexCreate(&mdef);
    if( !_channel_lock ){
        return osErrorResource;
    }
    return osOK;
}
static ps_channel_t * _append(ps_channel_t * head, ps_channel_t * node){
    if(!head){
        return node;
    }else{
        ps_channel_t * itr = head;
        while(itr->next){
            itr = itr->next;
        }
        itr->next = node;
    }
    return head;
}
/**
 * appends ch into the topic channel list if ch is not null
 * then returns the first element of the ps_channel_t linked list from that topic
 */
static ps_channel_t * _channel_list_append(ps_topic_t topic, ps_channel_t * ch){
    osStatus rc;
    ps_channel_t * ret = NULL;
    rc = osMutexWait(_channel_lock, osWaitForever);
    if( rc == osOK || rc == osErrorISR){//exception is we are in isr
        ret = _channels[topic];
        if( ch ){
            _channels[topic] = _append(ret, ch);
            ret = _channels[topic];
        }
        osMutexRelease(_channel_lock);
    }else{
        LOGE("Fatal Error, channel list not initiated\r\n");
    }
    return ret;
}

static ps_channel_t * _new_channel(size_t depth){
    osMailQDef_t def = (osMailQDef_t){
        .queue_sz = depth,
        .item_sz = sizeof(ps_message_t),
    };
    ps_channel_t * ret = (ps_channel_t*)os_malloc(sizeof(*ret));
    if(ret){
        ret->q = osMailCreate(&def, NULL);
        if( !ret->q ){
            os_free(ret);
            ret = NULL;
        }
    }
    return ret;
}

osStatus ps_init(void){
    return _channel_list_init();
}

osStatus ps_publish(ps_topic_t topic, const void * data, size_t sz){
    if( !data || !sz ){
        return osErrorParameter;
    }
    ps_channel_t * itr = _channel_list_append(topic, NULL);
    while ( itr ){
        ps_message_t * msg = osMailAlloc(itr->q, 0);
        if ( msg ){
            msg->data = (void*)os_malloc(sz);
            if ( ! msg->data ){
                osMailFree(itr->q, msg);
                return osErrorNoMemory;
            }else{//try to implement copy once with reference count
                msg->parent = itr->q;
                msg->sz = sz;
                memcpy(msg->data, data, sz);
                osMailPut(itr->q, msg);
            }
        }
        itr = itr->next;
    }
    return osOK;
}
ps_channel_t * ps_subscribe(ps_topic_t topic){
    ps_channel_t  * ret = _new_channel(DEFAULT_PS_QUEUE_DEPTH);
    if(ret){
        _channel_list_append(topic, ret);
    }
    return ret;
}
ps_message_t * ps_recv(const ps_channel_t * channel, uint32_t millisec, osStatus * opt_stat){
    ps_message_t * ret = NULL;
    osStatus rc = osErrorParameter;
    if(!channel){
        goto fexit;
    }
    osEvent evt = osMailGet(channel->q, millisec);
    if(evt.status == osEventMail){
        rc = osOK;
        ret = evt.value.p;
    }else{
        rc = osErrorTimeoutResource;
    }
fexit:
    if(opt_stat){
        *opt_stat = rc;
    }
    return ret;
}
osStatus ps_free_message(ps_message_t * message){
    if(message){
        if(message->data){
            os_free(message->data);
        }
        return osMailFree(message->parent, message);
    }else{
        return osErrorParameter;
    }
}

void ps_flush_channel(ps_channel_t * channel){
    osStatus rc;
    ps_message_t * ret;
    while( (ret = ps_recv(channel, 100, &rc )) != NULL ){
        ps_free_message(ret);
    }
}
