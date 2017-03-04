#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int timeout;
bool *g_order_lock;
bool gap[128];
bool *g_access_lock;
bool gap[128];
int readers = 0;


void lock()
{

}

void unlock() 
{

}

void writelock()
{
    // order lock
    // access lock
    // order unlock

}

void writeunlock()
{
    // access unlock
}

void readlock()
{

    /* readers related operation must be done with atomic instruction */
    // order lock

    if (readers == 0) {
        // access lock
    }
    readers++;
    // order unlock
}

void readunlock()
{
    /* readers related operation must be done with atomic instruction */
    readers--;
    if (readers == 0) {
        // access unlock
    }
}

void lock_init() {
    g_order_lock = (bool*)malloc(sizeof(bool));
    g_access_lock = (bool*)malloc(sizeof(bool));

    *g_order_lock = false;
    *g_access_lock = false;
}

/* in this code
 * reader lock is used in reader var access and modify
 * so reader lock is modified to atomic instruction ? 
 * thinking about this question is needed.
 * answer is no!! because ... when reader lock is called check and add is needed
 * it can be modified with test and set ???
 * check and modify - return old value so it can be done by test_and_set
 * value return is not atomic instruction .. so careful about this instruction.
 * no? hmmm. ... 
 */

void* writer(void* arg)
{
    int i = *((int*) arg);
    int counter = 0; 

    while (!timeout) {
        writelock();

        counter++;
        usleep(100);

        writeunlock();
    }
    printf("writer %d wrote %d times!\n", i, counter);

	return NULL;
}

void* reader(void* arg)
{
    int i = *((int*) arg);
    int counter = 0; 

    while (!timeout) {
        readlock();

        counter++;
        usleep(100);

        readunlock();
    }
    printf("reader %d read %d times!\n", i, counter);
	
	return NULL;
}

int main()
{
    //init
    int num_writer = 2;
    int num_reader = 10;
    printf("Test rwlock concurrency(%d writer, %d reader) started.\n", num_writer, num_reader);

    //test
    timeout = 0;
    pthread_t writer_thread[num_writer];
    int writer_index[num_writer];
    for (int i = 0; i < num_writer; i++) {
        pthread_create(&writer_thread[i], NULL, writer, (void*) &(writer_index[i]=i+1));
    }
    pthread_t reader_thread[num_reader];
    int reader_index[num_reader];
    for (int i = 0; i < num_reader; i++) {
        pthread_create(&reader_thread[i], NULL, reader, (void*) &(reader_index[i]=i+1));
    }

    usleep(5000000);
    timeout = 1;

    for (int i = 0; i < num_writer; i++) {
        pthread_join(writer_thread[i], NULL);
    }
    for (int i = 0; i < num_reader; i++) {
        pthread_join(reader_thread[i], NULL);
    }

    //assert
    printf("Test rwlock concurrency(%d writer, %d reader) passed.\n", num_writer, num_reader);
}
