#ifndef GOLDBERG_GRID_CPP
#define GOLDBERG_GRID_CPP

#include <iostream>
#include <stdio.h>
#include <cmath>
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
    const int SIDES = 4;
    const int SIZE = 8;
    const int TEMP1 = 1;
    const int TEMP2 = 2;
    const int SIGMA = 3;
    const int BITSINBYTE = 8;

    
    int copy(int size_in_bytes)
    {
        return 2;
    }

    int inc(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE * 5;
    }

    int add(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE * 10;
    }

    int sub(int size_in_bytes)
    {
        return add(size_in_bytes);
    }

    int max(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE * 10;
    }
    
    int min(int size_in_bytes)
    {
        return max(size_in_bytes);
    }

    int mux(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE * 3;
    }

    int andgate(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE * 3;
    }

    int orgate(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE * 2;
    }
        



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
        //short temp_vector1 [OUT_VERTICES]; // used as temp vector 1 // 12
        int temp_vector [OUT_VERTICES]; // used as temp vector 2 // 24
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
                        //temp_vector1[i] = 0;
                        temp_vector[i] = 0;
                    }
                }
            }

        void swap_LR()
        {
            int temp = sigma[LEFT];
            sigma[LEFT] = sigma[RIGHT];
            sigma[RIGHT] = temp;
        }
        void swap_UD()
        {
            int temp = sigma[UP];
            sigma[UP] = sigma[DOWN];
            sigma[DOWN] = temp;
        }
    };

    void swap_int(int * a, int * b)
    {
        int temp = *a;
        *a = *b;
        *b = temp;
    }

    void swap_short(short * a, short * b)
    {
        short temp = *a;
        *a = *b;
        *b = temp;
    }
    void print_stats()
    {
        std::cout << "________________________" << std::endl;

        std::cout << "sigma_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < OUT_VERTICES; k++)
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
        
        std::cout << "temp_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < OUT_VERTICES; k++)
                {
                    std::cout << nodes[i][j].temp_vector[k] << ", ";
                }
                std::cout << " " << std::endl;
            }  
        }
        /*
        std::cout << "temp2_vector:" << std::endl;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    std::cout << nodes[i][j].temp_vector[k] << ", ";
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
                for (int k = 0; k < OUT_VERTICES; k++)
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
            nodes[idxY][idxX].residual_capacities[UP] = cap;
        else if ( offset_x == -1 && offset_y == 0)
            nodes[idxY][idxX].residual_capacities[LEFT] = cap;
        else if ( offset_x == 0 && offset_y == 1)
            nodes[idxY][idxX].residual_capacities[DOWN] = cap;
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
        latency += copy(sizeof(int));

        // Computation 2 - updating the excess after flow push from source
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].e = nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }
        latency += copy(sizeof(int));

        // Computation 3 - updating the source height + comunication
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].neighbor_heights[TO_SOURCE] = D_S;
            }
        }
        latency += copy(std::ceil(log2(N)));

        // Computation 4 - updating the sum - do in parallel
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                sum += nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }
        latency += add(ceil(std::log2(N)));

        // Computation 5 - updating the out excess - do in parallel time
        E_S -= sum;

        latency += sub(sizeof(sum)); //we only need the 2 complement can we optimize?

        
        // Computation 6 - reseting the residual vertex from source
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                nodes[i][j].residual_capacities[FROM_SOURCE] = 0;
            }
        }
        latency += copy(sizeof(int));
    }

    bool check_excess()
    {
        latency += add(sizeof(int)); // can we optimize - only need to check if zeros
        return !(E_S + E_T == 0);

    }

    void calc_outflow()
    {
        int temp_latency = 0;
        // Computation 1 - Compute the maximum flow to each neighbor in this iteration
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {  
                for (int k = 0; k < OUT_VERTICES; k++)
                {
                    // temp vector 1 used as Cf temp - IDK if it is a legal writing
                    nodes[i][j].temp_vector[k] = nodes[i][j].d - nodes[i][j].neighbor_heights[k]; // compute 
                    nodes[i][j].temp_vector[k] = (nodes[i][j].temp_vector[k] == 1) ? nodes[i][j].residual_capacities[k] :  0; //can be translated to and and not to mux
                }
            }
        }
        temp_latency += (sub(sizeof(int)) + mux(sizeof(int)));

        // Computation 2 - execute suffix sum on temp_voctor1 and save it on temp_vector
        int sum;
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                sum = 0;
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].sigma[k] = sum; // temp vector 2 used as Cf sum
                    sum += nodes[i][j].temp_vector[k];
                }
            }
        }

        temp_latency += std::ceil(log2(OUT_VERTICES)) * add(sizeof(int)); // suffix sum

        // Computation 3 - compute sigma. non neggative flow of max of capacity or excess left 
        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].sigma[k] =  nodes[i][j].e - nodes[i][j].sigma[k]; // can we make all negative in this computation to zeroes?
                    nodes[i][j].sigma[k] = std::min((int)nodes[i][j].sigma[k], (int)nodes[i][j].temp_vector[k]);
                    nodes[i][j].sigma[k] = std::max((int)nodes[i][j].sigma[k], 0);
                }
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].e -= nodes[i][j].sigma[k];
                }
            }
        }

        temp_latency += (sub(sizeof(int)) + min(sizeof(int)) + max (sizeof(int)) + sub(sizeof(int)));
        //cout << "calc : " << temp_latency << endl;
        latency += temp_latency;
    }   

    void push_flow()
    {
        // naive solution  [ 3(copies)*4 (parameters) * H*W (num of nodes) * 2 (mux) ]   time (not including source and sink)
        int temp_latency = 0 ;

        // Computation 7 - adding temp_vector1 to the correct place in Cf FROM_SOURCE
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++){
                nodes[i][j].residual_capacities[FROM_SOURCE] += nodes[i][j].sigma[TO_SOURCE];
                E_S += nodes[i][j].sigma[TO_SOURCE];
            }
        }

        // copy all source flow
        // latency += copy ( sizeof(int));
        //sum reduce
        temp_latency += copy ( sizeof(int)) * ceil(log2(N));
        temp_latency += add ( sizeof(int)) * ceil(log2(N));

        // Computation 8 - adding temp_vector1 to the correct place in Cf FROM_SINK
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++){
                nodes[i][j].residual_capacities[FROM_SINK] += nodes[i][j].sigma[TO_SINK];
                E_T += nodes[i][j].sigma[TO_SINK];
            }
        }

        // copy all source flow
        // latency += copy ( sizeof(int));
        //sum reduce
        temp_latency += copy ( sizeof(int)) * ceil(log2(N));
        temp_latency += add ( sizeof(int)) * ceil(log2(N));


        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++)
            {
                // Computation 1 - subtracting sigma from Cf
                for (int k = RIGHT ; k <= OUT_VERTICES; k++) 
                    nodes[i][j].residual_capacities[k] = nodes[i][j].residual_capacities[k] - nodes[i][j].sigma[k];
            }
        }
        temp_latency += sub(sizeof(int) * OUT_VERTICES);
        //int r = temp_latency;
        //std::cout << "part 1 : " <<  temp_latency << endl;


        int t1 = latency;
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++)
            {
                // Computation 2,3 - swaping the sigma values in every dimension (orange arrow in drawing)
                if(i % 2 == 0)
                    swap_short(&nodes[i][j].sigma[UP], &nodes[i][j].sigma[DOWN]);

                if(j % 2 == 0)
                    swap_short(&nodes[i][j].sigma[LEFT], &nodes[i][j].sigma[RIGHT]);       

            }
        }
        // Checking if even requires checking LSB for one dimension, but checking another bit for the other dimension
        latency += 2 * andgate(sizeof(int));
        // Assuming 3 copies to swap, and 2 dimensions
        latency += copy(1) * 3 * 2;

        // Move the info between the nodes (purple arrow in drawing)
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++)
            {
                if(i % 2 == 0)
                {
                    // Communication 1,2,3,4 - mixing the sigma values in to the corresponding nodes
                    if ( i < H - 1) 
                        swap_short(&nodes[i][j].sigma[UP], &nodes[i + 1][j].sigma[UP]);
                    
                    if ( i > 0 )
                        swap_short(&nodes[i][j].sigma[DOWN], &nodes[i - 1][j].sigma[DOWN]);
                }
                if(j % 2 == 0)
                {
                    if ( j < W - 1) 
                        swap_short(&nodes[i][j].sigma[LEFT], &nodes[i][j + 1].sigma[LEFT]);

                    if ( j > 0 )
                        swap_short(&nodes[i][j].sigma[RIGHT], &nodes[i][j - 1].sigma[RIGHT]);
                }
            }
        }
        // Checking if even requires checking LSB for one dimension, but checking another bit for the other dimension
        latency += 2 * andgate(sizeof(int));
        // Communication costs
        latency += W * H * 4;

        //second mix of the odd nodes (red arrow in drawing)
        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++)
            {
                // Computation 4,5 - reswaping the sigma values in every dimension
                if(i % 2 == 0)
                    swap_short(&nodes[i][j].sigma[UP], &nodes[i][j].sigma[DOWN]);
                if(j % 2 == 0)
                    swap_short(&nodes[i][j].sigma[LEFT], &nodes[i][j].sigma[RIGHT]);          
            }
        }
        // Checking if even requires checking LSB for one dimension, but checking another bit for the other dimension
        latency += 2 * andgate(sizeof(int));
        // Assuming 3 copies to swap, and 2 dimensions
        latency += copy(1) * 3 * 2;
        //Putting the sigma into place

        for(int i = 0; i < H; i++){
            for(int j = 0; j < W; j++)
            {
                // Computation 6,7 - adding the sigma values to the correct vectors
                if ( i < H - 1) 
                {
                    nodes[i][j].residual_capacities[DOWN] += nodes[i][j].sigma[DOWN];
                    nodes[i][j].e += nodes[i][j].sigma[DOWN];
                }
                if ( i > 0 )
                {
                    nodes[i][j].residual_capacities[UP] += nodes[i][j].sigma[UP];
                    nodes[i][j].e += nodes[i][j].sigma[UP];                     
                }
                if ( j < W - 1) 
                {
                    nodes[i][j].residual_capacities[RIGHT] += nodes[i][j].sigma[RIGHT];
                    nodes[i][j].e += nodes[i][j].sigma[RIGHT];
                }
                if ( j > 0 )
                {
                    nodes[i][j].residual_capacities[LEFT] += nodes[i][j].sigma[LEFT];
                    nodes[i][j].e += nodes[i][j].sigma[LEFT];
                }
            }
        }
        // 8 adds in total
        latency += 8 * add(sizeof(int));

        latency += temp_latency;
    }


    void relabel()
    {
        int temp_latency = 0;
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
                    nodes[i][j].temp_vector[k] = (nodes[i][j].residual_capacities[k] == 0) ? MAX_HEIGHT : 0;
                    
                    // Computation 2 - adding the neightbors heights
                    nodes[i][j].temp_vector[k] += nodes[i][j].neighbor_heights[k];

                    // Computation 3 - comparing the min to the current height
                    min = std::min(min, nodes[i][j].temp_vector[k]);
                }

                // Computation 4 - every node gets its height from the temp var
                // "min" plus 1
                nodes[i][j].d = min + 1;
            }
        }


        // copy all maxheight
        temp_latency += copy( sizeof(int)) * ceil(log2(sizeof(OUT_VERTICES)));
        // calculte heights
        temp_latency += (mux ( sizeof(int)) + add( sizeof(int)));
        //choose min height - reduce
        temp_latency += (grid::min( sizeof(int)) * ceil(log2(sizeof(OUT_VERTICES))));


        for(int i = 0; i < H; i++)
        {
            for(int j = 0; j < W; j++)
            {

                // Computation 5 - copying this nodes height value to the temp vector
                nodes[i][j].temp_vector[RIGHT] = nodes[i][j].d;
                nodes[i][j].temp_vector[DOWN] = nodes[i][j].d;
                nodes[i][j].temp_vector[LEFT] = nodes[i][j].d;
                nodes[i][j].temp_vector[UP] = nodes[i][j].d;

                if ( j < W - 1)
                {
                    // Communication 1 - placing the height value in the RIGHT node
                    nodes[i][j + 1].temp_vector[LEFT] = nodes[i][j].temp_vector[RIGHT];

                    // Computation 6 - inserting the height value in the correct place
                    nodes[i][j + 1].neighbor_heights[LEFT] = nodes[i][j + 1].temp_vector[LEFT];
                }
                
                if ( i < H - 1)
                {
                    // Communication 2 - placing the height value in the DOWN node
                    nodes[i + 1][j].temp_vector[UP] = nodes[i][j].temp_vector[DOWN];

                    // Computation 7 - inserting the height value in the correct place
                    nodes[i + 1][j].neighbor_heights[UP] = nodes[i + 1][j].temp_vector[UP];
                }

                if ( j > 0 )
                {
                    // Communication 3 - placing the height value in the LEFT node
                    nodes[i][j - 1].temp_vector[RIGHT] = nodes[i][j].temp_vector[LEFT];

                    // Computation 8 - inserting the height value in the correct place
                    nodes[i][j - 1].neighbor_heights[RIGHT] = nodes[i][j - 1].temp_vector[RIGHT];
                }

                if ( i > 0 )
                {
                    // Communication 4 - placing the height value in the DOWN node
                    nodes[i - 1][j].temp_vector[DOWN] = nodes[i][j].temp_vector[UP];

                    // Computation 9 - inserting the height value in the correct place
                    nodes[i - 1][j].neighbor_heights[DOWN] = nodes[i - 1][j].temp_vector[DOWN];
                }
            }

            // copy the height to temp vector
            temp_latency += copy( sizeof(int)) * ceil(log2(sizeof(SIGMA + 1)));

            // copy the height to the four neighbors
            temp_latency += copy( sizeof(int)) * (SIGMA+1);

            // copy the height to the right place
            temp_latency += copy( sizeof(int)) * (SIGMA+1);
        }

        //cout << "relabel : " << temp_latency << endl;
        latency += temp_latency;
    }


    int goldberg_grid(int width, int height, int maxflow, bool details){
        
        // Initialization 
        latency = 0;
        initializations(width, height);       
        pre_flow();
        //print_stats();
        //cout << "preflow works" << endl;
        int i = 0;
        while(check_excess())
        {
            //cout << "ITER: " << i << endl;
            //cout << "E_S: " << E_S << ", E_T: " << E_T << endl;
            calc_outflow();
           // print_stats();
            push_flow();
            //print_stats();
            //if(i == 2)
            //    return -1;
            relabel();
            //cout << "all func work" << endl;
            //update_excess();
            i++; 
            if (i % 10000 == 0)
            {
                cout << i << ", ";
            } 

        }
        cout << endl;
        if( details)
            cout << "number of iterations: " << i << endl;
        if ( maxflow != 0 && maxflow != E_T)
        {
            cout << "XXXXX ERROR XXXXX" << endl;
            print_stats();
            cout << "max flow grid: " << E_T << "  max flow serial: " <<  maxflow << endl;
            return -1;
        }
        else
        {
            cout << "----- CORRECT -----" << endl;
            cout << "\nnumber of cycles: " << latency << "\n"  << "number of iteration: "  << i << endl;
            return E_T;
        }
        
    }

    // *** pre_flow - Uses neighbor_heights vector
    // *** calc_outflow - Uses two temp_vectors with 6 ints together and neighbor_heights, and then
    // a temp vector with 6 ints and sigma together, maybe we should use just 
    // sigma and one temp_vector -- added in this version
    // *** push_flow - Uses sigma and temp_vector that uses 4 ints to communicate
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