#include "pubsub.h"
#include "util.h"
#include "io.h"
#include "heap.h"
#include <string.h>

struct ps_channel{
    osMailQId q;
};
/**
 * this directly affects the throughput of the pubsub system
 */
#define DEFAULT_PS_QUEUE_DEPTH 10


/*****************************
 * CHANNEL MANAGEMENT IMPL
 * TODO: better data structure, array of linked list right now
 ****************************/
typedef struct ps_channel_list{
    ps_channel_t * channel;
    struct ps_channel_list * next;
}ps_channel_list_t;
static osMutexId _channel_list_lock;
static ps_channel_list_t * _channels[PS_TOPIC_SIZE];

static void _lock_channel_list(void){
    osStatus rc = osMutexWait(_channel_list_lock, osWaitForever);
    if ( rc == osOK || rc == osErrorISR ){
        //we are OK
    }else{
        LOGE("Fatal Error, channel list not initiated\r\n");
    }
}
static void _unlock_channel_list(void){
    osMutexRelease(_channel_list_lock);
}

static ps_channel_list_t * _channel_list_from_topic(ps_topic_t topic){
    ps_channel_list_t * head = NULL;
    if ( topic < PS_TOPIC_SIZE ){
        head = _channels[topic];
    }
    return head;
}

static osStatus _channel_list_init(void){
    osMutexDef_t mdef = (osMutexDef_t){0};
    _channel_list_lock = osMutexCreate(&mdef);
    if( !_channel_list_lock ){
        return osErrorResource;
    }
    return osOK;
}
/**
 *  creates a node on the channel list and set its contents to ch
 *  then return the head of channel list
 */
static ps_channel_list_t * _channel_list_append(ps_topic_t topic, ps_channel_t * ch){
    if ( topic >= PS_TOPIC_SIZE ){
        return NULL;
    }
    _lock_channel_list();
    if ( ch ) {
        //assume there are no duplicates on the ch
        ps_channel_list_t * itr = _channels[topic];
        ps_channel_list_t * node = os_malloc(sizeof(*node));
        node->channel = ch;
        node->next = NULL;
        if ( !itr ){
            _channels[topic]  = node;
        }else{
            while(itr->next){
                itr = itr->next;
            }
            itr->next = node;
        }
    }
    ps_channel_list_t * ret = _channels[topic];
    _unlock_channel_list();
    return ret;
}

/*****************************
 * PUBSUB API IMPL
 * TODO reference count root message
 ****************************/
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


//finish this
osStatus ps_publish(ps_topic_t topic, const void * data, size_t sz){
    if( !data || !sz ){
        return osErrorParameter;
    }
    ps_channel_list_t * itr = _channel_list_append(topic, NULL);
    while ( itr ){
        osMailQId q = itr->channel->q;
        ps_message_t * msg = osMailAlloc(q, 0);
        if ( msg ){
            msg->data = (void*)os_malloc(sz);
            if ( ! msg->data ){
                osMailFree(q, msg);
                return osErrorNoMemory;
            }else{//try to implement copy once with reference count
                msg->parent = q;
                msg->sz = sz;
                msg->topic = topic;
                memcpy(msg->data, data, sz);
                osMailPut(q, msg);
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
