#include "dtm.h"
#include "io.h"
#include "cmsis_os.h"
#include "unity.h"

static void test_assert_stuff(void){
    TEST_ASSERT_EQUAL_HEX(0x1, 1);
    TEST_ASSERT_EQUAL_HEX(0x2, 1);
}

static void test1(void const * arg){
    UnityBegin(__FILE__);
    RUN_TEST(test_assert_stuff);
    os_printf("%d\r\n", UnityEnd());
    while(1){
    }
}
int main(int argc, char * argv[]){
    osThreadDef_t t1 = (osThreadDef_t){
        .name = "test1",
        .pthread = test1,
        .tpriority = 0,
        .instances = 1,
        .stacksize = 512,
    };

    osKernelInitialize();
    osThreadCreate(&t1, 0);
    osKernelStart();
    for(;;);
    return 0;
}
