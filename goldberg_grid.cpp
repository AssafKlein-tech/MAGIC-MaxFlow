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
    const int OUT_VERTICES = 6;
    const int SIZE = 8;
    const int TEMP1 = 1;
    const int TEMP2 = 2;
    const int SIGMA = 3;


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
        int temp_vector1 [SIZE]; // used as temp vector 1
        int temp_vector2 [SIZE]; // used as temp vector 2
        
        Node (
            int residual[SIZE])
            {
                e = 0;
                d = 0;
                for (int i = 0; i < SIZE; i++)
                {
                    residual_capacities[i] = residual[i];
                    flow[i] = 0;
                    neighbor_heights[i] = 0;
                    sigma[i] = 0;
                    temp_vector1[i] = 0;
                    temp_vector2[i] = 0;
                }
            }
        

        void calc_cfTemp()
        {
            for (int i = 0; i < OUT_VERTICES; i++)
            {
                // temp vector 1 used as Cf temp
                temp_vector1[i] = (d - neighbor_heights[i] == 1) ? residual_capacities[i] : 0;
            }
        }

        void sf_sum()
        { 
            int sum = 0;
            for (int i = OUT_VERTICES - 1; i >= 0; i--)
            {
                temp_vector2[i] = sum; // temp vector 2 used as Cf sum
                sum += temp_vector1[i];
                // temp vector 1 used as Cf temp
                // temp vector 2 used as Cf sum
                sigma[i] = std::min((e - temp_vector2[i]), temp_vector1[i]);
                sigma[i] = std::max(sigma[i], 0);
            }
            for (int i = OUT_VERTICES - 1; i >= 0; i--)
            {
                e -= sigma[i];
            }

        }

        void calc_sigma()
        {
            for (int i = 0; i < SIZE; i++)
            {
                // temp vector 1 used as Cf temp
                // temp vector 2 used as Cf sum
                sigma[i] = ((e - temp_vector2[i]) < temp_vector1[i]) ? (e - temp_vector2[i]) : temp_vector1[i];
                sigma[i] = (sigma[i] > 0) ? sigma [i] : 0;
                e -= sigma[i];
            }
        }

        void calc_out_flows()
        {
            calc_cfTemp();
            sf_sum();

        }
    };


    void print_stats()
    {
        std::cout << "________________________" << std::endl;

        std::cout << "sigma_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].sigma[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }
        std::cout << "Cf_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].residual_capacities[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }
        /*
        std::cout << "temp1_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].temp_vector1[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }
        */
        std::cout << "temp2_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].temp_vector2[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }

        std::cout << "heights:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].neighbor_heights[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }

        std::cout << "E_S + E_T = " << (E_S + E_T) << std::endl;
        std::cout << "E_S = " << (E_S) << std::endl;
        std::cout << "E_T = " << (E_T) << std::endl;
        std::cout << "D_S = " << (D_S) << std::endl;
        std::cout << "D_T = " << (D_T) << std::endl;
        std::cout << "e_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                std::cout << nodes[i][j].e << ", ";
            }
            std::cout << " " << std::endl;
        }
        std::cout << "d_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                std::cout << nodes[i][j].d << ", ";
            }
            std::cout << " " << std::endl;
        }
        std::cout << "flow_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].flow[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }
        std::cout << "\n" << std::endl;

    }

    void initialize_nodes(graph capacity)
    {
        vector<Node> temp_v;
        int cap[SIZE];
        nodes.clear();
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                int index = i * W + j + 1;
                cap[TO_SOURCE] = 0;
                cap[FROM_SOURCE] = capacity[S][index];
                cap[TO_SINK] = capacity[index][T];
                cap[FROM_SINK] = 0;
                // Right vertex
                cap[RIGHT] = (j < W - 1) ? capacity[index][index + 1] : 0;
                // Down vertex
                cap[UP] = (i > 0) ? capacity[index][index - W] : 0;
                // Left vertex
                cap[LEFT] = (j > 0) ? capacity[index][index - 1] : 0;
                // Upwards vertex
                cap[DOWN] = (i < H - 1) ? capacity[index][index + W] : 0;            

                temp_v.push_back(Node(cap));

            }
            nodes.push_back(temp_v);
            temp_v.clear();
        }
        E_T = capacity[0][T];
        E_S = -(capacity[0][T]);
    }




    void initializations(graph capacity, int width, int height)
    {
        W = width;
        H = height;
        N = W * H;
        S = 0;
        T = N + 1;
        initialize_nodes(capacity);
        //initialize_graphs();
    }

    void pre_flow()
    {
        D_S = N + 2;
        D_T = 0;

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

                // The source height
                nodes[i][j].neighbor_heights[FROM_SOURCE] = D_S;
                nodes[i][j].neighbor_heights[TO_SOURCE] = D_S;
            }
        }
        //nodes[0][0].d = 1;
        E_S -= sum;
    }

    bool check_excess()
    {
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
                for (int k = RIGHT ; k <= OUT_VERTICES; k++) 
                {
                    node->flow[k] += node->sigma[k];
                    node->residual_capacities[k] -= node->sigma[k];
                }

                // update in flow
                node->flow[RIGHT] -= (j < W - 1)
                ? nodes[i][j + 1].sigma[LEFT] : 0;
                node->flow[DOWN] -= (i < H - 1)
                ? nodes[i + 1][j].sigma[UP] : 0;        
                node->flow[LEFT] -= (j > 0)
                ? nodes[i][j- 1].sigma[RIGHT] : 0; 
                node->flow[UP] -= (i > 0)
                ? nodes[i - 1][j].sigma[DOWN] : 0;
               // node->flow[TO_SOURCE] -= node->sigma[FROM_SOURCE];
                node->flow[FROM_SOURCE] -= node->sigma[TO_SOURCE];
                //node->flow[TO_SINK] -= node->sigma[FROM_SINK];
                node->flow[FROM_SINK] -= node->sigma[TO_SINK];               

                // Residual capacities
                node->residual_capacities[RIGHT] += (j < W - 1)
                ? nodes[i][j+ 1].sigma[LEFT] : 0;
                node->residual_capacities[DOWN] += (i < H - 1)
                ? nodes[i + 1][j].sigma[UP] : 0;        
                node->residual_capacities[LEFT] += (j> 0)
                ? nodes[i][j- 1].sigma[RIGHT] : 0; 
                node->residual_capacities[UP] += (i > 0)
                ? nodes[i - 1][j].sigma[DOWN] : 0;
                //node->residual_capacities[TO_SOURCE] += node->sigma[FROM_SOURCE];
                node->residual_capacities[FROM_SOURCE] += node->sigma[TO_SOURCE];
               // node->residual_capacities[TO_SINK] += node->sigma[FROM_SINK];
                node->residual_capacities[FROM_SINK] += node->sigma[TO_SINK];  
            }
        }
    }
    

    void relabel()
    {
        //calcualte new hight
        int min;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                min = 2*(N+2);
                Node *node = &(nodes[i][j]);
                for (int k = 0; k < OUT_VERTICES; k++)
                {
                    node->temp_vector2[k] = (node->residual_capacities[k] == 0) ? 2*(N+2) : 0;
                    node->temp_vector2[k] += node->neighbor_heights[k];
                    min = std::min(min, node->temp_vector2[k]);
                }
                node->d = min + 1;
            }
        }

        // sending the neighbors the new hight
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                Node *node = &(nodes[i][j]);
                node->neighbor_heights[RIGHT] = (j < W - 1) ? nodes[i][j + 1].d : 0;
                node->neighbor_heights[DOWN] = (i < H - 1) ? nodes[i + 1][j].d : 0;
                node->neighbor_heights[LEFT] = (j > 0) ? nodes[i][j - 1].d : 0;
                node->neighbor_heights[UP] = (i > 0) ? nodes[i - 1][j].d : 0;
            }
        }
    }

    void update_excess()
    {
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                Node *node = &(nodes[i][j]);
                node->e += (j < W - 1) ? nodes[i][j + 1].sigma[LEFT] : 0; // right
                node->e += (i > 0) ? nodes[i - 1][j].sigma[DOWN] : 0; // down
                node->e += (j > 0) ? nodes[i][j - 1].sigma[RIGHT] : 0; // left
                node->e += (i < H - 1) ? nodes[i + 1][j].sigma[UP] : 0; // up
                E_S += node->sigma[TO_SOURCE];
                E_T += node->sigma[TO_SINK];
            }
        }

    }

    int goldberg_grid(graph capacity, int width, int height){
        
        // Initialization 
        initializations(capacity, width, height);
        pre_flow();

        int i = 0;
        while(check_excess())
        {
            calc_outflow();
            push_flow();
            relabel();
            update_excess();
            i++;  
        }
        cout << "number of iterations: " << i << endl;
        return E_T;
    }
}



#endif