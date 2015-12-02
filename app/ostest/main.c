#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"

static volatile int gval = 0;

static void test_delay(void){
    uint64_t t0 = uptime();
    osDelay(1000);
    uint64_t t1 = uptime();
    TEST_ASSERT_INT32_WITHIN(5, 1000, (uint32_t)t1-t0);
}
static void test_val_thread(void const * arg){
    gval = 1;
    LOGT("test_thread");
    while(1){
    }
}
static void test_thread(void){
    osThreadDef_t t2 = (osThreadDef_t){
        .name = "1",
        .pthread = test_val_thread,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 128,
    };
    LOGT("zk");
    osThreadCreate(&t2, 0);
    LOGT("huh");
    osDelay(10);
    LOGT("bub");
    TEST_ASSERT_EQUAL(1, gval);
    LOGT("wuw");
    while(1);
}

static void test_thread_main(void const * arg){
    UnityBegin(__FILE__);

    RUN_TEST(test_delay);
    RUN_TEST(test_thread);

    LOGT("Test Exit: %d\r\n", UnityEnd());
    while(1){
    }
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
