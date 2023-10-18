#include <stdio.h>
#include <zephyr.h>
#include <unity.h>

#define STACKSIZE 2000

struct k_thread thread1;
struct k_thread thread2;

K_THREAD_STACK_DEFINE(thread1_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACKSIZE);


void busy_busy(char* name) {
    for (int i = 0; ; i++);
}

void busy_yield(char* name) {
    for (int i = 0; ; i++) {
        if (!(i & 0xFF)) {
            k_yield();
        }
    }
}

void test_coop_same_prio_busy_busy(void) {
    k_thread_create(&thread1,
        thread1_stack,
        STACKSIZE,
        (k_thread_entry_t)busy_busy,
        NULL,
        NULL,
        NULL,
        K_PRIO_COOP(1),
        0,
        K_NO_WAIT);

    k_thread_create(&thread2,
        thread2_stack,
        STACKSIZE,
        (k_thread_entry_t)busy_busy,
        NULL,
        NULL,
        NULL,
        K_PRIO_COOP(1),
        0,
        K_NO_WAIT);

    k_sleep(K_MSEC(5));
    k_thread_abort(&thread1);
    k_thread_abort(&thread2);

    uint64_t rt_stats1;
    uint64_t rt_stats1;
}


void setUp() { }

void tearDown() { }

int main(void) {
    UNITY_BEGIN();
    RUN_TEST();
    return UNITY_END();
}