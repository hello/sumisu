#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"
#include "util.h"


extern void test_delay(void);
extern void test_thread_creation(void);
extern void test_heap(void);
extern void test_mutex(void);
extern void test_timer(void);
extern void test_mail(void);
extern void test_ps(void);

static void test_thread_main(void const * arg){
    UnityBegin(__FILE__);

    RUN_TEST(test_ps);
    RUN_TEST(test_heap);
    RUN_TEST(test_delay);
    RUN_TEST(test_thread_creation);
    RUN_TEST(test_mutex);
    RUN_TEST(test_timer);
    RUN_TEST(test_mail);

    LOGT("Test Exit: %d\r\n", UnityEnd());
    while(1){
        osDelay(1000);
    }
    END_THREAD();
}

int main(int argc, char * argv[]){
    osThreadDef_t t1 = (osThreadDef_t){
        .name = "ostest",
        .pthread = test_thread_main,
        .tpriority = 0,
        .instances = 1,
        .stacksize = 2048,
    };
    osKernelInitialize();
    osThreadCreate(&t1, 0);
    osKernelStart();
    while(1){
    }
    return 0;
}
