#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"

static void test_delay(void){
    uint64_t t0 = uptime();
    osDelay(1000);
    uint64_t t1 = uptime();
    TEST_ASSERT_INT32_WITHIN(5, 1000, (uint32_t)t1-t0);
}

static void test_thread(void const * arg){
    UnityBegin(__FILE__);

    RUN_TEST(test_delay);

    os_printf("Test Exit: %d\r\n", UnityEnd());
    while(1){
    }
}
int main(int argc, char * argv[]){
    osThreadDef_t t1 = (osThreadDef_t){
        .name = "ostest",
        .pthread = test_thread,
        .tpriority = 0,
        .instances = 1,
        .stacksize = 512,
    };

    osKernelInitialize();
    osThreadCreate(&t1, 0);
    osKernelStart();
    while(1){
    }
    return 0;
}
