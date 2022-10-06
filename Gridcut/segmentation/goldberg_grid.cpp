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
    struct Node; // 144 bytes . without flow 128, 104
    vector<vector<Node>> nodes;
    struct Node{
        int e; // 4
        int d; // 4
        int flow [SIZE]; // 16 
        int neighbor_heights [SIZE]; // 16
        int residual_capacities [SIZE]; // 32
        int sigma [SIZE]; // 24
        int temp_vector1 [SIZE]; // used as temp vector 1 // 24
        int temp_vector2 [SIZE]; // used as temp vector 2 // 24
        
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


    void set_terminal_cap(int idxX, int idxY, int cap_source, int cap_sink)
    {
        nodes[idxY][idxX].residual_capacities[FROM_SOURCE] = cap_source;
        nodes[idxY][idxX].residual_capacities[TO_SINK] = cap_sink;
    }

    void set_neighbor_cap(int idxX, int idxY, int offset_x, int offset_y, int cap)
    {
        if ( offset_x == 1 && offset_y  == 0)
            nodes[idxY][idxX].residual_capacities[RIGHT] = cap;
        else if ( offset_x == 0 && offset_y == -1)
            nodes[idxY][idxX].residual_capacities[DOWN] = cap;
        else if ( offset_x == -1 && offset_y == 0)
            nodes[idxY][idxX].residual_capacities[LEFT] = cap;
        else if ( offset_x == 0 && offset_y == 1)
            nodes[idxY][idxX].residual_capacities[UP] = cap;
        else
            throw std::invalid_argument( "received non grid offsets" );        
    }



    void initialize_nodes(int w, int h)
    {
        vector<Node> temp_v;
        int zero[SIZE];
        nodes.clear();
        for(int k = 0; k < SIZE; k++)
            zero[k] = 0;   
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {   
                temp_v.push_back(Node(zero));

            }
            nodes.push_back(temp_v);
            temp_v.clear();
        }
    }




    void initializations(int width, int height)
    {
        W = width;
        H = height;
        N = W * H;
        S = 0;
        T = N + 1;
        E_S = 0;
        E_T = 0;
        //initialize_nodes(capacity);
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
                //nodes[i][j].neighbor_heights[FROM_SOURCE] = D_S;
                nodes[i][j].neighbor_heights[TO_SOURCE] = D_S;
            }
        }
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
                /*
                if (nodes[i][j].e > 0)
                    nodes[i][j].calc_out_flows();
                */
                
                nodes[i][j].calc_out_flows();
            }
        }
    }   


    long latencu = 0;

    void push_flow()
    {



        // Copy = 1
        // Increment (N) = 5N
        // Addition (N) = 10N
        // Maximum (N) = 10N
        // Minimum (N) = 10N


        // Comunication
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++){
                nodes[i][j].x = nodes[i - 1][j].x;
            }
        }
        latency += NUM_COMMUNCATIONS;

        // Computation
        for(int i ..){
            for(int j){
                nodes[i][j].y = nodes[i][j].x - ndoes[i][j].z;
            }
        }
        latency += 






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

    int goldberg_grid(int width, int height){
        
        // Initialization 
        initializations(width, height);
        pre_flow();

        int i = 0;
        while(check_excess())
        {
            calc_outflow();
            push_flow();
            relabel();
            update_excess();
            i++;  
            if (i %10000 == 0)
            {
                cout << i << endl;
            }
        }
        cout << "number of iterations: " << i << endl;
        return E_T;
    }
}



#endif