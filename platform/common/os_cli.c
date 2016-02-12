#include "os_cli.h"
#include "heap.h"
#include "cmsis_os.h"
#include "io.h"
#include "util.h"
#include <string.h>

#define CLI_MAX_ARGS 6
typedef struct{
    ps_topic_t topic;
    ps_topic_t secondary_output;
    size_t fork_stack_size;
    const cli_command_node_t * tbl;
}cli_context_t;


/****************************************
 * We have some default commands so we don't have to reimplement for all apps!
 **********************************/
static int _command_echo(int argc, char * argv[]){
    if(argc > 1){
        int i;
        for(i = 1; i < argc; i++){
            LOGI("%s ", argv[i]);
        }
        LOGI("\r\n");
    }
    return 0;
}
#include "heap.h"
static int _command_free(int argc, char * argv[]){
    LOGI("Free %u\r\n", os_free_heap_size());
    return 0;
}
static int _command_set_log_level(int argc, char * argv[]){
    if (argc > 1){
        char * itr = argv[1];
        uint32_t mylevel = 0;
        bool has_error = false;
        while(*itr){
            switch(*itr){
                case 'D':
                case 'd':
                    mylevel |= LOG_LEVEL_DEBUG;
                    break;
                case 'I':
                case 'i':
                    mylevel |= LOG_LEVEL_INFO;
                    break;
                case 'E':
                case 'e':
                    mylevel |= LOG_LEVEL_ERROR;
                    break;
                case 'T':
                case 't':
                    mylevel |= LOG_LEVEL_TEST;
                    break;
                default:
                    has_error = true;
                    break;
            }
            itr++;
        }
        if( has_error ){
            LOGE("Accepted characters are \"[DIET\"\r\n");
            return -1;
        }else{
            os_set_loglevel(mylevel);
        }
    }
    return 0;
}
static cli_command_node_t default_command_tbl[] = {
    {"echo", _command_echo,},
    {"free", _command_free,},
    {"loglevel", _command_set_log_level,},
    {0,0},
};

/**
 * much code copy pasted from
 * https://code.google.com/p/embox/source/browse/trunk/embox/src/lib/shell/tokenizer.c?r=6132
 * TODO: add quote supports
 **/
static bool
_isspace(char b){
    return b == ' ';
}
static char *
_next_token(char ** str){
    char * ret;
    char *ptr = *str;

    while( _isspace(*ptr)){
        ++ptr;
    }
    if(! *ptr){
        return NULL;
    }
    ret = (char *)ptr;

    while(*ptr && !_isspace(*ptr)){
        ++ptr;
    }
    *str = ptr;
    return ret;
}

static int
_tokenize(char * string, char **argv){
    int argc = 0;
    char * token;
    while(*string != '\0' && argc < CLI_MAX_ARGS){
        argv[argc++] = (token = _next_token(&string));
        if(*string){
            *string++ = '\0';
        }
    }
    return argc;
}
/**
 * end
 **/
static osStatus _handle_command(char * string, size_t string_size, const cli_command_node_t * head, int * ret_code){
    const cli_command_node_t * itr = head;
    char * argv[CLI_MAX_ARGS] = {0};
    int argc = _tokenize(string,argv);
    while(itr && itr->command){
        if( strncmp(itr->command, argv[0], strlen(itr->command)) == 0 && itr->cb ){
            *ret_code = itr->cb(argc, argv);
            return osOK;
        }
        itr++;
    }
    return osErrorResource;
}
static void _cli_daemon(const void * arg){
    cli_context_t * ctx = (cli_context_t *)arg;
    ps_channel_t * ch = ps_subscribe(ctx->topic);
    LOGI("Connecting CLI to channel %u\r\n", ctx->topic);
    while(ch){
        LOGD("%u>",ctx->topic);
        ps_message_t * msg = ps_recv(ch, osWaitForever, NULL);
        if ( msg ){
            int code = 0;
            osStatus ret;
handle_default:
            ret = _handle_command(msg->data, msg->sz, default_command_tbl, &code);
            if( osOK == ret ){
                LOGI("Command returned %d\r\n", code);
                goto done;
            }
handle_user:
            ret = _handle_command(msg->data, msg->sz, ctx->tbl, &code);
            if( osOK == ret ){
                LOGI("Command returned %d\r\n", code);
            }else{
                LOGI("Command not found\r\n");;
            }
done:
            ps_free_message(msg);
        }
    }
    LOGE("Fatal Error: CLI unable to acquire topic\r\n");
    END_THREAD();
}

osStatus os_cli_daemon_start(ps_topic_t topic, size_t stack_size, const cli_command_node_t * head){
    osThreadDef_t t = (osThreadDef_t){
        .name = "clid",
        .pthread = _cli_daemon,
        .tpriority = 2,
        .instances = 1,
        .stacksize = stack_size,
    };
    cli_context_t * ctx = (cli_context_t*)os_malloc(sizeof(*ctx));
    if(ctx){
        ctx->topic = topic;
        ctx->fork_stack_size = stack_size;
        ctx->tbl = head;
        if(osThreadCreate(&t, ctx)){
            return osOK;
        }
    }
    return osErrorNoMemory;
}
