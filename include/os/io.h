#ifndef IO_H
#define IO_H
#include <stdarg.h>
#include <stdint.h>

#define LOG_LEVEL_DEBUG     1   /* platform and driver development */
#define LOG_LEVEL_INFO      2   /* app development stuff */
#define LOG_LEVEL_ERROR     4   /* both app and platform */
#define LOG_LEVEL_TEST      8   /* factory and fcc stuff, and tests */
#define LOG_LEVEL_ALL       0xffffffff

#define LOGD(...) os_log(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOGI(...) os_log(LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOGE(...) os_log(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOGT(...) os_log(LOG_LEVEL_TEST,  __VA_ARGS__)

void os_uart_init(void);
/**
 * Use this for development
 */
void os_log(uint32_t loglevel, const char * format, ...);
uint32_t os_get_loglevel(void);
void os_set_loglevel(uint32_t loglevel);

#include "pubsub_topics.h"
void os_uart_set_broadcast_topic(ps_topic_t topic);


/*
 * platform specific
 */
void os_putc(char c);

#endif
