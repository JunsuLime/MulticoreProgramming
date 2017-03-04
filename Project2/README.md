# Project2 : Simple MVCC

---
---

* Compile environment and how to run program
    - Compile environment
    - How to run
* Implemenation explaination
    - Design and structure
    - About lock
    - Correctness
    - Garbage collecting

---

### Compile environment and how to run program
1) Compile environment
    
    * /bin - update_machine (executable file)
    * /src - main.cpp, bakery.cpp
    * /include - singly_linked_list.h, bakery.h
    
    Makefile
    ```
    CC = g++
    CXXFLAGS = -g -Wall -std=c++11 -Os

    # Macros specifying path for compile.
    TARGET = update_machine

    SRCS := $(wildcard src/*.cpp)
    OBJS := $(SRCS:.cpp=.o)
    BIN = ./bin/
    INC = ./include/
    SRC = ./src/
    LIB = ./lib/ -lpthread

    # pre-processor.
    CPPFLAGS += -I$(INC) -lpthread

    # compile command
    TARGET: $(OBJS)
	    $(CC) $(CXXFLAGS) -o  $(BIN)$(TARGET) $(OBJS) -L$(LIB)

    # Delete binary & object files.
    clean:
	    rm $(BIN)$(TARGET) $(OBJS)
    ```
    (singly_linked_list is generic type class, so it has only header file)
    

2) How to run program

    ```
    ./bin/update_machine (thread_num) (duration) (verbose- optional)
    ```
---    
    
### Implementation explaination

1) Design and structure
    
    #### section is seperated by 3 parts
    
    ###### insert section
    : insert thread info in global_active_thread_list and capture active_thread_list's status
    ###### update section
    : update current thread's value
    ###### delete section
    : insert version data in version_data linkedlist and delete thread from active_thread_list
    
    - ###### insert section
        When enter this section, we get lock(). In this time, we can guarantee current version is lastest. and through current active_thread_list's status, we can make read_view
    - ###### update section
        make read_view and update current version's value.
    - ###### delete section
        insert version value in version_controller (sinlgy linked list) and delete thread from active_thread_list

2) About lock

    * pthread_mutex
    * Bakery lock
    
    code in  main.cpp
    
    ```
    #ifdef BAKERY_MUTEX_LOCK
        #define lock(x) bakery->lock(x)
        #define unlock(x) bakery->unlock(x)
        #define BAKERY true
    #else
        #define lock(x) pthread_mutex_lock(&g_mutex)
        #define unlock(x) pthread_mutex_unlock(&g_mutex)
        #define BAKERY false
    #endif
    ```
    (lock setting is done by define code)
    : if lock is pthread_mutex, lock(thread_idx) -> pthread_mutex_lock(&g_mutex);
    : else lock(thread_idx) -> bakery->lock(thread_idx);
    
    code in bakery.cpp
    
    ```
    bool Bakery::check(int i) {
    // for every i
        for ( int j = 0 ; j < consumer_num ; j++) {
            // flag i == true
            if (flag[j] == true) {
                // and lexicographic order is higher than other
                if (label[i] > label[j]) {

                    __sync_synchronize();
                    return true;
                }
                else if (label[i] == label[j]) {
                    if (i > j) {

                        __sync_synchronize();
                        return true;
                    }
                }
            }
        }
        return false;
    }
    ```
    (memory barrier __sync_synchronize() is needed in bakery implementation)
    : so many thread access to shared var, so memory barrier is needed.
    

3) Correctness
    
    In this project, we can prove correctness.
    
    insert section
    : We can get updating thread and current lastest version. use this information, we make read_view.
    
    update section
    : Read_view made by active_thread_list information defends correctness of this program. Because read_view data is not updating and it's version doesn't overwhelming this thread's update version
    
    delete section
    : After updating, we go delete section and get lock. insert updated version data and delete thread from active_thread_list. So, program cannot corrupt it's correctness.
    
        
4) Garbage collecting

    example )

     gc  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
    ------------|- | -| -|-|-|-|-|-|
    thread 0    | 0 | 1 | 1 | 3|3|3|6|6|
    thread 1    |0|0|2|2|2|5|5|5|
    thread 2    |0|0|0|0|4|4|4|7|
    
    and active_thread_list is   5-6-7.
    
    then, it's unusable version is 
    * in thread 0 - 0,1
    * in thread 1 - 0,2
    * in thread 2 - 0
    
    and this can be deriven by read_view versions in oledst active_thread_list.
    
    in that case, 5's read_view version is (0,3) (1,5) (2,4) , (thread_idx, version)
    
    and unused version is
        * in thread 0 - 0,1
        * in thread 1 - 0
        * in thread 2 - 0

    oldest active_thread's read_view version is collected by this code
    
    ```
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
    ```

    in garbage collector, read that information and do garbage collect
    
    
    
    
    

