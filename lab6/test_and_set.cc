#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREAD  8
#define NUM_WORK    1000000

int g_counter;
int gap[128];
 // continous var decl makes same cache line in g_counter and object_tas
 // for performance gap is needed
int object_tas;

void lock(int *lock_object) {
    while (__sync_lock_test_and_set(lock_object, 1) == 1) {}
}

void unlock(int *lock_object) {
    *lock_object = 0;
    __sync_synchronize();
    // this func because execution time is not constant
    // store buffer - piplining and parallelism

}

void *Work(void *args) {
    for (int i = 0; i < NUM_WORK; i++) {
        lock(&object_tas);
        g_counter++;
        unlock(&object_tas);
    }
    // use schedule yield is needed to improve performance
}

int main(void) {
    pthread_t threads[NUM_THREAD];

    timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_create(&threads[i], 0, Work, 0);
    }
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], 0);
    }

    gettimeofday(&end, NULL);
    printf("time : %lld\n", (end.tv_sec * 1000000 + end.tv_usec) -
            - (start.tv_sec * 1000000 + start.tv_usec));

    printf("g_coutner: %d\n", g_counter);

    return 0;
}
