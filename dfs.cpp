
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
  vector<bool> visited;

   public:
  Graph(int V);
  void addEdge(int src, int dest);
  void DFS(int vertex);
};

// Initialize graph
Graph::Graph(int vertices) {
  numVertices = vertices;
  adjLists = new list<int>[vertices];
  visited = vector<bool>(vertices,false);
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

  list<int>::iterator i;
  for (i = adjList.begin(); i != adjList.end(); ++i)
  {
    if (!visited[*i])
      DFS(*i);
  }  
}

#endif