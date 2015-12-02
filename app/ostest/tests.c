#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"
#include "util.h"

static volatile int gval = 0;
static volatile int gcount = 0;


static void _val_thread(void const * arg){
    gval = 1;
    END_THREAD();
}
#include "crypto.h"
#include "heap.h"
static void _heap_thrasher(void const * arg){
    int i = 0;
    while(++i < 10){
        size_t sz = os_rand()%128 + 1;
        int delay = os_rand()%10 + 10;
        void * test = os_malloc(sz);
        LOGT("A:%u\r\n",sz);
        TEST_ASSERT_NOT_NULL(test);
        osDelay(delay);
        os_free(test);
    }
    gcount++;
    END_THREAD();
}
//tests
void test_thread_creation(void){
    osThreadDef_t t2 = (osThreadDef_t){
        .name = "runner",
        .pthread = _val_thread,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    osThreadCreate(&t2, 0);
    osDelay(10);
    TEST_ASSERT_EQUAL(1, gval);
}

#include "heap.h"
void test_heap(void){
    gcount = 0;
    osThreadDef_t t = (osThreadDef_t){
        .name = "trasher0",
        .pthread = _heap_thrasher,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 512,
    };
    osThreadCreate(&t, 0);
    osThreadCreate(&t, 0);
    osThreadCreate(&t, 0);
    //lazy thread join
    while(gcount < 3){
        osDelay(10);
    }
}

void test_delay(void){
    uint64_t t0 = uptime();
    osDelay(1000);
    uint64_t t1 = uptime();
    TEST_ASSERT_INT32_WITHIN(5, 1000, (uint32_t)t1-t0);
}
