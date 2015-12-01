/**
 * wip -jackson
 * none of these adheres to the standard but will get the target running on the platform
 */
#include <sys/types.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include "cmsis_os.h"
#include "io.h"

osStatus osKernelStart (void){
    os_printf("Sumisu - Darwin\r\n");
    return osOK;
}
osStatus osKernelInitialize (void){
    return osOK;
}
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument){
    pthread_t thread;
    pthread_attr_t attr;
    if(0 != pthread_attr_init(&attr)){
        //error
    }
    pthread_attr_setstacksize(&attr, thread_def->stacksize);
    int ret = pthread_create(
            &thread,
            &attr,
            thread_def->pthread,
            argument
            );
    if(0 == ret){
        return 0;
    }else{
        return (osThreadId)thread;
    }
}
osStatus osDelay(uint32_t millisec){
    /*
     *int rc;
     *struct timespec t = (struct timespec){
     *    .tv_sec = 0,
     *    .tv_nsec = millisec * 1000,
     *};
     *rc = pthread_delay_np(&t);
     */
    usleep(millisec * 1000);
    return osOK;
            
}
