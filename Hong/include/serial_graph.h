#ifndef __SERIAL__GRAPH__HEADER__
#define __SERIAL__GRAPH__HEADER__

#include<bits/stdc++.h>
using namespace std;

// macros declared 

#define vi vector<long>
#define pb push_back
#define pii pair<long,long>
#define mp make_pair
#define ff first
#define ss second

// Classes, structures and function prototypes for serial implementation

// Structure representing a Vertex
struct Vertex
{
    long height; // height of node
    long ex_flow; // excess flow 

    // constructor function
    Vertex(long h, long e)
    {
        this->height = h;
        this->ex_flow = e;
    }
};

// Structure representing an Edge
struct Edge_s
{
    long u,v; // edge from node u to node v
    long flow; // current flow
    long capacity; // capacity of edge

    // constructor function 
    Edge_s(long f, long c, long a, long b)
    {
        this->u = a;
        this->v = b;
        this->capacity = c;
        this->flow = f;
    }
};

// Class for graph object of serial implementation
class Graph_s
{
    long V; // number of vertices
    vector<Vertex> vertex; // vector of vertices
    vector<Edge_s> edge; // vector of edges

    // function to push excess flow from u
    bool push(long u);

    // function to relabel a vertex u
    void relabel(long u);

    // function to initialize preflow
    void preflow(long s);

    // function to reverse edge
    void updatereverseflow(long i, long flow);

public:
    Graph_s(long v); // constructor to create graph with v vertices

    void addedge(long u, long v, long w); // function to add an edge

    long maxflow(long s, long t); // function that returns maximum flow from source s to sink t

};

long check(long V, long E, long source, long sink);
 
#endif


