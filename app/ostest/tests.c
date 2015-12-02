#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"
#include "util.h"

static volatile int gval = 0;
static volatile int gcount = 0;
static volatile int gtest_mutex_counter = 0;
static osMutexId gmutex = NULL;


static void _val_thread(void const * arg){
    gval = 1;
    END_THREAD();
}
#include "crypto.h"
#include "heap.h"
static void _heap_thrasher(void const * arg){
    int i = 0;
    while(++i < 1000){
        size_t sz = os_rand()%256 + 1;
        int delay = os_rand()%5 + 1;
        void * test = os_malloc(sz);
        TEST_ASSERT_NOT_NULL(test);
        osDelay(delay);
        os_free(test);
    }
    gcount++;
    END_THREAD();
}
static void _mutex_waiter(void const * arg){
    TEST_ASSERT_NOT_NULL(gmutex);
    TEST_ASSERT(osOK == osMutexWait(gmutex, osWaitForever));
    gtest_mutex_counter = 0;
    TEST_ASSERT(osOK == osMutexRelease(gmutex));
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
void test_mutex(void){
    int i;
    osMutexDef_t def = {0};
    osThreadDef_t t = (osThreadDef_t){
        .name = "mutex",
        .pthread = _mutex_waiter,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 512,
    };
    gmutex = NULL;
    gtest_mutex_counter = 1;
    
    gmutex = osMutexCreate(&def);
    TEST_ASSERT_NOT_NULL(gmutex);

    //lock this thread
    TEST_ASSERT(osOK == osMutexWait(gmutex, osWaitForever));
    TEST_ASSERT_NOT_NULL(osThreadCreate(&t, 0));
    osDelay(5);
    TEST_ASSERT_EQUAL_INT(1,gtest_mutex_counter);
    TEST_ASSERT(osOK == osMutexRelease(gmutex));
    //unlock this thread
    //wait for mutex waiter to change value
    osDelay(5);
    TEST_ASSERT(osOK == osMutexWait(gmutex, osWaitForever));
    TEST_ASSERT_EQUAL_INT(0,gtest_mutex_counter);
    TEST_ASSERT(osOK == osMutexRelease(gmutex));
    TEST_ASSERT(osOK == osMutexDelete(gmutex));
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
