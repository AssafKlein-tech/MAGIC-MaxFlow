#ifndef GOLDBERG_GRID_CPP
#define GOLDBERG_GRID_CPP

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <queue>
#include <string>
#include <list>
#include "dfs.cpp"

using namespace std;

namespace grid
{

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
    const int BITSINBYTE = 8;
    const int TILE_WIDTH = 32;


    //Latency calculation function
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

    int norgate(int size_in_bytes)
    {
        return size_in_bytes * BITSINBYTE;
    }

    int swap(int size_in_bytes)
    {
        return 4;
    }

    typedef vector<int> vi;
    typedef vector<vi> graph;

    // Declarations
    long latency = 0;
    int E_S, E_T, D_S, D_T;
    int N, W, H, S, T;

    struct Node; // 104
    vector<vector<Node>> nodes;
    struct Node
    {
        int e;                                      //  4
        int d;                                      //  4
        int neighbor_heights[OUT_VERTICES];         // 16
        short residual_capacities[SIZE];            // 16
        short sigma[OUT_VERTICES];                  // 12
        int temp_vector[OUT_VERTICES];              // 24
        // D_S - saved in any node in the mem model //  4
                                            // overall 80 for each node

        void swap_LR()
        {
            short temp = sigma[LEFT];
            sigma[LEFT] = sigma[RIGHT];
            sigma[RIGHT] = temp;
        }
        void swap_UD()
        {
            short temp = sigma[UP];
            sigma[UP] = sigma[DOWN];
            sigma[DOWN] = temp;
        }
    };

    void swap_int(int *a, int *b)
    {
        int temp = *a;
        *a = *b;
        *b = temp;
    }

    void swap_short(short *a, short *b)
    {
        short temp = *a;
        *a = *b;
        *b = temp;
    }

    // initialization functions
    void initialize_nodes(int w, int h)
    {
        W = w;
        H = h;
        vector<Node> temp_v;
        nodes.clear();
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                temp_v.push_back(Node());
            }
            nodes.push_back(temp_v);
            temp_v.clear();
        }
    }

    void set_terminal_cap(int idxX, int idxY, int cap_source, int cap_sink)
    {
        nodes[idxY][idxX].residual_capacities[FROM_SOURCE] = cap_source;
        nodes[idxY][idxX].residual_capacities[TO_SINK] = cap_sink;
    }

    void set_neighbor_cap(int idxX, int idxY, int offset_x, int offset_y, int cap)
    {
        if (offset_x == 1 && offset_y == 0)
            nodes[idxY][idxX].residual_capacities[RIGHT] = cap;
        else if (offset_x == 0 && offset_y == -1)
            nodes[idxY][idxX].residual_capacities[UP] = cap;
        else if (offset_x == -1 && offset_y == 0)
            nodes[idxY][idxX].residual_capacities[LEFT] = cap;
        else if (offset_x == 0 && offset_y == 1)
            nodes[idxY][idxX].residual_capacities[DOWN] = cap;
        else
            throw std::invalid_argument("received non grid offsets");
    }

    void initializations()
    {
        N = W * H;
        E_S = 0;
        E_T = 0;
        D_S = N + 2;
        D_T = 0;
    }

    //builds graph to dfs and invokes the calculation
    int graph_dfs()
    {
        const int sink = W * H + 1;
        Graph g(2 + W * H);
        int index;
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                index = 1 + j + i * W;
                if (nodes[i][j].residual_capacities[FROM_SOURCE] != 0)
                {
                    g.addEdge(0,index);
                    //cout << index << endl;
                }
                if (nodes[i][j].residual_capacities[TO_SINK] != 0)
                    g.addEdge(index, sink);
                if (j < W-1 && nodes[i][j].residual_capacities[RIGHT] != 0)
                    g.addEdge(index, index + 1);
                if (i > 0 && nodes[i][j].residual_capacities[DOWN] != 0)
                    g.addEdge(index, index + W);
                if (j > 0 && nodes[i][j].residual_capacities[LEFT] != 0)
                    g.addEdge(index, index - 1);
                if (i < H-1 && nodes[i][j].residual_capacities[UP] != 0)
                    g.addEdge(index, index - W);
            }
        }
        g.DFS(0);
        return source_cut.size();
    }


    // PPPR functions
    void pre_flow()
    {
        int sum = 0;

        // Computation 1 - pushing the flow from the source, reversing the residual vertex direction
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                nodes[i][j].residual_capacities[TO_SOURCE] = nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }
        latency += copy(sizeof(short));

        // Computation 2 - updating the excess after flow push from source
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                nodes[i][j].e = nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }
        latency += copy(sizeof(short));

        // Computation 3 - Broudcasting the source height
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                nodes[i][j].neighbor_heights[TO_SOURCE] = D_S;
            }
        }
        latency += copy(sizeof(int));

        // Computation 4 - updating the sum - do in parallel
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                sum += nodes[i][j].residual_capacities[FROM_SOURCE];
            }
        }
        E_S -= sum;
        latency += add(sizeof(short)) * ceil(std::log2(N));


        // Computation 6 - reseting the residual vertex from source
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                nodes[i][j].residual_capacities[FROM_SOURCE] = 0;
            }
        }
        latency += copy(sizeof(short));
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
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                for (int k = 0; k < OUT_VERTICES; k++)
                {
                    // temp vector 1 used as Cf temp - IDK if it is a legal writing
                    nodes[i][j].temp_vector[k] = nodes[i][j].d - nodes[i][j].neighbor_heights[k];                            // compute
                    nodes[i][j].temp_vector[k] = (nodes[i][j].temp_vector[k] == 1) ? nodes[i][j].residual_capacities[k] : 0; // can be translated to and and not to mux
                }
            }
        }
        temp_latency += (sub(sizeof(int)) + andgate(sizeof(int)) + 1);

        // Computation 2 - execute suffix sum on temp_voctor1 and save it on temp_vector
        int sum;
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                sum = 0;
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].sigma[k] = sum; // temp vector 2 used as Cf sum
                    sum += nodes[i][j].temp_vector[k];
                }
            }
        }

        temp_latency += std::ceil(log2(OUT_VERTICES)) * add(sizeof(short)); // suffix sum

        // Computation 3 - compute sigma. non neggative flow of max of capacity or excess left
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].sigma[k] = nodes[i][j].e - nodes[i][j].sigma[k]; // can we make all negative in this computation to zeroes?
                    nodes[i][j].sigma[k] = std::min((int)nodes[i][j].sigma[k], (int)nodes[i][j].temp_vector[k]);
                    nodes[i][j].sigma[k] = std::max((int)nodes[i][j].sigma[k], 0);
                }
                for (int k = OUT_VERTICES - 1; k >= 0; k--)
                {
                    nodes[i][j].e -= nodes[i][j].sigma[k];
                }
            }
        }

        temp_latency += (sub(sizeof(short)) + min(sizeof(short)) + max(sizeof(short)) + sub(sizeof(short)));
        //cout << "calc : " << temp_latency << endl;
        latency += temp_latency;
    }

    void push_flow()
    {
        // Computation 1 - adding sigma to the correct place in Cf FROM_SOURCE, FROM_SINK and adding the sum to E_T and E_S
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                nodes[i][j].residual_capacities[FROM_SOURCE] += nodes[i][j].sigma[TO_SOURCE];
                nodes[i][j].residual_capacities[FROM_SINK] += nodes[i][j].sigma[TO_SINK];
                E_T += nodes[i][j].sigma[TO_SINK];
                E_S += nodes[i][j].sigma[TO_SOURCE];
            }
        }
        // sum reduce + adding to residual FS FT
        latency += 2 * copy(sizeof(short)) * ceil(log2(N));
        latency += 2 * add(sizeof(short)) * (ceil(log2(N))+1) ;

        // Computation 2 - subtracting sigma from Cf
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                for (int k = RIGHT; k < OUT_VERTICES; k++)
                    nodes[i][j].residual_capacities[k] = nodes[i][j].residual_capacities[k] - nodes[i][j].sigma[k];
            }
        }
        latency += sub(sizeof(short)) * OUT_VERTICES;

        // Computation 2,3 - swaping the sigma values in every dimension (orange arrow in drawing) 
        //(RIGHT<->LEFT, DOEN <->UP) only on the even nodes in every dimension
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                if (i % 2 == 0)
                    swap_short(&nodes[i][j].sigma[UP], &nodes[i][j].sigma[DOWN]);

                if (j % 2 == 0)
                    swap_short(&nodes[i][j].sigma[LEFT], &nodes[i][j].sigma[RIGHT]);
            }
        }

        // Assuming 2 copies to swap, and 2 dimensions
        latency += swap(sizeof(short)) * (SIDES/2) * sizeof(short) * BITSINBYTE;

        // Move the info between the nodes (purple arrow in drawing)
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                if (i % 2 == 0)
                {
                    // Communication 1,2,3,4 - mixing the sigma values in to the corresponding nodes
                    if (i < H - 1)
                        swap_short(&nodes[i][j].sigma[UP], &nodes[i + 1][j].sigma[UP]);

                    if (i > 0)
                        swap_short(&nodes[i][j].sigma[DOWN], &nodes[i - 1][j].sigma[DOWN]);
                }
                if (j % 2 == 0)
                {
                    if (j < W - 1)
                        swap_short(&nodes[i][j].sigma[LEFT], &nodes[i][j + 1].sigma[LEFT]);

                    if (j > 0)
                        swap_short(&nodes[i][j].sigma[RIGHT], &nodes[i][j - 1].sigma[RIGHT]);
                }
            }
        }
        
        latency += TILE_WIDTH * TILE_WIDTH * (swap(sizeof(short))) * (SIDES/2);
        
        // second mix of the odd nodes (red arrow in drawing)
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                // Computation 4,5 - reswaping the sigma values in every dimension
                if (i % 2 == 0)
                    swap_short(&nodes[i][j].sigma[UP], &nodes[i][j].sigma[DOWN]);
                if (j % 2 == 0)
                    swap_short(&nodes[i][j].sigma[LEFT], &nodes[i][j].sigma[RIGHT]);
            }
        }

        latency += swap(sizeof(short)) * (SIDES/2) * sizeof(short) * BITSINBYTE;
        
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {
                // Computation 6,7 - adding the sigma values to the correct vectors
                if (i < H - 1)
                {
                    nodes[i][j].residual_capacities[DOWN] += nodes[i][j].sigma[DOWN];
                    nodes[i][j].e += nodes[i][j].sigma[DOWN];
                }
                if (i > 0)
                {
                    nodes[i][j].residual_capacities[UP] += nodes[i][j].sigma[UP];
                    nodes[i][j].e += nodes[i][j].sigma[UP];
                }
                if (j < W - 1)
                {
                    nodes[i][j].residual_capacities[RIGHT] += nodes[i][j].sigma[RIGHT];
                    nodes[i][j].e += nodes[i][j].sigma[RIGHT];
                }
                if (j > 0)
                {
                    nodes[i][j].residual_capacities[LEFT] += nodes[i][j].sigma[LEFT];
                    nodes[i][j].e += nodes[i][j].sigma[LEFT];
                }
            }
        }
        // 8 adds in total
        latency += 2 * SIDES * add(sizeof(short));
    }

    void relabel()
    {
        int temp_latency = 0;
        int MAX_HEIGHT = 2 * (N + 2), min;
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
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

        // checks if risdual k is 0
        latency += norgate(sizeof(short)) * OUT_VERTICES;
        // broucast AND with max height
        latency += andgate(sizeof(short)) * OUT_VERTICES;
        // calculte heights
        latency +=  add(sizeof(int)) * OUT_VERTICES ;
        // choose min height - reduce
        latency += (grid::min(sizeof(int)) * ceil(log2(OUT_VERTICES)));
        //add 1 to height
        latency +=  add(sizeof(int)) * OUT_VERTICES;

        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < W; j++)
            {

                // Computation 5 - copying this nodes height value to the temp vector
                nodes[i][j].temp_vector[RIGHT] = nodes[i][j].d;
                nodes[i][j].temp_vector[DOWN] = nodes[i][j].d;
                nodes[i][j].temp_vector[LEFT] = nodes[i][j].d;
                nodes[i][j].temp_vector[UP] = nodes[i][j].d;

                if (j < W - 1)
                {
                    // Communication 1 - placing the height value in the RIGHT node
                    nodes[i][j + 1].temp_vector[LEFT] = nodes[i][j].temp_vector[RIGHT];

                    // Computation 6 - inserting the height value in the correct place
                    nodes[i][j + 1].neighbor_heights[LEFT] = nodes[i][j + 1].temp_vector[LEFT];
                }

                if (i < H - 1)
                {
                    // Communication 2 - placing the height value in the DOWN node
                    nodes[i + 1][j].temp_vector[UP] = nodes[i][j].temp_vector[DOWN];

                    // Computation 7 - inserting the height value in the correct place
                    nodes[i + 1][j].neighbor_heights[UP] = nodes[i + 1][j].temp_vector[UP];
                }

                if (j > 0)
                {
                    // Communication 3 - placing the height value in the LEFT node
                    nodes[i][j - 1].temp_vector[RIGHT] = nodes[i][j].temp_vector[LEFT];

                    // Computation 8 - inserting the height value in the correct place
                    nodes[i][j - 1].neighbor_heights[RIGHT] = nodes[i][j - 1].temp_vector[RIGHT];
                }

                if (i > 0)
                {
                    // Communication 4 - placing the height value in the DOWN node
                    nodes[i - 1][j].temp_vector[DOWN] = nodes[i][j].temp_vector[UP];

                    // Computation 9 - inserting the height value in the correct place
                    nodes[i - 1][j].neighbor_heights[DOWN] = nodes[i - 1][j].temp_vector[DOWN];
                }
            }
        }
        //brouudcast height to 4 neighbors
        latency += copy(sizeof(int)) * SIDES;
    }

    int goldberg_grid(int maxflow, bool details)
    {

        // Initialization
        latency = 0;
        initializations();
        pre_flow();

        int i = 0;
        while (check_excess())
        {
            calc_outflow();
            push_flow();
            relabel();
            i++;
            if (i % 10000 == 0)
            {
                cout << i << endl;
            }
        }

        if (details)
            cout << "number of iterations: " << i << endl;
        if (maxflow != 0 && maxflow != E_T)
        {
            cout << "XXXXX ERROR XXXXX" << endl;
            cout << "max flow grid: " << E_T << "  max flow serial: " << maxflow << endl;
            return -1;
        }
        else
        {
            cout << "----- CORRECT -----" << endl;
            int cut = graph_dfs();
            cout << "\nnumber of cycles: " << latency << "\n"
                 << "number of iteration: " << i << "\nsourcecut: " << cut << endl;
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
// short residual_capacities [SIZE] (16)
// short sigma [OUT_VERTICES] (12)
// int temp_vector [OUT_VERTICES] (24)
// int D_S (4)
// In total 80 Bytes

#endif