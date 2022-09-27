#ifndef GOLDBERG_GRID_CPP
#define GOLDBERG_GRID_CPP

#include <iostream>
#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
using namespace std;

namespace grid{


    const int INF = 1000000000;
    const int RIGHT = 0;
    const int DOWN = 1;
    const int LEFT = 2;
    const int UP = 3;
    const int TO_SOURCE = 4;
    const int TO_SINK = 5;
    const int FROM_SOURCE = 6;
    const int FROM_SINK = 7;
    const int SIZE = 8;

    typedef vector<int> vi;
    typedef vector<vi> graph;

    // Declarations
    // graph residual, D, C, sigma, F;
    vi d, e;
    int N, W, H, S, T;
    struct Node;
    vector<vector<Node>> nodes;
    struct Node{

        int capacities [SIZE];
        int flow [SIZE];
        int neighbor_heights [SIZE];
        int residual_capacities [SIZE];
        int sigma [SIZE];
        
        Node (int caps[SIZE],
            int flow_arg[SIZE],
            int d[SIZE],
            int residual[SIZE],
            int sig[SIZE]){
                for (int i = 0; i < SIZE; i++)
                {
                    capacities[i] = caps[i];
                    flow[i] = flow_arg[i];
                    neighbor_heights[i] = d[i];
                    residual_capacities[i] = residual[i];
                    sigma[i] = sig[i];
                }
            }
    };

    void initializations(graph capacity)
    {
        N = W * H;
        S = N;
        T = N + 1;
        vector<Node> temp_v;
        int zeros[SIZE];
        for (int i = 0; i < SIZE; i++)
            zeros[i] = 0;
        int cap[SIZE];
        for(int i = 0; i < H; i++)
        {

            for(int j = 0; j < W; j++)
            {
                int index = i * W + j;
                cap[TO_SOURCE] = capacity[i * W + j][0];
                cap[FROM_SOURCE] = capacity[0][i * W + j];
                cap[TO_SINK] = capacity[i * W + j][N + 1];
                cap[FROM_SINK] = capacity[N + 1][i * W + j];
                // Right vertex
                cap[RIGHT] = (i - 1 < W) ? capacity[i + 1][j] : 0;
                // Down vertex
                cap[DOWN] = (j > 0) ? capacity[i][j-1] : 0;
                // Left vertex
                cap[LEFT] = (i > 0) ? capacity[i - 1][j] : 0;
                // Upwards vertex
                cap[UP] = (j - 1 < H) ? capacity[i][j + 1] : 0;            

                temp_v.push_back(Node(cap,
                                    zeros,
                                    zeros,
                                    cap,
                                    zeros));
            }
            nodes.push_back(temp_v);
            temp_v.clear();

        }
        e.assign(N + 2, 0);
        d.assign(N + 2, 0);
        e[S] = INF;
    }

    void pre_flow()
    {
        d[S] = N;
        int sum = 0;
        int temp;
        // std::cout << W << ", H=" << H << std::endl;
        for(int i = 0; i < W; i++) for(int j = 0; j < H; j++)
        {
            Node node = nodes[i][j];
            temp = node.residual_capacities[FROM_SOURCE];
            node.residual_capacities[FROM_SOURCE] = 0;
            node.flow[FROM_SOURCE] = temp;
            node.flow[TO_SOURCE] = -temp;
            node.residual_capacities[TO_SOURCE] = temp;
            node.neighbor_heights[TO_SOURCE] = d[S];
            sum += temp;
            e[i * H + j] = temp;
            // std::cout << temp << std::endl;
        }
        e[S] = -sum;
    }

    bool check_excess()
    {
        // Returns true if theres at least one non-zero
        // value in excess, excluding {s,t}
        for (int i = 0; i < N; i++)
        {
            if (e[i] != 0)
                return true;
        }
        return false;
    }

    void calc_outflow()
    {

        graph D_binary, residual_temp, residual_sum;
        D_binary.assign(N, vi(SIZE, 0));
        residual_temp.assign(N, vi(SIZE, 0));
        residual_sum.assign(N, vi(SIZE, 0));
        int height, width;

        // For the other vertices
        for(int i = 0; i < N; i++) for(int j = 0; j < SIZE; j++)
        {
            Node node = nodes[i / W][i % W]; //[height][width]
            if (j == FROM_SOURCE)
            {
                D_binary[i][j] = ((d[S] - node.neighbor_heights[j] ) == 1) ? 1 : 0;
            }
            else if (j == FROM_SINK)
            {
                D_binary[i][j] = ((d[T] - node.neighbor_heights[j] ) == 1) ? 1 : 0;
            }
            else
            {
            D_binary[i][j] = ((d[i] - node.neighbor_heights[j] ) == 1) ? 1 : 0;
            }
            residual_temp[i][j] = D_binary[i][j] * node.residual_capacities[j];
        }

        for (int i = 0; i < N; i++)
        {
            height = i / W;
            width = i % W;
            int sum = 0;
            for (int j = SIZE-1; j >= 0; j--)
            {
                Node node = nodes[height][width];
                // prefix sum
                residual_sum[i][j] = sum;
                sum += residual_temp[i][j];
                // Take the minimum
                if (j == FROM_SOURCE)
                {
                    node.sigma[j] = std::max(
                    std::min((e[S] - residual_sum[i][j]), residual_temp[i][j]), 0);
                    // node.sigma[j] = ((e[S] - residual_sum[i][j]) < residual_temp[i][j])
                    // ? (e[S] - residual_sum[i][j]) : (residual_temp[i][j]);
                    // // node.sigma[j] = std::max(node.sigma[j], 0);
                    // node.sigma[j] = (node.sigma[j] > 0) ? node.sigma[j] : 0;
                    // Calc the excess leftover
                    e[S] -= node.sigma[j];
                }
                if (j == FROM_SINK)
                {
                    node.sigma[j] = std::max(
                    std::min((e[T] - residual_sum[i][j]), residual_temp[i][j]), 0);
                    // node.sigma[j] = ((e[T] - residual_sum[i][j]) < residual_temp[i][j])
                    // ? (e[T] - residual_sum[i][j]) : (residual_temp[i][j]);
                    // node.sigma[j] = (node.sigma[j] > 0) ? node.sigma[j] : 0;
                    // Calc the excess leftover
                    e[T] -= node.sigma[j];
                }   
                else
                {
                    node.sigma[j] = std::max(
                    std::min((e[i] - residual_sum[i][j]), residual_temp[i][j]), 0);
                    // node.sigma[j] = ((e[i] - residual_sum[i][j]) < residual_temp[i][j])
                    // ? (e[i] - residual_sum[i][j]) : (residual_temp[i][j]);
                    // node.sigma[j] = (node.sigma[j] > 0) ? node.sigma[j] : 0;
                    // Calc the excess leftover
                    e[i] -= node.sigma[j];
                }         
            }
        }
    }   

    void push_flow()
    {
        int height, width;
        for(int i = 0; i < N; i++) for(int j = 0; j < SIZE; j++)
        {
            height = i / W;
            width = i % W;
            Node node = nodes[height][width]; //[height][width]
            node.flow[j] += node.sigma[j];
            node.residual_capacities[j] -= node.sigma[j];
            // Transposed operations
            // Flow
            node.flow[RIGHT] = (width < W - 1)
            ? nodes[height][width + 1].flow[LEFT] : 0;
            node.flow[DOWN] = (height > 0)
            ? nodes[height - 1][width].flow[UP] : 0;        
            node.flow[LEFT] = (width > 0)
            ? nodes[height][width - 1].flow[RIGHT] : 0; 
            node.flow[UP] = (height < H - 1)
            ? nodes[height + 1][width].flow[DOWN] : 0;
            // Residual capacities
            node.residual_capacities[RIGHT] = (width < W - 1)
            ? nodes[height][width + 1].residual_capacities[LEFT] : 0;
            node.residual_capacities[DOWN] = (height > 0)
            ? nodes[height - 1][width].residual_capacities[UP] : 0;        
            node.residual_capacities[LEFT] = (width > 0)
            ? nodes[height][width - 1].residual_capacities[RIGHT] : 0; 
            node.residual_capacities[UP] = (height < H - 1)
            ? nodes[height + 1][width].residual_capacities[DOWN] : 0;
        }
    }

    void relabel()
    {
        graph new_D, residual_temp;
        new_D.assign(N, vi(SIZE, 0));
        residual_temp.assign(N, vi(SIZE, 0));
        for(int i = 0, min = 2*N; i < N; i++)
        {
            for(int j = 0; j < SIZE; j++)
            {
                Node node = nodes[i / W][i % W]; //[height][width]
                new_D[i][j] = 2 * N;
                residual_temp[i][j] = (node.residual_capacities[j] == 0) ? 1 : 0;
                new_D[i][j] *= residual_temp[i][j];
                new_D[i][j] += node.neighbor_heights[j];
                min = std::min(min, new_D[i][j]);
            } 
            d[i] = min + 1; 
        }
        for (int i = 0; i < N; i++) for (int j = 0; j < SIZE; j++)
        {
            Node *node = &(nodes[i / W][i % W]); //[height][width]

            if (j == FROM_SOURCE)
                node->neighbor_heights[j] = d[S];
            else if (j == FROM_SINK)
                node->neighbor_heights[j] = d[T];
            else    
                node->neighbor_heights[j] = d[i];
        }
    }

    void update_excess()
    {
        for (int i = 0; i < N; i++) for (int j = 0; j < SIZE; j++)
        {
            Node node = nodes[i / W][i % W]; //[height][width]

            if (j == FROM_SOURCE)
                e[S] += node.sigma[j];
            else if (j == FROM_SINK)
                e[T] += node.sigma[j];
            else    
                e[i] += node.sigma[j];
        }
    }

    void reset_sigma()
    {
        for (int i = 0; i < N; i++) for (int j = 0; j < SIZE; j++)
        {
            nodes[i / W][i % W].sigma[j] = 0;
        }
    }

    int goldberg_grid(graph capacity, int width, int height){
        W = width;
        H = height;
        // Initialization 
        initializations(capacity);
        pre_flow();
        int i = 0;
        while(check_excess())
        {
            reset_sigma();
            calc_outflow();
            push_flow();
            relabel();
            update_excess();
            i++;
        }
        cout << "number of iterations: " << i << endl;
        return e[T];

        // // Pre flow

        // while(true){


        //     for(int i = 0; i < W; i++) for(int j = 0; j < H; j++){
        //         Node node = nodes[i][j];
        //         // Some local operation in the node (optionally with neighbours)
        //         // node.height = max(node.neighbor_heights[0], [1], ...)
        //         // node.height = node_up.
        //     }



        //     // if(...) break;
        // }

        // // Generate output by reading from each Node, save as segmentation


    }
}



#endif