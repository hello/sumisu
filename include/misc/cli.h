#ifndef CLI_H
#define CLI_H
#include "pubsub.h"

/**
 * starts cli daemon on a certain topic
 * forks a thread for each topic
 */
osStatus os_cli_daemon_start(ps_topic_t topic, size_t max_forks, size_t fork_stack_size);

#endif
