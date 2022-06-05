#include "goldberg.cpp"
#include "graph_gen.cpp"
// vector<vector<int>> capacity, flow;
graph flow, capacity;
vi height, excess, seen;
queue<int> excess_vertices;
int n;

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
    int d = inf;
    for (int i = 0; i < n; i++) {
        if (capacity[u][i] - flow[u][i] > 0)
            d = min(d, height[i]);
    }
    if (d < inf)
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
    excess[s] = inf;
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
    graph_gen(&capacity, &n);
    int maxflow = max_flow(0, n-1);
    cout << maxflow << endl;
    // string s = to_string(maxflow);
    // printf("%s", s);
    maxflow = goldberg(capacity, n);
    cout << maxflow << endl;
    return 0;
}

