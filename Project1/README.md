# Project1 : Shortest path

### version 
1. single thread version
2. muti thread version
3. convert data structure
4. convert algorithm 1
5. convert algorithm 2

## Single thread version
* #### Data structure

    ```
    struct Node {
        int node_num;
        set<int> dests;
        bool is_home;
        bool visited;
    }
	
    // vector search_list
    vector < set<int> > search_list;
 
    // road_map is map of all world
    map < int, struct Node > road_map;
    ```

* #### Algorithm
    BFS Search is done

    ```
    Breadth-First-Search(Graph, root):
        for each node n in Graph:            
            n.distance = INFINITY        
            n.parent = NIL
        create empty queue Q      
        root.distance = 0
        Q.enqueue(root)                      
        while Q is not empty:        
            current = Q.dequeue()
            for each node n that is adjacent to current:
                if n.distance == INFINITY:
                n.distance = current.distance + 1
                n.parent = current
                Q.enqueue(n)
                if find_home:
                    renew min and max of depth
    ```
* #### Improved thing
    because it's first version of this project, there is no improved thing.

* #### TODO
    output of this version is wrong answer, so it must be revised.

## Multi thread version
* #### Data structure

    ```
    map <int, set <int> > g_road_map;
    set<int> g_home_pos;
    ```

* #### Algorithm
    BFS Search is done. search algorithm is same as before.

    Visited vertex is added in set<int>
    
    In every home_vertex, bfs search is done.
* #### Improved thing
    This version is multi threading version, so execution time of this version is faster than before

* #### TODO
    this version's result is timeout!! so have to revise !!

## Convert Data Structure
* #### Data structure

    ```{c++}
    int g_road_map[10000001][11];
    set<int> g_is_home;

    // in BFS search function
    bool* visited = new bool[g_vertex_num+1];
    ```

* #### Algorithm
    BFS Search is done.

    Visited vertex is checked in boolean array
    
    In every home_vertex, bfs search is done.
* #### Improved thing
    This version is multi threading version, and data structure are convert set, map -> array

    So, it's var access performance is faster than before.

* #### TODO
    Performance of this version is better than before, but performance is bad so far.

## Convert Algorithm1
* #### Data structure

    ```
     int g_road_map[10000001][11];;
     set<int> g_is_home;
     int* g_is_home;
     int g_vertex_num;
     int g_home_num;
    
    // visited check array
    int *visited = new int[g_vertex_num+1];
    ```

* #### Algorithm
    BFS Search is done.

    Visited vertex is checked in int array.
    
    this array save distance, too.
    
    In every home_vertex, bfs search is done.
* #### Improved thing
        
    ```
    set<int> is_home;
         for (int i = 0; i < g_home_num; i++) {
             if ( g_is_home[i] == start_idx ) {
                 break;
             }
             is_home.insert(g_is_home[i]);
         }
    ```
    
    each thread have unoverlapped desination
        
* #### TODO
    Performance of this version is better than before. and there are more to do

## Convert Algorithm2
* #### Data structure
    
    ```
     int g_road_map[10000001][11];;
     set<int> g_is_home;
     int* g_is_home;
     int g_vertex_num;
     int g_home_num;
    
    // visited check array
    int *visited = new int[g_vertex_num+1];
    ```

* #### Algorithm
    Search Algorithm is same as before

    
* #### Improved thing

    ```
    for (not_yet_thread_top; not_yet_thread_top >= 0;) {
        if (pthread_create(&threads[not_yet_thread_top], 0, Run, (void*)(&(g_is_home[not_yet_thread_top]))) < 0) {
                 cout << "error" << endl;
                 return 1;
        }
        usable_core_num--;
        not_yet_thread_top--;
        if (usable_core_num == 0 || not_yet_thread_top < 0) {
                 break;
        }
    }
    ```
    create thread until thread_num == CORE NUM


    ```
    while (done >= 0) {
        int p = pthread_tryjoin_np(threads[done], (void**)&result);
        if (p == 0) {
            min = min < result[0] ? min : result[0];
            max = max > result[1] ? max : result[1];
            free(result);
            done--;
            if (not_yet_thread_top >= 0) {
                if (pthread_create(&threads[not_yet_thread_top], 0, Run, (void*)(&(g_is_home[not_yet_thread_top]))) < 0 ) {
                    return 1;
                }
                not_yet_thread_top--;
            }
        }
    }
    ```
    when thread is finished, another thread start!!


# Finish
    so hard but interesting homework.

