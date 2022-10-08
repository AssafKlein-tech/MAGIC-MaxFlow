#include "goldberg.cpp"
#include "goldberg_grid.cpp"
#include "graph_gen.cpp"
#include <chrono>
// vector<vector<int>> capacity, flow;
graph flow, capacity;
vi height, excess, seen;
queue<int> excess_vertices;
int n;
int M = 20;

// int n = 6;
// graph capacity{{0, 16, 13, 0, 0, 0},
//                 {0, 0, 10, 12, 0, 0},
//                 {0, 4, 0, 0, 14, 0},
//                 {0, 0, 9, 0, 0, 20},
//                 {0, 0, 0, 7, 0, 4},
//                 {0, 0, 0, 0, 0, 0}};

// int n = 3;
// vector<vector<int>> capacity{{0, 3, 0},
//                             {0, 0, 2},
//                             {0, 0, 0}};


void push(int u, int v)
{
    int d = min(excess[u], capacity[u][v] - flow[u][v]);
    flow[u][v] += d;
    flow[v][u] -= d;
    excess[u] -= d;
    excess[v] += d;
    if (d && excess[v] == d)
        excess_vertices.push(v);
}

void relabel(int u)
{
    int d = grid::INF;
    for (int i = 0; i < n; i++) {
        if (capacity[u][i] - flow[u][i] > 0)
            d = min(d, height[i]);
    }
    if (d < grid::INF)
        height[u] = d + 1;
}

void discharge(int u)
{
    while (excess[u] > 0) {
        if (seen[u] < n) {
            int v = seen[u];
            if (capacity[u][v] - flow[u][v] > 0 && height[u] > height[v])
                push(u, v);
            else 
                seen[u]++;
        } else {
            relabel(u);
            seen[u] = 0;
        }
    }
}

int max_flow(int s, int t)
{
    height.assign(n, 0);
    height[s] = n;
    flow.assign(n, vector<int>(n, 0));
    excess.assign(n, 0);
    excess[s] = grid::INF;
    for (int i = 0; i < n; i++) {
        if (i != s)
            push(s, i);
    }
    seen.assign(n, 0);

    while (!excess_vertices.empty()) {
        int u = excess_vertices.front();
        excess_vertices.pop();
        if (u != s && u != t)
            discharge(u);
    }

    int max_flow = 0;
    for (int i = 0; i < n; i++)
        max_flow += flow[i][t];
    return max_flow;
}

void create_nodes(graph capacity, int w, int h)
{
    int n = w * h;
    grid::initialize_nodes(w,h);
    int index;
    for(int y=0;y<h;y++)
    for(int x=0;x<w;x++)
    {
        index = x+y*w + 1;
        grid::set_terminal_cap(x, y, capacity[0][index], capacity[index][n + 1]);

        if (x>0  ) // Left
            grid::set_neighbor_cap(x,y,-1, 0,capacity[index][index - 1]);

        if (x<w-1) // right
            grid::set_neighbor_cap(x,y,+1, 0,capacity[index][index + 1]);

        if (y>0  ) // up
            grid::set_neighbor_cap(x,y, 0,+1,capacity[index][index - w]);

        if (y<h-1) // down
            grid::set_neighbor_cap(x,y, 0,-1,capacity[index][index + w]);

    }                

}




int main()
{
    chrono::steady_clock sc;
    auto start = sc.now();
    auto end = sc.now();
    double serial_time;
    double parallel_time;
    auto time_span = static_cast<chrono::duration<double>>(end - start);
    int fail_counter = 0;
    double speedup_sum = 0, potential_speedup;
    for(int i = 0; i < M; ++i)
    {
        // graph_gen(&capacity, &n);
        int width, height;
        grid_graph_gen(&capacity, &width, &height, &n);
        capacity[0][width*height +1] = 0;
        cout << "\ngraph size:" << n << endl;
        start = sc.now();     // start timer
        int maxflow = max_flow(0, n-1);
        end = sc.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
        time_span = static_cast<chrono::duration<double>>(end - start);
        serial_time = time_span.count();
        cout << "sequential maxflow:" << maxflow  << " time:" << serial_time << endl;
        //start = sc.now();
        //maxflow = goldberg(capacity, n);
        //end = sc.now();
        //time_span = static_cast<chrono::duration<double>>(end - start);
        //parallel_time = time_span.count();
        //cout << "matrix maxflow:" << maxflow << " time:" << parallel_time << endl;
        create_nodes(capacity, width, height);
        start = sc.now();
        maxflow = grid::goldberg_grid(width, height, maxflow);
        if ( maxflow == -1)
        {
            print_C();
            fail_counter++;
        }
        end = sc.now();
        time_span = static_cast<chrono::duration<double>>(end - start);
        parallel_time = time_span.count();
        cout << "grid matrix maxflow:" << maxflow << " time:" << parallel_time << endl;
        potential_speedup = serial_time / (parallel_time/n);
        cout << "potential speedup:" << potential_speedup << endl;
        speedup_sum += potential_speedup;
    }
    cout << "\n\nAverage potential speedup: " << (speedup_sum / M) << endl;
    if(fail_counter == 0)
        cout << "All " << M << " runs passed :)" << endl;
    else
    {
        cout << "failed runs:" << fail_counter << endl;
        cout << "total runs:" << M << endl;
    }

    return 0;
}

