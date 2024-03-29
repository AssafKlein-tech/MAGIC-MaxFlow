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
const int min_vert = 1000;
const int max_vert = 2000;
const int limit_capacity = 10000;
const int graph_density_percent = 50;
const int min_percent = 1;
const int max_percent = 100;


int random_int(int first, int last)
{
    //std::default_random_engine generator;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> distribution(first, last);
	return distribution(gen);
}




void graph_gen(graph* capacity, int *num_vertices)
{
    int N = random_int(min_vert, max_vert);
    *num_vertices = N;
    //graph C;
    graph binary;
    (*capacity).assign(N, vi(N, 0));
    binary.assign(N, vi(N, 1));

    int path_len = random_int(1, N-2); //generate default path length
    //cout << "len: " << path_len << endl;
    // int path_vertices[path_len];
    // int temp = 1;
    // for (int i = 0; i < path_len - 1; i++)
    // {
    //     path_vertices[i] = random_int(temp, 100);
    // }

    for (int i = 0; i < N; i++) //set first col and last row to zeros
    {
        binary[i][0] = 0;
        binary[N-1][i] = 0;
    }
    for (int i = 0; i < path_len; i++) //adding the default path capacities
    {
        (*capacity)[i][i+1] = random_int(1, limit_capacity);
        binary[i][i+1] = 0;
        binary[i+1][i] = 0;
    }
    (*capacity)[path_len][N-1] = random_int(1, limit_capacity); // adding last vertice of the default path
    binary[path_len][N-1] = 0;
    binary[N-1][path_len] = 0;
    //cout << "number of vertices: " << N << endl;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            //cout << "i: " << i << ", j:" << j << endl;
            if(binary[i][j] != 0)
            {
                if(random_int(min_percent, max_percent) < graph_density_percent)
                {    
                    binary[j][i] = 0;
                    (*capacity)[i][j] = random_int(1, limit_capacity);
                }
            }
        }
    }
}

#endif