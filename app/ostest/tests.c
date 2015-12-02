#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"

static volatile int gval = 0;

void test_delay(void){
    uint64_t t0 = uptime();
    osDelay(1000);
    uint64_t t1 = uptime();
    TEST_ASSERT_INT32_WITHIN(5, 1000, (uint32_t)t1-t0);
}

static void test_val_thread(void const * arg){
    gval = 1;
    LOGT("test_thread");
    osThreadTerminate(osThreadGetId());
}

void test_thread_creation(void){
    osThreadDef_t t2 = (osThreadDef_t){
        .name = "1",
        .pthread = test_val_thread,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 128,
    };
    osThreadCreate(&t2, 0);
    osDelay(10);
    TEST_ASSERT_EQUAL(1, gval);
}
