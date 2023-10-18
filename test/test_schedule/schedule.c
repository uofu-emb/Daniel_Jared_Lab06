#include <stdio.h>
#include <zephyr.h>
#include <unity.h>

#define STACKSIZE 2000

struct k_thread thread1;
struct k_thread thread2;

K_THREAD_STACK_DEFINE(thread1_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACKSIZE);


int thread1_count = 0;
int thread2_count = 0;

struct k_sem sem;

void thread1_entry(int* count) {
    k_sem_take(&sem, K_FOREVER);
    printf("Hello from thread 1\n");
    printf("1's count: %d\n", *count);
    (*count)++;
    printf("1's count (post): %d\n", *count);
    while (1) { }
}

void thread2_entry(int* count) {
    k_sem_take(&sem, K_FOREVER);
    printf("Hello from thread 2\n");
    printf("2's count: %d\n", *count);
    (*count)++;
    printf("2's count (post): %d\n", *count);
    while (1) { }
}

void test_bad_schedule(void) {
    k_sem_init(&sem, 1, 1);


    k_thread_create(&thread1,
        thread1_stack,
        STACKSIZE,
        (k_thread_entry_t)thread1_entry,
        &thread1_count,
        NULL,
        NULL,
        K_PRIO_PREEMPT(1),
        0,
        K_MSEC(1));

    k_thread_create(&thread2,
        thread2_stack,
        STACKSIZE,
        (k_thread_entry_t)thread2_entry,
        &thread2_count,
        NULL,
        NULL,
        K_PRIO_PREEMPT(2),
        0,
        K_NO_WAIT);


    k_sleep(K_MSEC(5));

    TEST_ASSERT_EQUAL(0, thread1_count);
    TEST_ASSERT_EQUAL(1, thread2_count);

}


void setUp() { }

void tearDown() { }

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_bad_schedule);
    return UNITY_END();
}