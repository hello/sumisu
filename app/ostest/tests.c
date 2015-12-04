#include "cmsis_os.h"
#include "unity.h"
#include "io.h"
#include "sysinfo.h"
#include "util.h"

static volatile int gtest_thread_creation_val = 0;
static volatile int gtest_heap_count = 0;
static volatile int gtest_mutex_counter = 0;
static volatile int gtest_timer_counter = 0;
static volatile osMailQId gtest_mailbox;
static osMutexId gmutex = NULL;

#define TEST_MAIL_ITEM_SIZE 16
#define TEST_MAIL_SEND_COUNT 10

static void _val_thread(void const * arg){
    gtest_thread_creation_val = 1;
    END_THREAD();
}
#include "crypto.h"
#include "heap.h"
static void _heap_thrasher(void const * arg){
    int i = 0;
    while(++i < 1000){
        size_t sz = os_rand()%1024 + 1;
        int delay = os_rand()%5 + 1;
        void * test = os_malloc(sz);
        TEST_ASSERT_NOT_NULL(test);
        osDelay(delay);
        os_free(test);
    }
    gtest_heap_count++;
    END_THREAD();
}
static void _mutex_waiter(void const * arg){
    TEST_ASSERT_NOT_NULL(gmutex);
    TEST_ASSERT(osOK != osMutexWait(gmutex, 0));
    TEST_ASSERT(osOK == osMutexWait(gmutex, osWaitForever));
    gtest_mutex_counter = 0;
    TEST_ASSERT(osOK == osMutexRelease(gmutex));
    END_THREAD();
}
static void _test_timer(void const * arg){
    ++gtest_timer_counter;
}
static void _mail_sender(void const * arg){
    int i;
    void * a;
    for(i = 0; i < TEST_MAIL_SEND_COUNT; i++){
        a = osMailCAlloc (gtest_mailbox, osWaitForever);
        TEST_ASSERT_NOT_NULL(a);
        *(uint8_t*)a = i;
        TEST_ASSERT(osOK == osMailPut(gtest_mailbox, a));
    }
    END_THREAD();
}
//tests
void test_thread_creation(void){
    size_t orig_heap = os_free_heap_size();
    osThreadDef_t t2 = (osThreadDef_t){
        .name = "runner",
        .pthread = _val_thread,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    osThreadCreate(&t2, 0);
    osDelay(10);
    TEST_ASSERT_EQUAL(1, gtest_thread_creation_val);
    osDelay(10);
    TEST_ASSERT_EQUAL_INT(os_free_heap_size(), orig_heap);
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
    osDelay(100);
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
    gtest_heap_count = 0;
    size_t orig_heap = os_free_heap_size();
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
    while(gtest_heap_count < 3){
        osDelay(10);
    }
    size_t thrashed_heap = os_free_heap_size();
    TEST_ASSERT_EQUAL_INT(thrashed_heap, orig_heap);
}

void test_delay(void){
    uint64_t t0 = uptime();
    osDelay(1000);
    uint64_t t1 = uptime();
    TEST_ASSERT_INT32_WITHIN(5, 1000, (uint32_t)t1-t0);
}
void test_timer(void){
#define TEST_TIMER_WAIT_TIME 1000
    osTimerDef_t def = (struct os_timer_def){
        .ptimer = _test_timer,
    };
    size_t orig_heap = os_free_heap_size();
    osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument);
    osTimerId timer = osTimerCreate(&def, osTimerPeriodic, NULL);
    TEST_ASSERT_NOT_NULL(timer);
    gtest_timer_counter = 0;
    uint64_t t0 = uptime();
    TEST_ASSERT(osOK == osTimerStart(timer,1));
    osDelay(TEST_TIMER_WAIT_TIME);
    TEST_ASSERT(osOK == osTimerStop(timer));
    uint64_t t1 = uptime();
    TEST_ASSERT_INT32_WITHIN(1, t1-t0, gtest_timer_counter);
    TEST_ASSERT_INT32_WITHIN(1, TEST_TIMER_WAIT_TIME, gtest_timer_counter);
    TEST_ASSERT(osOK == osTimerDelete(timer));
    osDelay(10);
    TEST_ASSERT_EQUAL_INT(os_free_heap_size(), orig_heap);
}
void test_mail(void){
    int i;
    void *a, *b, *c;
    osStatus stat;
    osMailQDef_t def = (osMailQDef_t){
        .queue_sz = 2,    ///< number of elements in the queue
        .item_sz = TEST_MAIL_ITEM_SIZE,    ///< size of an item
    };
    gtest_mailbox = osMailCreate(&def, NULL);
    TEST_ASSERT_NOT_NULL(gtest_mailbox);
    for(i = 0; i < 2; i++){
        a = osMailCAlloc (gtest_mailbox, 10);
        b = osMailCAlloc (gtest_mailbox, 10);
        c = osMailCAlloc (gtest_mailbox, 10);
        TEST_ASSERT_NOT_NULL(a);
        TEST_ASSERT_NOT_NULL(b);
        TEST_ASSERT_NULL(c);
        stat = osMailFree(gtest_mailbox, a);
        TEST_ASSERT_EQUAL(osOK,stat);
        stat = osMailFree(gtest_mailbox, b);
        TEST_ASSERT_EQUAL(osOK,stat);
    }
    osThreadDef_t t = (osThreadDef_t){
        .name = "sender",
        .pthread = _mail_sender,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 1024,
    };
    osThreadCreate(&t, 0);
    for(i = 0; i < TEST_MAIL_SEND_COUNT; i++){
        osEvent evt = osMailGet(gtest_mailbox, osWaitForever);
        TEST_ASSERT(evt.status == osEventMail);
        TEST_ASSERT_NOT_NULL(evt.value.p);
        TEST_ASSERT_EQUAL(gtest_mailbox, evt.def.mail_id);
        TEST_ASSERT_EQUAL_INT(*(uint8_t*)evt.value.p, i);
        TEST_ASSERT(osOK == osMailFree(gtest_mailbox,evt.value.p));
    }
    osDelay(100);
}
#include "pubsub.h"
static const char testmsg[] = "hello\r\n";
static volatile uint32_t thrash_count = 0;
#define TEST_PS_COUNT 10000
static void _on_ps_message(const void * message, size_t sz){
    TEST_ASSERT_EQUAL_STRING_MESSAGE(testmsg, message, "message not match\r\n");
    TEST_ASSERT_EQUAL( osOK, ps_publish(PS_UART0_RX, message, sz));
}
static void _on_load(const void * message, size_t sz){
    TEST_ASSERT_EQUAL_STRING_MESSAGE(testmsg, message, "message not match\r\n");
    thrash_count++;
}
static void _ps_sender(const void * arg){
    int i = 0;
    osStatus rc;
    ps_message_t * out_msg = NULL;

    while( i++ < TEST_PS_COUNT ){
        out_msg = NULL;
        rc = ps_publish_timeout(PS_UART0_RX, testmsg, sizeof(testmsg), osWaitForever);
        TEST_ASSERT(  osOK == rc );

    }
    END_THREAD();
}
static void _ps_thrasher(const void * arg){
    int i = 0;
    osStatus rc;
    while( i++ < TEST_PS_COUNT ){
        rc = ps_publish_timeout(PS_TEST_LOAD, testmsg, sizeof(testmsg), 0);
        TEST_ASSERT(  osOK == rc || osErrorTimeoutResource );
    }
    END_THREAD();
}
void test_ps(void){
    ps_channel_t * rx = NULL;
    ps_message_t * out_msg = NULL;
    int i = 0;
    size_t orig_heap;
    TEST_ASSERT_EQUAL(osOK, ps_init());
    //first test for async mode
    TEST_ASSERT_NOT_NULL( (rx = ps_subscribe(PS_UART0_RX, NULL)) );
    TEST_ASSERT_NULL( ps_subscribe(PS_UART0_TX, _on_ps_message) );
    TEST_ASSERT_NULL( ps_subscribe(PS_TEST_LOAD, _on_load) );

    /*
     *orig_heap = os_free_heap_size();
     */

    TEST_ASSERT_EQUAL( osOK, ps_publish(PS_UART0_TX, testmsg, sizeof(testmsg)));
    TEST_ASSERT_EQUAL( osOK, ps_listen(rx, &out_msg, osWaitForever) );
    TEST_ASSERT_EQUAL_STRING_MESSAGE(testmsg, out_msg->data, "message not match\r\n");
    TEST_ASSERT_EQUAL( osOK, ps_free_message(out_msg) );

    osDelay(100);

    /*
     *TEST_ASSERT_EQUAL_INT(orig_heap, os_free_heap_size());
     */
    
test_sync:
    //now test for synchronous mode both ways
    orig_heap = os_free_heap_size();
    osThreadDef_t t = (osThreadDef_t){
        .name = "sender",
        .pthread = _ps_sender,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 1024,
    };
    osThreadCreate(&t, NULL);
    size_t counter = 0;
    i = 0;
    uint64_t t0 = uptime();
    while( i++ < TEST_PS_COUNT ){
        TEST_ASSERT_EQUAL( osOK, ps_listen(rx, &out_msg, osWaitForever) );
        TEST_ASSERT_EQUAL( osOK, ps_free_message(out_msg) );
    }
    osDelay(100);//allow time for idle thread to free heap
    TEST_ASSERT_EQUAL_INT(orig_heap, os_free_heap_size());
    uint64_t t1 = uptime();
    LOGT( "ps synchronous throughput %u packets/s\r\n", i * 1000 / (t1-t0) );

test_async:
    //lastly for stress testing
    orig_heap = os_free_heap_size();
    osThreadDef_t t2 = (osThreadDef_t){
        .name = "thrasher",
        .pthread = _ps_thrasher,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 1024,
    };
    osThreadCreate(&t2, NULL);
    osThreadCreate(&t2, NULL);
    osThreadCreate(&t2, NULL);
    osThreadCreate(&t2, NULL);
    osThreadCreate(&t2, NULL);
    t0 = uptime();
    osDelay(5000);
    t1 = uptime();
test_exit:
    LOGT( "ps asynchronous throughput %u packets/s\r\n", thrash_count * 1000 /(t1-t0));
}
