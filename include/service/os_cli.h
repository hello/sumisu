#ifndef OS_CLI_H
#define OS_CLI_H
#include "pubsub.h"

typedef int (*cli_command_cb)(int argc, char * argv[]);
typedef struct{
    const char * command;
    cli_command_cb cb;
}cli_command_node_t;

/**
 * starts cli daemon on a certain topic and a runs table of commands provided(2d array terminated with an empty element)
 */
osStatus os_cli_daemon_start(ps_topic_t topic, size_t stack_size, const cli_command_node_t * head);



#endif
