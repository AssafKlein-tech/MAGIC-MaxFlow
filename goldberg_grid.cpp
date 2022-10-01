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
    //graph d, e;
    int E_S, E_T, D_S, D_T;
    int N, W, H, S, T;
    struct Node;
    vector<vector<Node>> nodes;
    struct Node{
        int e;
        int d;
        int flow [SIZE];
        int neighbor_heights [SIZE];
        int residual_capacities [SIZE];
        int sigma [SIZE];
        int Cf_temp [SIZE];
        int Cf_sum [SIZE];
        
        Node (
            int flow_arg[SIZE],
            int d[SIZE],
            int residual[SIZE],
            int sig[SIZE]){
                e = 0;
                d = 0;
                for (int i = 0; i < SIZE; i++)
                {
                    flow[i] = flow_arg[i];
                    neighbor_heights[i] = d[i];
                    residual_capacities[i] = residual[i];
                    sigma[i] = sig[i];
                    Cf_temp[i] = 0;
                    Cf_sum[i] = 0;
                }
            }

        void calc_cfTemp()
        {
            for (int i = 0; i < SIZE; i++)
            {
                Cf_temp[i] = (d - neighbor_heights[i] == 1) ? residual_capacities[i] : 0;
            }
        }

        void sf_sum()
        { 
            int sum = 0;
            for (int i = 0; i < SIZE; i++)
            {
                Cf_sum[i] = sum;
                sum += Cf_temp[i];
            }
        }

        void calc_sigma()
        {
            for (int i = 0; i < SIZE; i++)
            {
                sigma[i] = ((e- Cf_sum[i]) < Cf_temp[i]) ? (e - Cf_sum[i]) : Cf_temp[i];
                sigma[i] = (sigma[i] > 0) ? sigma [i] : 0;
                e -= sigma[i];
            }
        }

        void calc_out_flows()
        {
            calc_cfTemp();
            sf_sum();
            calc_sigma();
        }
    };
/*
    struct out_flow{
        int Cf_temp [SIZE];
        int Cf_sum [SIZE];

        
        out_flow ()
        {
            for (int i = 0; i < SIZE; i++)
            {
                Cf_temp[i] = 0;
                Cf_sum[i] = 0;
            }
        }

        void calc_cfTemp(int *hights, int * cf, int d)
        {
            for (int i = 0; i < SIZE; i++)
            {
                Cf_temp[i] = (d - hights[i] == 1) ? cf[i] : 0;
            }
        }

        void sf_sum()
        { 
            int sum = 0;
            for (int i = 0; i < SIZE; i++)
            {
                Cf_sum[i] = sum;
                sum += Cf_temp[i];
            }
        }

        int * calc_sigma(int e)
        {
            int sigma[SIZE];
            for (int i = 0; i < SIZE; i++)
            {
                sigma[i] = ((e- Cf_sum[i]) < Cf_temp[i]) ? (e - Cf_sum[i]) : Cf_temp[i];
                sigma[i] = (sigma[i] > 0) ? sigma [i] : 0;
            }
            return sigma;
        }

        int * calc_out_flows(int *hights, int * cf, int d, int e)
        {
            calc_cfTemp(hights, cf,d);
            sf_sum();
            return calc_sigma(e);
        }
    };*/


    //vector<vector<out_flow>> out_flow_matrix;


    void print_f()
    {

    }

    void print_e()
    {
        /*
        std::cout << "e_vector:" << std::endl;
        for(size_t i = 0; i < e.size(); i++)
            std::cout << (e[i]) << std::endl;
        */
    }

    void initialize_nodes(graph capacity)
    {
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
                cap[TO_SOURCE] = capacity[index][0];
                cap[FROM_SOURCE] = capacity[0][index];
                cap[TO_SINK] = capacity[index][N + 1];
                cap[FROM_SINK] = capacity[N + 1][index];
                // Right vertex
                cap[RIGHT] = (j - 1 < W) ? capacity[index][index + 1] : 0;
                // Down vertex
                cap[DOWN] = (i > 0) ? capacity[index][index - W] : 0;
                // Left vertex
                cap[LEFT] = (j > 0) ? capacity[index][index - 1] : 0;
                // Upwards vertex
                cap[UP] = (i - 1 < H) ? capacity[index][index + W] : 0;            

                temp_v.push_back(Node(
                                    zeros,
                                    zeros,
                                    cap,
                                    zeros));
            }
            nodes.push_back(temp_v);
            temp_v.clear();
        }
    }

   /* void initialize_out_flow_matrix()
    {
        vector<out_flow> temp_v;
        for(int j = 0; j < W; j++)
        {
            temp_v.push_back(out_flow());
        }
        for(int i = 0; i < H; i++)
        {
            out_flow_matrix.push_back(temp_v);
            temp_v.clear();
        }
    }*/

    /*
    void initialize_graphs()
    {
        e.assign(H , vi(W,0));
        d.assign(H , vi(W,0));
    }
    */


    void initializations(graph capacity, int width, int height)
    {
        W = width;
        H = height;
        N = W * H;
        S = N;
        T = N + 1;
        initialize_nodes(capacity);
        //initialize_graphs();
    }

    void pre_flow()
    {
        D_S = N;
        D_T = 0;
        E_T = 0;

        int sum = 0;
        int flow;

        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                flow = nodes[i][j].residual_capacities[FROM_SOURCE];
                
                //updates the flow matrix
                nodes[i][j].flow[FROM_SOURCE] = flow;

                //updates the residual_capacities
                nodes[i][j].residual_capacities[FROM_SOURCE] = 0;
                nodes[i][j].residual_capacities[TO_SOURCE] = flow;

                // the excess for each node
                nodes[i][j].e = flow;
                sum += flow;

                // std::cout << temp << std::endl;
            }
        }
        E_S= -sum;
    }

    bool check_excess()
    {
        // Returns true if theres at least one non-zero
        // value in excess, excluding {s,t}
        /*for (int i = 0; i < N; i++)
        {
            if (e[i] != 0)
                return true;
        }
        return false;*/
        return !(E_S + E_T == 0);
    }

    void calc_outflow()
    {

        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].calc_out_flows();

            }
        }
    }   

    void push_flow()
    {
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                Node *node = &(nodes[i][j]);
                //update out flow
                for (int k = RIGHT ; k <= SIZE; k++) 
                {
                    node->flow[k] += node->sigma[k];
                    node->residual_capacities[k] -= node->sigma[k];
                }

                // update in flow
                node->flow[RIGHT] -= (j < W - 1)
                ? nodes[i][j + 1].sigma[LEFT] : 0;
                node->flow[DOWN] -= (i > 0)
                ? nodes[i - 1][j].sigma[UP] : 0;        
                node->flow[LEFT] -= (j > 0)
                ? nodes[i][j- 1].sigma[RIGHT] : 0; 
                node->flow[UP] -= (i < H - 1)
                ? nodes[i + 1][j].sigma[DOWN] : 0;

                // Residual capacities
                node->residual_capacities[RIGHT] += (j < W - 1)
                ? nodes[i][j+ 1].sigma[LEFT] : 0;
                node->residual_capacities[DOWN] += (i > 0)
                ? nodes[i - 1][j].sigma[UP] : 0;        
                node->residual_capacities[LEFT] += (j> 0)
                ? nodes[i][j- 1].sigma[RIGHT] : 0; 
                node->residual_capacities[UP] += (i < H - 1)
                ? nodes[i + 1][j].sigma[DOWN] : 0;
            }
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
                Node *node = &(nodes[i / W][i % W]); //[height][width]
                new_D[i][j] = 2 * N;
                residual_temp[i][j] = (node->residual_capacities[j] == 0) ? 1 : 0;
                new_D[i][j] *= residual_temp[i][j];
                new_D[i][j] += node->neighbor_heights[j];
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
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                Node *node = &(nodes[i][j]);
                node->e
            }
        }

        for (int i = 0; i < N; i++) for (int j = 0; j < SIZE; j++)
        {
            Node *node = &(nodes[i / W][i % W]); //[height][width]

            if (j == FROM_SOURCE){
                // std::cout << "Here 1" << node->sigma[j] << std::endl; 
                e[S] += node->sigma[j];
                }
            else if (j == FROM_SINK){
                // std::cout << "Here 2" << node->sigma[j] << std::endl;
                e[T] += node->sigma[j];}
            else    {
                // std::cout << "Here 3" << node->sigma[j] << std::endl;
                e[i] += node->sigma[j];
            }
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
        
        // Initialization 
        initializations(capacity, width, height);
        pre_flow();
        int i = 0;
        while(check_excess())
        {
            // std::cout << "Here " << i << std::endl;
            // std::cout << (e[S] + e[T]) << std::endl;
            reset_sigma();
            calc_outflow();
            push_flow();
            relabel();
            update_excess();
            i++;
            print_e();
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