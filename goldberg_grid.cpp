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
    long latency = 0;
    int E_S, E_T, D_S, D_T;
    int N, W, H, S, T;
    struct Node; // 104
    vector<vector<Node>> nodes;
    struct Node{
        int e; // 4
        int d; // 4
       // int flow [SIZE]; // 16 
        int neighbor_heights [OUT_VERTICES]; // 16
        int residual_capacities [SIZE]; // 32
        short sigma [OUT_VERTICES]; // 12
        short temp_vector1 [OUT_VERTICES]; // used as temp vector 1 // 12
        int temp_vector2 [OUT_VERTICES]; // used as temp vector 2 // 24
        //E_S, E_T, D_S, D_T , i ,j
        
        
        Node (
            int residual[SIZE])
            {
                e = 0;
                d = 0;
                for (int i = 0; i < SIZE; i++)
                {
                    residual_capacities[i] = residual[i];
                    //flow[i] = 0;
                    if (i < OUT_VERTICES)
                    {
                        neighbor_heights[i] = 0;
                        sigma[i] = 0;
                        temp_vector1[i] = 0;
                        temp_vector2[i] = 0;
                    }
                }
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
        */
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
       /* std::cout << "flow_vector:" << std::endl;
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
        */
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


        // Computation 1 - pushing the flow from the source, reversing the
        // residual vertex direction
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].residual_capacities[TO_SOURCE] = nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }

        // Computation 2 - updating the excess after flow push from source
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].e = nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }

        // Computation 3 - updating the sum - 
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                sum += nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }

        // Computation 4 - reseting the residual vertex from source
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].residual_capacities[FROM_SOURCE] = 0;
            }
        }

        // Computation 5 - updating the source height
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].neighbor_heights[TO_SOURCE] = D_S;
            }
        }
        E_S -= sum;


        // int flow;
        // for(int i = 0; i < H; i++)
        // {
        //     for(int j = 0; j < W; j++)
        //     {
        //         flow = nodes[i][j].residual_capacities[FROM_SOURCE];
                
        //         //updates the flow matrix
        //        // nodes[i][j].flow[FROM_SOURCE] = flow;

        //         //updates the residual_capacities
        //         nodes[i][j].residual_capacities[FROM_SOURCE] = 0;
        //         nodes[i][j].residual_capacities[TO_SOURCE] = flow;

        //         // the excess for each node
        //         nodes[i][j].e = flow;
        //         sum += flow;

        //         // The source height
        //         //nodes[i][j].neighbor_heights[FROM_SOURCE] = D_S;
        //         nodes[i][j].neighbor_heights[TO_SOURCE] = D_S;
        //     }
        // }
        // E_S -= sum;
    }

    bool check_excess()
    {
        return !(E_S + E_T == 0);
    }

    void calc_outflow()
    {
        // Computation 1 - Compute the maximum flow to each neighbor in this iteration
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {  
                for (int k = 0; k < OUT_VERTICES; k++)
                {
                    // temp vector 1 used as Cf temp - IDK if it is a legal writing
                    nodes[i][j].temp_vector2[k] = nodes[i][j].d - nodes[i][j].neighbor_heights[k]; // compute 
                    nodes[i][j].temp_vector2[k] = (nodes[i][j].temp_vector2[k] == 1) ? nodes[i][j].residual_capacities[k] :  0; //can be translated to and and not to mux
                }
            }
        }

        // Computation 2 - execute suffix sum on temp_voctor1 and save it on temp_vector2
        int sum;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                sum = 0;
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].sigma[k] = sum; // temp vector 2 used as Cf sum
                    sum += nodes[i][j].temp_vector2[k];
                }
            }
        }

        // Computation 3 - compute sigma. non neggative flow of max of capacity or excess left 
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].sigma[k] =  nodes[i][j].e - nodes[i][j].sigma[k]; // can we make in this computation all negative to zeroes?
                    nodes[i][j].sigma[k] = std::min((int)nodes[i][j].sigma[k], (int)nodes[i][j].temp_vector2[k]);
                    nodes[i][j].sigma[k] = std::max((int)nodes[i][j].sigma[k], 0);
                }
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].e -= nodes[i][j].sigma[k];
                }
            }
        }
    }   

    void push_flow()
    {

        // *********** REMINDER **************
        // vector oprations in the same node but all nodes in parallel happen in O(1)
        // moving data from one node to another cost O(1)

        // Copy = 1
        // Increment (N) = 5N
        // Addition (N) = 10N
        // Maximum (N) = 10N
        // Minimum (N) = 10N
        // Mux (N) = 3N
        // And (N) = 3N
        // Or (N) = 2n

        // trivial solution  [ 3(copies)*4 (parameters) * H*W (num of nodes) * 2 (mux) ]   time (not including source and sink)
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++)
            {
                // Computation 1 - subtracting sigma from Cf
                for (int k = RIGHT ; k <= OUT_VERTICES; k++) 
                    nodes[i][j].residual_capacities[k] = nodes[i][j].residual_capacities[k] - nodes[i][j].sigma[k];

                // Computation 2 - copying sigma to temp_vector1 - considering just 
                // overriding sigma but it might be used in following functions
                for (int k = RIGHT ; k <= SIGMA; k++) 
                    nodes[i][j].temp_vector2[k] = nodes[i][j].sigma[k];

                
                if ( j < W - 1)
                {
                    // Communication 1 - placing the RIGHT sigma values in another node
                    nodes[i][j + 1].temp_vector2[RIGHT] = nodes[i][j].temp_vector2[RIGHT];

                    // Computation 3 - adding temp_vector2 to the correct place in Cf LEFT + excess
                    nodes[i][j + 1].residual_capacities[LEFT] += nodes[i][j + 1].temp_vector2[RIGHT];
                    nodes[i][j + 1].e += nodes[i][j + 1].temp_vector2[RIGHT];
                }

                
                if ( i < H - 1)
                {
                    // Communication 2 - placing the DOWN sigma values in another node
                    nodes[i + 1][j].temp_vector2[DOWN] = nodes[i][j].temp_vector2[DOWN];

                    // Computation 4 - adding temp_vector2 to the correct place in Cf UP + excess
                    nodes[i + 1][j].residual_capacities[UP] += nodes[i + 1][j].temp_vector2[DOWN];
                    nodes[i + 1][j].e += nodes[i + 1][j].temp_vector2[DOWN];
                }

                if ( j > 0 )
                {
                     // Communication 3 - placing the LEFT sigma values in another node
                    nodes[i][j - 1].temp_vector2[LEFT] = nodes[i][j].temp_vector2[LEFT];

                    // Computation 5 - adding temp_vector2 to the correct place in Cf RIGHT + excess
                    nodes[i][j - 1].residual_capacities[RIGHT] += nodes[i][j - 1].temp_vector2[LEFT];
                    nodes[i][j - 1].e += nodes[i][j - 1].temp_vector2[LEFT];
                }

                if ( i > 0 )
                {
                    // Communication 4 - placing the UP sigma values in another node
                    nodes[i - 1][j].temp_vector2[UP] = nodes[i][j].temp_vector2[UP];

                    // Computation 6 - adding temp_vector2 to the correct place in Cf DOWN + escess
                    nodes[i - 1][j].residual_capacities[DOWN] += nodes[i - 1][j].temp_vector2[UP];
                    nodes[i - 1][j].e += nodes[i - 1][j].temp_vector2[UP];
                }
            }

        }

        // Computation 7 - adding temp_vector1 to the correct place in Cf FROM_SOURCE
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++){
                nodes[i][j].residual_capacities[FROM_SOURCE] += nodes[i][j].sigma[TO_SOURCE];
                E_S += nodes[i][j].sigma[TO_SOURCE];
            }
        }
        // Computation 8 - adding temp_vector1 to the correct place in Cf FROM_SINK
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++){
                nodes[i][j].residual_capacities[FROM_SINK] += nodes[i][j].sigma[TO_SINK];
                E_T += nodes[i][j].sigma[TO_SINK];
            }
        }
    }

    void relabel()
    {
        int MAX_HEIGHT = 2*(N+2), min;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                min = MAX_HEIGHT;
                for (int k = 0; k < OUT_VERTICES; k++)
                {
                    // Computation 1 - setting max height to saturated vertices,
                    // This if can be translated to bitwise operations
                    nodes[i][j].temp_vector2[k] = (nodes[i][j].residual_capacities[k] == 0) ? MAX_HEIGHT : 0;
                    
                    // Computation 2 - adding the neightbors heights
                    nodes[i][j].temp_vector2[k] += nodes[i][j].neighbor_heights[k];

                    // Computation 3 - comparing the min to the current height
                    min = std::min(min, nodes[i][j].temp_vector2[k]);
                }

                // Computation 4 - every node gets its height from the temp var
                // "min" plus 1
                nodes[i][j].d = min + 1;
            }
        }

        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {

                // Computation 5 - copying this nodes height value to the temp vector
                nodes[i][j].temp_vector2[RIGHT] = nodes[i][j].d;
                nodes[i][j].temp_vector2[DOWN] = nodes[i][j].d;
                nodes[i][j].temp_vector2[LEFT] = nodes[i][j].d;
                nodes[i][j].temp_vector2[UP] = nodes[i][j].d;

                if ( j < W - 1)
                {
                    // Communication 1 - placing the height value in the RIGHT node
                    nodes[i][j + 1].temp_vector2[LEFT] = nodes[i][j].temp_vector2[RIGHT];

                    // Computation 6 - inserting the height value in the correct place
                    nodes[i][j + 1].neighbor_heights[LEFT] = nodes[i][j + 1].temp_vector2[LEFT];
                }
                
                if ( i < H - 1)
                {
                    // Communication 2 - placing the height value in the DOWN node
                    nodes[i + 1][j].temp_vector2[UP] = nodes[i][j].temp_vector2[DOWN];

                    // Computation 7 - inserting the height value in the correct place
                    nodes[i + 1][j].neighbor_heights[UP] = nodes[i + 1][j].temp_vector2[UP];
                }

                if ( j > 0 )
                {
                    // Communication 3 - placing the height value in the LEFT node
                    nodes[i][j - 1].temp_vector2[RIGHT] = nodes[i][j].temp_vector2[LEFT];

                    // Computation 8 - inserting the height value in the correct place
                    nodes[i][j - 1].neighbor_heights[RIGHT] = nodes[i][j - 1].temp_vector2[RIGHT];
                }

                if ( i > 0 )
                {
                    // Communication 4 - placing the height value in the DOWN node
                    nodes[i - 1][j].temp_vector2[DOWN] = nodes[i][j].temp_vector2[UP];

                    // Computation 9 - inserting the height value in the correct place
                    nodes[i - 1][j].neighbor_heights[DOWN] = nodes[i - 1][j].temp_vector2[DOWN];
                }
            }
        }

        // // calcualte new heights
        // int min;
        // for(int i = 0; i < H; i++)
        // {
        //     for(int j = 0; j < W; j++)
        //     {
        //         min = 2*(N+2);
        //         Node *node = &(nodes[i][j]);
        //         for (int k = 0; k < OUT_VERTICES; k++)
        //         {
        //             node->temp_vector2[k] = (node->residual_capacities[k] == 0) ? 2*(N+2) : 0;
        //             node->temp_vector2[k] += node->neighbor_heights[k];
        //             min = std::min(min, node->temp_vector2[k]);
        //         }
        //         node->d = min + 1;
        //     }
        // }

        // // sending the neighbors the new hight
        // for(int i = 0; i < H; i++)
        // {
        //     for(int j = 0; j < W; j++)
        //     {
        //         Node *node = &(nodes[i][j]);
        //         node->neighbor_heights[RIGHT] = (j < W - 1) ? nodes[i][j + 1].d : 0;
        //         node->neighbor_heights[DOWN] = (i < H - 1) ? nodes[i + 1][j].d : 0;
        //         node->neighbor_heights[LEFT] = (j > 0) ? nodes[i][j - 1].d : 0;
        //         node->neighbor_heights[UP] = (i > 0) ? nodes[i - 1][j].d : 0;
        //     }
        // }
    }


    int goldberg_grid(int width, int height, int maxflow){
        
        // Initialization 
        initializations(width, height);
        pre_flow();

        int i = 0;
        while(check_excess())
        {
            calc_outflow();
            push_flow();
            relabel();
            //update_excess();
            i++;  
            if (i %10000 == 0)
            {
                cout << i << endl;
                print_stats();
                break;
            }
        }
        cout << "number of iterations: " << i << endl;
        if ( maxflow != E_T)
        {
            cout << "XXXXX ERROR XXXXX" << endl;
            print_stats();
            cout << "max flow: " << E_T << endl;
            return -1;
        }
        else
        {
            cout << "----- CORRECT -----" << endl;
            return E_T;
        }
        
    }

    // *** pre_flow - Uses neighbor_heights vector
    // *** calc_outflow - Uses two temp_vectors with 6 ints together and neighbor_heights, and then
    // a temp vector with 6 ints and sigma together, maybe we should use just 
    // sigma and one temp_vector -- added in this version
    // *** push_flow - Uses sigma and temp_vector2 that uses 4 ints to communicate
    // *** relabel - Uses neighbor_heights and a 6 int temp vector
}

// ----- Each node contains:
// int e (4)
// int d (4)
// int neighbor_heights [OUT_VERTICES] (16)
// int residual_capacities [SIZE] (32)
// short sigma [OUT_VERTICES] (12)
// int temp_vector [OUT_VERTICES] (24)
// In total 92 Bytes

#endif