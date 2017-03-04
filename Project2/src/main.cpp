/**
 * this file is main file for project2 (MVCC)
 *
 * @author  Junsu Lim
 * @since   2016-10-24
 */

#include <fstream>
#include <iostream>
#include <malloc.h>
#include <string.h>
#include "singly_linked_list.h"
#include "bakery.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define INIT_VERSION 0
#define DEFAULT_THREAD_NUM 10

#define DEFAULT_LOCK true
//#define BAKERY_MUTEX_LOCK true

#ifdef BAKERY_MUTEX_LOCK
    #define lock(x) bakery->lock(x)
    #define unlock(x) bakery->unlock(x)
    #define BAKERY true
#else
    #define lock(x) pthread_mutex_lock(&g_mutex)
    #define unlock(x) pthread_mutex_unlock(&g_mutex)
    #define BAKERY false
#endif

using namespace std;

void *Run(void* idx);
void *GarbageCollector(void *);
void InitList(int *list, int list_size, int value);
void printCurrentVersion();


/* about project explaination
 var : 
global_active_thread_list
global_version_controller [THREAD_NUM]
0 - v0 v2 v7
1 - v1 v3
2 - v4 v5 v6
..
..

*/

/* declaration of global variable */
SinglyLinkedList<bool> g_active_thread_list;
SinglyLinkedList< pair <int, int> > *g_version_controller;

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
Bakery *bakery;

int g_execution_order = INIT_VERSION;

int g_thread_num = DEFAULT_THREAD_NUM;

bool g_verbose_opt = false;

int* g_read_view_list;

int *g_throughput;

ofstream *g_verbose;

/*
 * main function
 * construct updating machine
 * 
 * in concurretncy control
 *
 * @return  0- success, 1- failure in create_thread
 */

int main(int argc, char** argv) {
    
    // by argument receive thread_num, duration, (optional) verbose
    
    // var : thread_num, duration
    // int thread_num; -> thread_num is declared as global var
    int duration;

    if (argc >= 3 && argc <= 4) {
        g_thread_num = stoi(argv[1]);
        duration = stoi(argv[2]);
        cout << "thread num : " << g_thread_num << endl;
        cout << "duration : " << duration << endl;
        if (argc == 4) {
            g_verbose_opt = true;
            cout << "verbose option : ON" << endl;
        }
    }
    else {
        cout << "argument is wrong\n[usage]\n./update_machine (thread_num) (duration) (verbose- optional)" << endl;
        return 1;
    }
    cout << "lock implementation : Bakery lock " << BAKERY << endl;
    // argv[1] - thread num, argv[2] - duration, argv[3] - (optional) verbose

        
    // init global_linked_list, thread_idx list
    g_version_controller = new SinglyLinkedList< pair<int, int> >[g_thread_num];
    pthread_t threads[g_thread_num];
    g_throughput = new int[g_thread_num];
    g_read_view_list = new int[g_thread_num];
    bakery = new Bakery(g_thread_num+1);
    g_verbose = new ofstream[g_thread_num];

    if (g_verbose_opt) {
        for (int i = 0; i < g_thread_num; i++) {
            char tmp[100] = {'\0'};
            sprintf(tmp, "verbose_thread_%d.txt", i);
            g_verbose[i].open(tmp, ios::out);
        }
    }

    // In thread creating .. / value overlapped problem
    int *thread_idx_list = new int[g_thread_num];
    for (int i = 0; i < g_thread_num ; i++) {
        thread_idx_list[i] = i;
    }

    // init - init value of thread's pair value
    for (int i = 0; i < g_thread_num ; i++) {
        int rand_val0 = rand() % 100;
        int rand_val1 = rand() % 100;
        g_version_controller[i].Insert(/*random value*/ make_pair(rand_val0, rand_val1), INIT_VERSION);
    }

    printCurrentVersion();

    for (int i=0; i < g_thread_num; i++) {
        if ( pthread_create(&threads[i], 0, Run, (void*)(&thread_idx_list[i])) < 0 ) {
            cout << "error" << endl;
            return 1;
        }
    }
    
    
    // set garbage collector
    pthread_t garbage_collector_thread;
    // create garbage collect thread 
    if ( pthread_create(&garbage_collector_thread, 0, GarbageCollector, NULL)) {
        cout << "error" << endl;
        return 1;
    }

    
    // if verbose opt is set
    for (int i = 0 ; i < duration * 10 ; i++) {
        usleep(100*1000);
        /*
        if (g_verbose_opt) {
            cout << "second : " << i << endl;
            printCurrentVersion(); 
        }*/
    }

    // ref conditional variable in c++
    // instead of using signal ...
    lock(g_thread_num);
    for (int i = 0; i < g_thread_num; i++) {
        pthread_cancel(threads[i]);
    }
    unlock(g_thread_num);
    
    printCurrentVersion();


    // calculate throughput and fairness
    float tmp_u = 0;
    float tmp_d = 0;
    float sum = 0;

    for (int i = 0; i < g_thread_num; i++) {
        //cout << i << " thread's execution : " << g_throughput[i] << endl;
        tmp_u += g_throughput[i];
        tmp_d += g_throughput[i]*g_throughput[i];
        sum += g_throughput[i];
    }

    cout << endl << "all thread's throughput : " << sum / (float)duration << endl;

    cout << "fairness : " << tmp_u * tmp_u / ( g_thread_num * tmp_d ) << endl;

    
}


/*
 * function Run ( Update machine )
 * 
 * this is a thread function.
 * each thread run one transaction and after do the job,
 * we get new version of value.
 *
 * @param[in]   idx : thread_idx
 */

void *Run(void *idx) {

    int thread_idx = *(int*)(idx);

    int read_view[g_thread_num][2];
    int updating_list[g_thread_num];

    while(1) {
        InitList(updating_list, g_thread_num, -1);

        /* atomic process */
        lock(thread_idx);
        g_execution_order++;

        int cur_order = g_execution_order;
        // function implementation is not yet
        //g_active_thread_list.InsertAtomic(true, g_execution_order);
        g_active_thread_list.Insert(true, cur_order);
        g_active_thread_list.GetPrevVersions(updating_list);

        unlock(thread_idx);
        /******************/

        // make read view
        // when make read view, careful about updating thread_list
        // TODO: updating thread ......
        int *read_view_version = new int[g_thread_num];
        for (int i = 0; i < g_thread_num; i++) {
            int r_ver = cur_order;
            lock(thread_idx);
            pair<int, int> p_value = g_version_controller[i].MakeReadView(&r_ver, updating_list, g_thread_num);
            unlock(thread_idx);
            read_view_version[i] = r_ver;
            read_view[i][0] = p_value.first;
            read_view[i][1] = p_value.second;
        }


        // pick random i
        int r_idx = rand()%g_thread_num;

        if (r_idx == thread_idx) {
            r_idx = (r_idx+1)%g_thread_num;
        }

        // update my version
        pair<int, int> cur_val = g_version_controller[thread_idx].FindCurrentVersion();
        cur_val.first = cur_val.first + (read_view[r_idx][0])%100;
        cur_val.second = cur_val.second - (read_view[r_idx][0])%100;

        if (g_verbose_opt) {
            for (int i = 0; i < g_thread_num; i++) {
                if (updating_list[i] == -1) {
                    g_verbose[thread_idx] << i << endl;
                    break;
                }
            }
            for (int i = 0; i < g_thread_num; i++) {
                if (updating_list[i] == -1) {
                   break; 
                }
                g_verbose[thread_idx] << updating_list[i] << " ";
            }
            g_verbose[thread_idx] << endl;
            g_verbose[thread_idx] << read_view_version[r_idx] << endl;
        }


        g_throughput[thread_idx]++;
        
        bool set_oldest_read_view = false;
        /* atomic process */
        lock(thread_idx);
        set_oldest_read_view = g_active_thread_list.IsFirst(cur_order);

        g_version_controller[thread_idx].Insert(cur_val, cur_order);
        // function implementation is not yet
        g_active_thread_list.DeleteByVersion(cur_order);
        unlock(thread_idx);
        /******************/

        if (set_oldest_read_view) {
            for (int i = 0; i < g_thread_num; i++) {
                g_read_view_list[i] = read_view_version[i];
            }
        }
    }
}


/*
 * this fucntion is used to garbage collecting
 *
 */
void *GarbageCollector(void* d) {
    while (1) {
        // garbage collecting
        for (int i = 0; i < g_thread_num; i++) {
            lock(g_thread_num);
            g_version_controller[i].GarbageCollect(g_read_view_list[i]);
            unlock(g_thread_num);
            //g_version_controller[i].PrintAll();
            //cout << g_read_view_list[i] << endl;
            //g_version_controller[i].PrintAll();
        }
    }
    
}

/*
 * this function is used to initialize with argument
 * to check active_thread_list's updating version. 
 *
 * @param[in,out]   list : this list will be inited.
 * @param[in]       list_size : length of list
 * @param[in]       value : list's inited value
 */

void InitList(int *list, int list_size, int value) {
    for (int i = 0; i < list_size; i++) {
        list[i] = value;
    }
}

/*
 * this function is used to print each thread's currentVersion's value
 * 
 */

void printCurrentVersion() {
    int sum = 0;
    for (int j = 0 ; j < g_thread_num ; j++) {
        pair<int, int> tmp = g_version_controller[j].FindCurrentVersion();
        /*
        cout << "A : " << tmp.first << " B : " << tmp.second << " A+B : " << tmp.first+tmp.second << " version : " << g_version_controller[j].FindCurrentV() << endl;
        */
        sum += tmp.first + tmp.second;
    }
    cout << "\nversion value's sum : " << sum << endl;
}
