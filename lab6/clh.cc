#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <malloc.h>

#define NUM_THREAD  8
#define NUM_WORK    30000

bool *tail;
int gap[128];
bool flags[NUM_THREAD];
int g_counter = 0;

void lock(bool* flag) {
    flag = __sync_lock_test_and_set(&tail, flag); // swap address OK
    while(*flag) {sched_yield();} // wait for other
}

void unlock(bool *flag) {
    *flag = false;
    __sync_synchronize();
}

void *Work(void *args) {

    bool* local_flag = (bool*)malloc(sizeof(bool)*128);
    bool cur = false;
    int cur_index[2] = {0,64};
    for (int i = 0; i < NUM_WORK; i++) {
        local_flag[cur_index[cur]] = true;
        lock(&local_flag[cur_index[cur]]);
        g_counter++;
        //if (g_counter % 1000 == 0) 
            printf("%d\t", g_counter);
        unlock(&local_flag[cur_index[cur]]);
        cur = !cur;
    }
    // use schedule yield is needed to improve performance
}

int main(void) {
    pthread_t threads[NUM_THREAD];

    timeval start, end;
    gettimeofday(&start, NULL);

    // tail initialize
    tail = (bool*)malloc(sizeof(bool));
    *tail = false;



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
