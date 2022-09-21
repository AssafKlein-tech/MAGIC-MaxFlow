#ifndef __PARALLEL__GRAPH__HEADER__CUDA__
#define __PARALLEL__GRAPH__HEADER__CUDA__

#include <cuda.h>
#include <bits/stdc++.h>

// macros declared

#define number_of_nodes V
#define number_of_edges E
#define threads_per_block 256
#define number_of_blocks_nodes ((number_of_nodes/threads_per_block) + 1)
#define number_of_blocks_edges ((number_of_edges/threads_per_block) + 1)
#define INF 1000000000
#define IDX(x,y) ( ( (x)*(number_of_nodes) ) + (y) )
#define KERNEL_CYCLES V

// function prototypes for parallel implementation

void preflow(long V, long source, long sink, long *cpu_height, long *cpu_excess_flow, long *cpu_adjmtx, long *cpu_rflowmtx, long *Excess_total);
void push_relabel(long V, long source, long sink, long *cpu_height, long *cpu_excess_flow, long *cpu_adjmtx, long *cpu_rflowmtx, long *Excess_total, long *gpu_height, long *gpu_excess_flow, long *gpu_adjmtx, long *gpu_rflowmtx);
void global_relabel(long V, long source, long sink, long *cpu_height, long *cpu_excess_flow, long *cpu_adjmtx, long *cpu_rflowmtx, long *Excess_total, bool *mark, bool *scanned);
void readgraph(long V, long E, long source, long sink, long *cpu_height, long *cpu_excess_flow, long *cpu_adjmtx, long *cpu_rflowmtx);
void prlong(long V,long *cpu_height, long *cpu_excess_flow, long *cpu_rflowmtx, long *cpu_adjmtx);

// prototype for the push relabel kernel

__global__ void push_relabel_kernel(long V, long source, long sink, long *gpu_height, long *gpu_excess_flow, long *gpu_adjmtx,long *gpu_rflowmtx);

#endif