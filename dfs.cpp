
#ifndef DFS_CPP
#define DFS_CPP

// DFS algorithm in C++
#include "goldberg_grid.cpp"
#include <iostream>
#include <list>
using namespace std;

list<int> source_cut;

class Graph {
  int numVertices;
  list<int> *adjLists;
  bool *visited;

   public:
  Graph(int V);
  void addEdge(int src, int dest);
  void DFS(int vertex);
};

// Initialize graph
Graph::Graph(int vertices) {
  numVertices = vertices;
  adjLists = new list<int>[vertices];
  visited = new bool[vertices];
}

// Add edges
void Graph::addEdge(int src, int dest) {
  adjLists[src].push_front(dest);
}

// DFS algorithm
void Graph::DFS(int vertex) {
  visited[vertex] = true;
  list<int> adjList = adjLists[vertex];
  source_cut.push_back(vertex);

  // cout << vertex << " ";

  list<int>::iterator i;
  for (i = adjList.begin(); i != adjList.end(); ++i)
    if (!visited[*i])
      DFS(*i);
}

int graph_dfs(int W, int H) {
  const int sink = W*H + 1;
  Graph g(2 + W * H);
  int index;
  for(int i = 0; i < H; i++)
  {
    for(int j = 0; j < W; j++)
    {
      index = 1 + j + i * W;
      if(grid::nodes[i][j].residual_capacities[grid::FROM_SOURCE])
        g.addEdge(0,index);
      if(grid::nodes[i][j].residual_capacities[grid::TO_SOURCE])
        g.addEdge(index,0);
      if(grid::nodes[i][j].residual_capacities[grid::FROM_SINK])
        g.addEdge(sink,index);
      if(grid::nodes[i][j].residual_capacities[grid::TO_SINK])
        g.addEdge(index,sink);        
      if(grid::nodes[i][j].residual_capacities[grid::RIGHT])
        g.addEdge(index,index + 1);
      if(grid::nodes[i][j].residual_capacities[grid::DOWN])
        g.addEdge(index,index + W);
      if(grid::nodes[i][j].residual_capacities[grid::LEFT])
        g.addEdge(index,index - 1);
      if(grid::nodes[i][j].residual_capacities[grid::UP])
        g.addEdge(index,index - W); 

    }
  }

  g.DFS(sink);

  return 0;
}

#endif