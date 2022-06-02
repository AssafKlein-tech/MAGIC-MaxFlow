#ifndef GOLDBERG_CPP
#define DEFINE_CPP

#include <iostream>
#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
using namespace std;

const int inf = 1000000000;
typedef vector<int> vi;
typedef vector<vi> graph;

// Declarations
graph Cf, D, C, sigma, F;
vi d, e;
int N;

void initializations(int s, graph capacity, int num_vertices)
{
    // Initializations

    N = num_vertices;
    F.assign(N, vi(N, 0));
    D.assign(N, vi(N, 0));
    F.assign(N, vi(N, 0));
    C = capacity;
    Cf = capacity;
    e.assign(N, 0);
    e[s] = inf;
    
}

void pre_flow()
{
    d[0] = N;
    for (int i = 0; i < N; i++)
    {
        D[i][0] = d[0];
        vi temp_preflow = Cf[0];
        (Cf[0]).assign(N, 0);

        int sum = 0;

        for (int i = 0; i < N; i++)
        {
            Cf[0][i] = temp_preflow[i];
            sum += temp_preflow[i];
        }

        e = temp_preflow;
        e[0] = -sum;
    }

}

bool check_excess()
{
    // Returns true if theres at least one non-zero value in excess, excluding {s,t}
    for (int i = 1; i < N - 1; i++)
    {
        if (e[i] != 0)
            return true;
    }
    return false;
}

void calc_outflow()
{
    graph D_binary, Cf_temp;
    D_binary.assign(N, vi(N, 0));
    Cf_temp.assign(N, vi(N, 0));

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            D_binary[i][j] = ((D[i][j] - d[i]) == 1) ? 1 : 0;
            Cf_temp[i][j] = D_binary[i][j] * Cf[i][j];
        }
    }

    int temp, sum;

    for (int i = 0; i < N; i++)
    {
        temp = 0;
        sum = 0;
        for (int j = N-1; j >= 0; j--)
        {
            temp = Cf_temp[i][j];
            Cf_temp[i][j] = sum;
            sum += temp;
            // Take the minimum
            sigma[i][j] = ((e[i] - Cf_temp[i][j]) < Cf_temp[i][j]) ? (e[i] - Cf_temp[i][j]) : Cf_temp[i][j];
            // Calc the excess leftover
            e[i] -= sigma[i][j];
        }
    }
}   

void push_flow()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            F[i][j] += sigma[i][j];
            Cf[i][j] -= sigma[i][j];
            // Transposed operations
            F[i][j] += sigma[j][i];
            Cf[i][j] -= sigma[j][i];
        }
    }
}

void relabel()
{
    graph new_D, Cf_temp;
    new_D.assign(N, vi(N, 0));
    Cf_temp.assign(N, vi(N, 0));
    int min;
    for (int i = 1; i < N-1; i++)
    {
        min = 2 * N;
        for (int j = 0; j < N; j++)
        {
            new_D[i][j] = 2 * N;
            Cf_temp[i][j] = (Cf[i][j] == 0) ? 1 : 0;
            new_D[i][j] *= Cf_temp[i][j];
            new_D[i][j] += D[i][j];
            min = (min <= new_D[i][j]) ? min : new_D[i][j];
        }
        d[i] = min + 1;
    }

    for (int i = 0; i < N; i++)
    {
        D[i] = d;
    }
}

void update_excess()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            e[i] -= sigma[j][i];
        }
    }
}

int goldberg(graph capacity, int num_vertices)
{
    initializations(0, capacity, num_vertices);
    pre_flow();

    while(check_excess())
    {
        sigma.assign(N, vi(N, 0));
        calc_outflow();
        push_flow();
        relabel();
        update_excess();
    }
    return e[N-1];
}

#endif