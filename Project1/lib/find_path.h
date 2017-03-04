#include <set>
#include <map>

#include <pair> // ?

using namespace std;

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

pair<int, int> run(struct Node start_node) {
    
    // Q.Create
    // Q.Enqueue(start_node)

    pair<int, int> p_min_max;
    // p_min_max's init value is (-1,-1)

    // mark start as visited

    // depth = 0 - initialize

    while (/*Q's len is not 0*/) {
        
        // Q.GetFront
        // Q.Dequeue
        
        // get adjacent node_num set
        // get iter of dests, use them to make for phrase
        for (;;) {
            // get adjacent node
            // check 'this node is visited??'

            // Q.Enqueue
            // mark node as visited

            // (multi-thread env)
            // - searched-list is null ?? check !! 
            // - - if 'y' : return (-1,-1)
            // - - el 'n' : keep going

            // check this node is_home is true
            // - if 'y' : (1) renew p_min_max (2) del connection from search_list
            // - el 'n' : keep going
        }
    }
}


// profile is needed to upgrade performance
// -perf ? gprof ? o_o
