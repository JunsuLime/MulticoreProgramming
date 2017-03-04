/**
 *  this file is main file of this Project (shortest path)
 *
 *  @author Junsu Lim
 *  @since 2016-09-09
 */

#define CORE_NUM 36
#define MAX_VERTEX 10000000
#define MAX_EDGE 10

#include <stdio.h>
#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include <array>

#include <array>

#include <string.h>


using namespace std;


int g_road_map[MAX_VERTEX+1][MAX_EDGE+1];;
int* g_is_home;
int g_vertex_num;   // to make visited bool array
int g_home_num;

void ReadInput();
void* Run (void* start_idx);


/*
 * this function is used to run this project, finding shortest path
 */

int main(void) {

    ReadInput();
    
    pthread_t threads[g_home_num];
    
    int min = 1000000;
    int max = -1;

    int usable_core_num = CORE_NUM - 1;  // except main thread ( CORE_NUM - 1 )
    int not_yet_thread_top = g_home_num - 1;
    int done_thread = 0;
    
    for (not_yet_thread_top; not_yet_thread_top >= 0;) {
        if (pthread_create(&threads[not_yet_thread_top], 0, Run, (void*)(&(g_is_home[not_yet_thread_top]))) < 0) {
            cout << "error" << endl;
            return 1;
        }
        //cout << "thread(" << not_yet_thread_top << ") is running" <<endl;

        // make program -> run thread num == core
        usable_core_num--;
        not_yet_thread_top--;
        if (usable_core_num == 0 || not_yet_thread_top < 0) {
            break;
        }
    }

    //cout << " hello world !!\n" << endl;
    
    //cout << " craete is done !!\n";
    //cout << " not_yet_thread_top " << not_yet_thread_top << endl;

    // ret index must be changed in every iter
    int *result;
    int done = g_home_num-1;
    while (done >= 0) {
        
        int p = pthread_tryjoin_np(threads[done], (void**)&result);
        //if (pthread_tryjoin_np(threads[thread_ret_iter], (void**)&result) == 0) {
        if (p == 0) {
            // if return 0 success to join

            min = min < result[0] ? min : result[0];
            max = max > result[1] ? max : result[1];

            free(result);

            //cout << "thread(" << done << ") is done" << endl;
            done--;

            if (not_yet_thread_top >= 0) {
                if (pthread_create(&threads[not_yet_thread_top], 0, Run, (void*)(&(g_is_home[not_yet_thread_top]))) < 0 ) {
                    cout << "error" <<endl;
                    return 1;
                }
                //cout << "thread(" << not_yet_thread_top << endl;
                not_yet_thread_top--;
            }
        }
    }

    cout << min - 1 << endl;
    cout << max - 1 << endl;

    return 0;
}

/*
 * this function is used to receive input and
 * save data to my data structure
 */

void ReadInput()
{
    
    int idx;

    int line_num;

    int edge_num, arrival_idx;

    ios::sync_with_stdio(false);
    
    //scanf("%d", &g_home_num);
    cin >> g_home_num;
    g_is_home = (int*)malloc(sizeof(int)*g_home_num);

    for (int i=0; i < g_home_num; i++) {
        //scanf("%d", &idx);
        cin >> idx;
        g_is_home[i] = idx;
    }

    //scanf("%d", &g_vertex_num);
    cin >> g_vertex_num;

    memset(g_road_map, 0, sizeof(g_road_map));

    //scanf("%d", &line_num);
    cin >> line_num;

    for (int i=0; i <line_num; i++) {
        //scanf("%d %d", &idx, &edge_num);
        cin >> idx >> edge_num;
        for (int j=0; j <edge_num; j++) {
            //scanf("%d", &arrival_idx);
            cin >> arrival_idx;
            g_road_map[idx][++g_road_map[idx][0]] = arrival_idx;
            g_road_map[arrival_idx][++g_road_map[arrival_idx][0]] = idx;
        }
    }
    /*
    for (int i=0 ; i< g_vertex_num+1; i++) {
        for (int j=0; j < 11; j++)
        {
            cout << g_road_map[i][j] << " ";
        }
        cout << endl;
    }
    */
    
}

/*
 * this function is used to run BFS
 * start from start_idx, this func run BFS and search for home
 * if it arrive at home check depth and renew min and max
 *
 * @param [ void* start_idx ]   set start index
 * @return  return value that arr[2] = { min, max }
 */
void* Run(void* idx) {

    // recv start_idx
    int start_idx = *(int*)(idx);
   
    //cout << "thread : " << start_idx << endl;

    // create visited home set
    
    int* min_max = (int*)malloc(sizeof(int)*2);

    min_max[0] = 10000;
    min_max[1] = -1;
 

    int next_node; 
   

    // create visited home set
    int *visited = new int[g_vertex_num+1];
    memset(visited, 0, sizeof(int)*(g_vertex_num+1));

    set<int> is_home;
    for (int i = g_home_num-1; i >= 0; i--) {
        if ( g_is_home[i] == start_idx ) {
            break;
        }
        is_home.insert(g_is_home[i]);
        //cout << g_is_home[i] <<" ";
    }
    //cout << endl;

    int home_num = is_home.size();
    
    if (home_num == 0)
        return (void*)min_max;

    // Q.Create
    queue <int> q;
   
    // Q.Enqueue(start_node)
    q.push(start_idx);
    // mark start as visited
    visited[start_idx] = 1;

    //cout << " visit node : " << start_idx;
    //cout << " depth : " << visited[start_idx] <<endl;
    //cout << " depth_div : " << depth_div << endl;

    while (!q.empty()) {
        
        // Q.GetFront
        next_node = q.front();
       
        // Q.Dequeue
        q.pop();
        // get adjacent node_num set
        // get iter of dests, use them to make for phrase
        int adj_vertex_num = g_road_map[next_node][0];
        //cout << next_node << "'s adjacent node num : " << adj_vertex_num << endl;

        for (int i=1; i <= adj_vertex_num ; i++) {

            // get adjacent node idx : v_idx 
            int v_idx = g_road_map[next_node][i];

            if (visited[v_idx]) {
                // perf - bad
                //cout << "already visit vertex : " << v_idx << endl;
                continue;
            }

            // Q.Enqueue
            q.push(v_idx);
            // renew depth

            
            visited[v_idx] = visited[next_node]+1;
            //cout << " visit node : " << v_idx;
            //cout << " depth : " << visited[v_idx]<<endl;;
            //cout << " depth_div : " << depth_div << endl;
            

            // TODO: implement overlap searching checking var

            // (multi-thread env)
            // - searched-list is null ?? check !! 
            // - - if 'y' : return (-1,-1)
            // - - el 'n' : keep going

            // check this node is_home is true
            // - if 'y' : (1) renew p_min_max (2) del connection from search_list
            // - el 'n' : keep going

            if (is_home.find(v_idx) != is_home.end()) {

                //cout << "visit home !!" << endl;
                home_num--;
                //cout << "home num : " << home_num << endl;
                // min check
                if (min_max[0] ==10000) {
                    min_max[0] = visited[v_idx];
                    //cout << "renewed value (min) : " << depth << endl;
                }
                min_max[1] = visited[v_idx];
                //cout << "renewed value (max) : " << depth << endl;
                if (home_num == 0) {
                    min_max[1] = visited[v_idx];
                    delete visited;
                    return (void*)min_max;
                }
            }
        }
    }
    return (void*)min_max;
}
