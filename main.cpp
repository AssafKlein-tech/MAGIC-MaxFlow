#include "goldberg.cpp"
#include "goldberg_grid.cpp"
#include "graph_gen.cpp"
#include <chrono>
// vector<vector<int>> capacity, flow;
graph flow, capacity;
vi height, excess, seen;
queue<int> excess_vertices;
int n;
int M = 1;

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


int main()
{
    chrono::steady_clock sc;
    auto start = sc.now();
    auto end = sc.now();
    double serial_time;
    double parallel_time;
    auto time_span = static_cast<chrono::duration<double>>(end - start);
    for(int i = 0; i< M; ++i)
    {
        // graph_gen(&capacity, &n);
        int width, height;
        grid_graph_gen(&capacity, &width, &height, &n);
        cout << "\ngraph size:" << n << endl;
        //start = sc.now();     // start timer
       // int maxflow = max_flow(0, n-1);
        //end = sc.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
        //time_span = static_cast<chrono::duration<double>>(end - start);
        //serial_time = time_span.count();
        //cout << "sequential maxflow:" << maxflow  << " time:" << serial_time << endl;
        //start = sc.now();
        //maxflow = goldberg(capacity, n);
        //end = sc.now();
       // time_span = static_cast<chrono::duration<double>>(end - start);
        //parallel_time = time_span.count();
        //cout << "matrix maxflow:" << maxflow << " time:" << parallel_time << endl;
        start = sc.now();
        int maxflow = grid::goldberg_grid(capacity, width, height);
        end = sc.now();
        time_span = static_cast<chrono::duration<double>>(end - start);
        parallel_time = time_span.count();
        cout << "grid matrix maxflow:" << maxflow << " time:" << parallel_time << endl;
        //cout << "potential speedup:" << serial_time / (parallel_time/n) << endl;

    }
    return 0;
}

