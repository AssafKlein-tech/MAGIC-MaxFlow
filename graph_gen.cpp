#ifndef GRAPH_GEN_CPP
#define GRAPH_GEN_CPP

#include <iostream>
#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <random>
using namespace std;

typedef vector<int> vi;
typedef vector<vi> graph;

int random_int(int first, int last)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> distribution(first, last);
	return distribution(generator);
}




void graph_gen(graph* capacity, int *num_vertices)
{
    int N = random_int(4, 100);
    *num_vertices = N;
    int limit_capacity = 10000;
    int graph_density_percent = 50;
    graph C;
    graph binary;
    C.assign(N, vi(N, 0));
    binary.assign(N, vi(N, 1));

    int path_len = random_int(1, N-2);
    // int path_vertices[path_len];
    // int temp = 1;
    // for (int i = 0; i < path_len - 1; i++)
    // {
    //     path_vertices[i] = random_int(temp, 100);
    // }

    for (int i = 0; i < N; i++)
    {
        binary[0][i] = 0;
        binary[N-1][i] = 0;
    }
    for (int i = 0; i < path_len; i++)
    {
        C[i][i+1] = random_int(1, limit_capacity);
        binary[i][i+1] = 0;
        binary[i+1][i] = 0;
    }
    C[path_len][N-1] = random_int(1, limit_capacity);
    binary[path_len][N-1] = 0;
    binary[N-1][path_len] = 0;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; i++)
        {
            if(binary[i][j] != 0)
            {
                if(random_int(1, 100) < graph_density_percent)
                {    
                    binary[j][i] = 0;
                    C[i][j] = random_int(1, limit_capacity);
                }
            }
        }
    }
    capacity = &C;
}

#endif