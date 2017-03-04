#include <stdio.h>
#include <pthread.h>

#define NUM_SNACK 10000000

bool g_flag[2];
int victim;
int food_eaten_num = 0;

void InitLock() {


    g_flag[0] = false;
    g_flag[1] = false;
}

void Lock(int thread_id) {
    // in case 
    // first victim
    // second flag

    // 


    g_flag[thread_id] = true;
    // memeory barriar must be here __sync_synchronize();
    victim = thread_id;    
    while (g_flag[1-thread_id] && victim == thread_id) {
        //empty
    }
}

void Unlock(int thread_id) {
    g_flag[thread_id] = false;
}

void *FeedMyPet(void* arg) {
    int i;
    int id = *((int *) arg);
    int pet_size = 0;
    for (i = 0; i < NUM_SNACK; i++) {
        Lock(id);
        pet_size++;
        food_eaten_num++;
        Unlock(id);
    }

    printf("The size of my pet is %d!\n", pet_size);
    printf("Food eaten num : %d\n", foot_eaten_num);

    return NULL;
}

int main() {
    pthread_t alice, bob;
    int alice_id = 0;
    int bob_id = 1;

    victim = alice_id;

    InitLock();

    pthread_create(&alice, NULL, FeedMyPet, (void*) &alice_id);
    pthread_create(&bob, NULL, FeedMyPet, (void*) &bob_id);

    pthread_join(alice, NULL);
    pthread_join(bob, NULL);

    printf("Finished!\n");
}
