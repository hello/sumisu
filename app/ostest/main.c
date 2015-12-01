#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"

static void test_delay(void){
    uint64_t t0 = uptime();
    osDelay(1000);
    uint64_t t1 = uptime();
    TEST_ASSERT_INT64_WITHIN(10, 1000, t1-t0);
}
static void test1(void const * arg){
    while(1){
    }
}
int main(int argc, char * argv[]){
/*
 *    osThreadDef_t t1 = (osThreadDef_t){
 *        .name = "test1",
 *        .pthread = test1,
 *        .tpriority = 0,
 *        .instances = 1,
 *        .stacksize = 512,
 *    };
 *
 *    osKernelInitialize();
 *    osThreadCreate(&t1, 0);
 *    osKernelStart();
 *    while(1){
 *    }
 *    return 0;
 */
    UnityBegin(__FILE__);
    RUN_TEST(test_delay);
    os_printf("Test Exit: %d\r\n", UnityEnd());
}
