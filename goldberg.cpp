#ifndef GOLDBERG_CPP
#define GOLDBERG_CPP

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


void print_e()
{
    cout<< "e_vector:" << endl;
    for(size_t i = 0; i < e.size(); i++)
        cout << (e[i]) << endl;
}

void print_sigma()
{
    cout<< "sigma:" << endl;
    for(size_t i = 0; i < sigma.size(); i++)
    {
        for(size_t j = 0; j < sigma[i].size(); j++)
            cout << (sigma[i][j]) << " ";
        cout << endl;
    }
}

void print_D()
{
    cout<< "D:" << endl;
    for(size_t i = 0; i < D.size(); i++)
    {
        for(size_t j = 0; j < D[i].size(); j++)
            cout << (D[i][j]) << " ";
        cout << endl;
    }
}

void print_Cf()
{
    cout<< "Cf:" << endl;
    for(size_t i = 0; i < Cf.size(); i++)
    {
        for(size_t j = 0; j < Cf[i].size(); j++)
            cout << (Cf[i][j]) << " ";
        cout << endl;
    }
}
void print_C()
{
    cout<< "C:" << C.size() << endl;

    for(size_t i = 0; i < C.size(); i++)
    {
        for(size_t j = 0; j < C[i].size(); j++)
            cout << (C[i][j]) << " ";
        cout << endl;
    }
}
void print_F()
{
    cout<< "F:" << endl;
    for(size_t i = 0; i < F.size(); i++)
    {
        for(size_t j = 0; j < F[i].size(); j++)
        {
            if (F[i][j] > 0)
                cout << (F[i][j]) << " ";
            else  
                cout << '0' << " ";  
        }   
        cout << endl;
    }
}

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
    d.assign(N, 0);
    e[s] = inf;
    
}

void pre_flow()
{
    d[0] = N;
    int sum = 0;
    vi temp_preflow = Cf[0];
    (Cf[0]).assign(N, 0);
    for (int i = 0; i < N; i++)
    {
        D[i][0] = d[0];
        F[0][i] = temp_preflow[i];
        F[i][0] = -temp_preflow[i];
        Cf[i][0] = temp_preflow[i];
        sum += temp_preflow[i];

        e = temp_preflow;
    }
    e[0] = -sum;
    //print_e();
    //print_Cf();
}

//e(s) + e(t) = 0
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
    graph D_binary, Cf_temp, Cf_sum;
    D_binary.assign(N, vi(N, 0));
    Cf_temp.assign(N, vi(N, 0));
    Cf_sum.assign(N, vi(N, 0));
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            D_binary[i][j] = ((d[i] - D[i][j]) == 1) ? 1 : 0;
            Cf_temp[i][j] = D_binary[i][j] * Cf[i][j];
        }
    }

    int sum;

    for (int i = 0; i < N; i++)
    {
        sum = 0;
        for (int j = N-1; j >= 0; j--)
        {
            // prefix sum
            Cf_sum[i][j] = sum;
            sum += Cf_temp[i][j];
            // Take the minimum
            sigma[i][j] = ((e[i] - Cf_sum[i][j]) < Cf_temp[i][j]) ? (e[i] - Cf_sum[i][j]) : Cf_temp[i][j];
            sigma[i][j] = (sigma[i][j] > 0) ? sigma[i][j] : 0;
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
            F[i][j] -= sigma[j][i];
            Cf[i][j] += sigma[j][i];
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
        for (int j = 0; j < N; j++)
        {
            D[i][j] = d[j];
        }
    }
}

void update_excess()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            e[i] += sigma[j][i];
        }
    }
}

int goldberg(graph capacity, int num_vertices)
{
    initializations(0, capacity, num_vertices);
    //print_C();
    pre_flow();
    //print_F();
    int i = 0;
    while(check_excess())
    {

        sigma.assign(N, vi(N, 0));
        calc_outflow();
        push_flow();
       // print_Cf();
        //print_F();
        relabel();
        update_excess();
        i++;
    }
    //print_Cf();
    //print_F();
    cout << "number of iterations: " << i << endl;
    return e[N-1];
}

#endif




// struct Node{
//     uint8_t capacities[4];
//     uint8_t flow_out [4];
//     uint8_t neighbor_heights[4];
// }

// void main(){


//     vector<vector<Node>> nodes = ...

//     // Initialization 
//     ...


//     // Pre flow

//     while(true){


//         for(int i = 0; i < W; i++) for(int j = 0; j < H; j++)){
//             Node node = nodes[i][j];
//             // Some local operation in the node (optionally with neighbours)
//             node.height = max(node.neighbor_heights[0], [1], ...)
//             node.height = node_up.
//         }



//         if(...) break;
//     }

//     // Generate output by reading from each Node, save as segmentation


// }