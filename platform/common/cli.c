#include "cli.h"
#include "cmsis_os.h"
#include "io.h"
#include "util.h"

typedef struct{
    ps_topic_t topic;
    size_t fork_stack_size;
    //add sem for max forks
}cli_context_t;

static void _cli_daemon(const void * arg){
    cli_context_t * ctx = (cli_context_t *)arg;
    ps_channel_t * ch = ps_subscribe(ctx->topic);
    while(ch){
        ps_message_t * msg = ps_recv(ch, osWaitForever, NULL);
        if ( msg ){
            LOGI("Received Command %s\r\n", msg->data);
            ps_free_message(msg);
        }
    }
    LOGE("Fatal Error: CLI unable to acquire topic\r\n");
    END_THREAD();
}

osStatus os_cli_daemon_start(ps_topic_t topic, size_t max_forks, size_t fork_stack_size){
    osThreadDef_t t = (osThreadDef_t){
        .name = "cli",
        .pthread = _cli_daemon,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 128,
    };
    cli_context_t * ctx = os_malloc(sizeof(*ctx));
    if(ctx){
        ctx->topic = topic;
        ctx->fork_stack_size = fork_stack_size;
        osThreadCreate(&t, ctx);
    }else{
        return osErrorNoMemory;
    }

}
