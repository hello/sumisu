#include "pubsub.h"
#include "util.h"
#include "io.h"

/**
 * this directly affects the throughput of the pubsub system
 */
#define DEFAULT_PS_QUEUE_DEPTH 10

typedef struct _async_listener{
    async_on_message cb;
    struct _async_listener * next;
}async_listener_t;

struct ps_channel_t{
    osMailQId q;
    ps_channel_t * next;
};

typedef struct{
    ps_channel_t channel;
//async inherits from syncchannel
    osThreadId worker;
    osMutexId lock;
    async_listener_t * listeners;
}ps_async_channel_t;

static osMutexId _channel_lock;
static ps_channel_t * _channels[PS_CHANNEL_SIZE];
static const osMutexDef_t mdef = (osMutexDef_t){0};

osStatus ps_init(void){
    _channel_lock = osMutexCreate(&mdef);
    if( !_channel_lock ){
        return osErrorResource;
    }
    return osOK;
}
static void _init_channel(ps_channel_t * ret){
    osMailQDef_t def = (osMailQDef_t){
        .queue_sz = DEFAULT_PS_QUEUE_DEPTH,
        .item_sz = sizeof(ps_message_t),
    };
    ret->q = osMailCreate(&def, NULL);
    ret->next = NULL;
}
static void _async_worker(void const * arg){
    ps_async_channel_t * ch = (ps_async_channel_t*)arg;
    osStatus rc;
    while(1){
        ps_message_t * msg = NULL;
        async_listener_t * itr = NULL;

        rc = ps_listen((ps_channel_t*)arg, &msg, osWaitForever);

        if( rc == osOK ){
            if( msg ){
                itr = ch->listeners;
                while( itr ){
                    itr->cb(msg->data, msg->sz);
                    if( osOK == osMutexWait(ch->lock, osWaitForever) ){
                        itr = itr->next;
                        osMutexRelease(ch->lock);
                    }
                }
                ps_free_message(msg);
            }
        }else{
            LOGE("listen error %d", rc);
        }
    }
}
static ps_channel_t * _new_async_channel(void){
    ps_async_channel_t * ret = os_malloc(sizeof(*ret));
    if (ret) {
        _init_channel((ps_channel_t*)ret);
        osThreadDef_t def = (osThreadDef_t){
            .name = "async_worker",
            .pthread = _async_worker,
            .tpriority =  2,
            .instances = 1,
            .stacksize = 256,//TODO find a good value
        };
        ret->lock = osMutexCreate(&mdef);
        if( ! ret->lock ){
            LOGE("kaboom\r\n");
            os_free(ret);
            return NULL;
        }
        ret->worker = osThreadCreate(&def, ret);
        if( !ret->worker ){
            LOGE("kaboom2\r\n");
            //TODO destroy
            return NULL;
        }
    }
    return ret;
}
osStatus _publish(ps_channel_t * ch, void * data, size_t sz, uint32_t millisec){
    osStatus rc = osErrorOS;
    ps_message_t * msg = NULL;

    rc = osErrorTimeoutResource;
    msg = osMailCAlloc(ch->q, millisec); 

    if( msg ){
        msg->data = os_malloc(sz);
        if( !msg->data ){
            osMailFree(msg->parent, msg);
            return osErrorNoMemory;
        }else{
            msg->parent = ch->q;
            msg->sz = sz;
            memcpy(msg->data, data, sz);
        }
        return osMailPut(ch->q, msg);
    }

    return rc;
}
osStatus ps_publish_timeout(ps_channel_type channel, void * data, size_t sz, uint32_t millisec){
    osStatus rc;
    rc = osMutexWait(_channel_lock, osWaitForever);
    if(rc != osOK){
        LOGE("Channel Timeout\r\n");
        return rc;
    }
    ps_channel_t * head = _channels[channel];
    osMutexRelease(_channel_lock);

    ps_channel_t * itr = head;

    while( itr ){
        rc = _publish(itr, data, sz, millisec);
        if( rc != osOK ){
            break;
        }
        osMutexWait(((ps_async_channel_t*)head)->lock, osWaitForever);
        itr = itr->next;
        osMutexRelease(((ps_async_channel_t*)head)->lock);
    }
    return rc;
}
/*
 * sends a message to the channel
 */
osStatus ps_publish(ps_channel_type channel, void * data, size_t sz){
    return ps_publish_timeout(channel, data, sz, 1);
}
static osStatus _append_channel(ps_async_channel_t * head, ps_channel_t * node){
    ps_channel_t * itr = (ps_channel_t*)head;
    while ( itr->next ){
        itr = itr->next;
    }
    itr->next = node;
    return osOK;
}
static osStatus _append_listener(ps_async_channel_t * head, async_on_message opt_cb){
    osStatus rc = osOK;
    async_listener_t * node = os_malloc(sizeof(*node));
    if ( !node ){
        rc = osErrorResource;
    }else{
        node->next = NULL;
        node->cb = opt_cb;
    }
    async_listener_t * itr = head->listeners;
    if(!itr){
        head->listeners = node;
    }else{
        while(itr->next){
            itr = itr->next;
        }
        itr->next = node;
    }
    return rc;
}
static ps_channel_t *_new_channel(void){
    ps_channel_t * ret = os_malloc(sizeof(ret));
    if ( ret ){
        _init_channel(ret);
    }
    return ret;
}
/*
 * if opt_cb is supplied, it'll be called asynchronously on a publish event
 * if NULL is passed as opt_cb, then it'll run in receiver mode where the subscriber 
 * gets the message by calling ps_listen()
 */
ps_channel_t * ps_subscribe(ps_channel_type channel, async_on_message opt_cb){
    osStatus rc;
    ps_channel_t * ret = NULL;
    osMutexWait(_channel_lock, osWaitForever);
    ps_channel_t * head = _channels[channel];
    if ( !head ){
        //the first node is the async channel
        head = _new_async_channel();
        _channels[channel] = head;
    }
    osMutexRelease(_channel_lock);

    osMutexWait(((ps_async_channel_t*)head)->lock, osWaitForever);
    if( !opt_cb ){
        ret = _new_channel();
        _append_channel(head, ret);
    }else{
        _append_listener((ps_async_channel_t*)head, opt_cb);
    }
    osMutexRelease(((ps_async_channel_t*)head)->lock);

    return ret;
}

/**
 * the following pair is only ran in synchronous mode
 */
osStatus ps_listen(const ps_channel_t * channel, ps_message_t ** out_message, uint32_t millisec){
    osEvent evt = osMailGet(channel->q, millisec);
    if(evt.status = osEventMessage){
        *out_message = evt.value.p;
        return osOK;
    }
    return evt.status;
}
osStatus ps_free_message(ps_message_t * message){
    os_free(message->data);
    return osMailFree(message->parent, message);
}
