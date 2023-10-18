#include <stdio.h>
#include <zephyr.h>
#include <unity.h>

#define STACKSIZE 2000

struct k_thread thread1;
struct k_thread thread2;
struct k_thread super_thread;

K_THREAD_STACK_DEFINE(thread1_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(super_thread_stack, STACKSIZE);

void super_entry(){
    printf("Suspending threads...\n");
    k_thread_suspend(&thread1);
    k_thread_suspend(&thread2);
}


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

void run_threads(
    char* test_name,
    k_thread_entry_t t1_entry, int t1_prio,
    k_thread_entry_t t2_entry, int t2_prio
) {
    k_thread_create(&super_thread,
        super_thread_stack,
        STACKSIZE,
        (k_thread_entry_t)super_entry,
        NULL,
        NULL,
        NULL,
        -CONFIG_NUM_COOP_PRIORITIES,
        0,
        K_MSEC(50)
    );

    k_thread_create(&thread1,
        thread1_stack,
        STACKSIZE,
        (k_thread_entry_t)t1_entry,
        NULL,
        NULL,
        NULL,
        t1_prio,
        0,
        K_MSEC(1)
    );

    k_thread_create(&thread2,
        thread2_stack,
        STACKSIZE,
        (k_thread_entry_t)t2_entry,
        NULL,
        NULL,
        NULL,
        t2_prio,
        0,
        K_MSEC(2)
    );

    k_thread_join(&super_thread, K_MSEC(300));

    k_thread_runtime_stats_t rt_stats1;
    k_thread_runtime_stats_get(&thread1, &rt_stats1);
    k_thread_runtime_stats_t rt_stats2;
    k_thread_runtime_stats_get(&thread2, &rt_stats2);

    printk("%s: t1 %lld t2 %lld\n",
        test_name,
        timing_cycles_to_ns(rt_stats1.execution_cycles),
        timing_cycles_to_ns(rt_stats2.execution_cycles)
    );

    k_thread_abort(&thread1);
    k_thread_abort(&thread2);
}

void test_coop_same_prio(void) {
    run_threads(
        "coop_same_prio_busy_busy",
        busy_busy, K_PRIO_COOP(1),
        busy_busy, K_PRIO_COOP(1)
    );

    run_threads(
        "coop_same_prio_busy_yield",
        busy_yield, K_PRIO_COOP(1),
        busy_yield, K_PRIO_COOP(1)
    ); 
}

void test_preempt_same_prio(void) {
    run_threads(
        "preempt_same_prio_busy_busy",
        busy_busy, K_PRIO_PREEMPT(1),
        busy_busy, K_PRIO_PREEMPT(1)
    );

    run_threads(
        "preempt_same_prio_busy_yield",
        busy_yield, K_PRIO_PREEMPT(1),
        busy_yield, K_PRIO_PREEMPT(1)
    ); 
}

void test_coop_diff_prio(void) {
    run_threads(
        "coop_diff_prio_busy_busy_high_first",
        busy_busy, K_PRIO_COOP(1),
        busy_busy, K_PRIO_COOP(2)
    );
    
    run_threads(
        "coop_diff_prio_busy_busy_low_first",
        busy_busy, K_PRIO_COOP(2),
        busy_busy, K_PRIO_COOP(1)
    );
    
    run_threads(
        "coop_diff_prio_busy_yield_high_first",
        busy_yield, K_PRIO_COOP(1),
        busy_yield, K_PRIO_COOP(2)
    );

    run_threads(
        "coop_diff_prio_busy_yield_low_first",
        busy_yield, K_PRIO_COOP(2),
        busy_yield, K_PRIO_COOP(1)
    );
}

void test_preempt_diff_prio(void) {
    run_threads(
        "PREEMPT_diff_prio_busy_busy_high_first",
        busy_busy, K_PRIO_PREEMPT(1),
        busy_busy, K_PRIO_PREEMPT(2)
    );
    
    run_threads(
        "PREEMPT_diff_prio_busy_busy_low_first",
        busy_busy, K_PRIO_PREEMPT(2),
        busy_busy, K_PRIO_PREEMPT(1)
    );
    
    run_threads(
        "PREEMPT_diff_prio_busy_yield_high_first",
        busy_yield, K_PRIO_PREEMPT(1),
        busy_yield, K_PRIO_PREEMPT(2)
    );

    run_threads(
        "PREEMPT_diff_prio_busy_yield_low_first",
        busy_yield, K_PRIO_PREEMPT(2),
        busy_yield, K_PRIO_PREEMPT(1)
    );
}




void setUp() { }

void tearDown() { }

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_coop_same_prio);
    RUN_TEST(test_preempt_same_prio);
    RUN_TEST(test_coop_diff_prio);
    RUN_TEST(test_preempt_diff_prio);
    return UNITY_END();
}