#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"


extern void test_delay(void);
extern void test_thread_creation(void);
extern void test_heap(void);
extern void test_mutex(void);

static void test_thread_main(void const * arg){
    UnityBegin(__FILE__);

    RUN_TEST(test_heap);
    RUN_TEST(test_delay);
    RUN_TEST(test_thread_creation);
    RUN_TEST(test_mutex);

    LOGT("Test Exit: %d\r\n", UnityEnd());
    osDelay(1);
    osThreadTerminate(osThreadGetId());
}

int main(int argc, char * argv[]){
    osThreadDef_t t1 = (osThreadDef_t){
        .name = "ostest",
        .pthread = test_thread_main,
        .tpriority = 0,
        .instances = 1,
        .stacksize = 128,
    };
    osKernelInitialize();
    osThreadCreate(&t1, 0);
    osKernelStart();
    while(1){
    }
    return 0;
}
