#include"../include/parallel_graph.cuh"

void push_relabel(long V, long source, long sink, long *cpu_height, long *cpu_excess_flow, long *cpu_adjmtx, long *cpu_rflowmtx, long *Excess_total, long *gpu_height, long *gpu_excess_flow, long *gpu_adjmtx, long *gpu_rflowmtx)
{
    /* Instead of checking for overflowing vertices(as in the sequential push relabel),
     * sum of excess flow values of sink and source are compared against Excess_total 
     * If the sum is lesser than Excess_total, 
     * it means that there is atleast one more vertex with excess flow > 0, apart from source and sink
     */

    /* declaring the mark and scan boolean arrays used in the global_relabel routine outside the while loop 
     * This is not to lose the mark values if it goes out of scope and gets redeclared in the next iteration 
     */

    bool *mark,*scanned;
    mark = (bool*)malloc(V*sizeof(bool));
    scanned = (bool*)malloc(V*sizeof(bool));

    // initialising mark values to false for all nodes
    for(long i = 0; i < V; i++)
    {
        mark[i] = false;
    }

    while((cpu_excess_flow[source] + cpu_excess_flow[sink]) < *Excess_total)
    {
        // copying height values to CUDA device global memory
        cudaMemcpy(gpu_height,cpu_height,V*sizeof(long),cudaMemcpyHostToDevice);

        prlongf("Invoking kernel\n");

        // invoking the push_relabel_kernel
        push_relabel_kernel<<<number_of_blocks_nodes,threads_per_block>>>(V,source,sink,gpu_height,gpu_excess_flow,gpu_adjmtx,gpu_rflowmtx);

        cudaDeviceSynchronize();


        // copying height, excess flow and residual flow values from device to host memory
        cudaMemcpy(cpu_height,gpu_height,V*sizeof(long),cudaMemcpyDeviceToHost);
        cudaMemcpy(cpu_excess_flow,gpu_excess_flow,V*sizeof(long),cudaMemcpyDeviceToHost);
        cudaMemcpy(cpu_rflowmtx,gpu_rflowmtx,V*V*sizeof(long),cudaMemcpyDeviceToHost);

        prlongf("After invoking\n");
        //prlong(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);
        prlongf("Excess total : %d\n",*Excess_total);
        // perform the global_relabel routine on host
        global_relabel(V,source,sink,cpu_height,cpu_excess_flow,cpu_adjmtx,cpu_rflowmtx,Excess_total,mark,scanned);

        prlongf("\nAfter global relabel\n");
        //prlong(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);
        prlongf("Excess total : %d\n",*Excess_total);
    }

}
