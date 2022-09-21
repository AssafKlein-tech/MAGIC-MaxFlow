#include"../include/parallel_graph.cuh"

void prlong(long V,long *cpu_height, long *cpu_excess_flow, long *cpu_rflowmtx, long *cpu_adjmtx)
{
    printf("\nHeight :");
    for(long i = 0; i < V; i++)
    {
        printf("%ld ",cpu_height[i]);
    }

    printf("\nExcess flow :");
    for(long i = 0; i < V; i++)
    {
        printf("%ld ",cpu_excess_flow[i]);
    }

    printf("\nRflow mtx :\n");
    for(long i = 0; i < V; i++)
    {
        for(long j = 0; j < V; j++)
        {
            printf("%ld ", cpu_rflowmtx[IDX(i,j)]);
        }
        printf("\n");
    }

    printf("\nAdj mtx :\n");
    for(long i = 0; i < V; i++)
    {
        for(long j = 0; j < V; j++)
        {
            printf("%ld ", cpu_adjmtx[IDX(i,j)]);
        }
        printf("\n");
    }
}


void readgraph(long V, long E, long source, long sink, long *cpu_height, long *cpu_excess_flow, long *cpu_adjmtx, long *cpu_rflowmtx)
{
    // initialising all adjacent matrix values to 0 before input 
    for(long i = 0; i < (number_of_nodes)*(number_of_nodes); i++)
    {
        cpu_adjmtx[i] = 0;
        cpu_rflowmtx[i] = 0;
    }
    // declaring file polonger to read edgelist
    FILE *fp = fopen("edgelist.txt","r");

    // declaring variables to read and store data from file
    char buf1[10],buf2[10],buf3[10];
    long e1,e2,cp;
    std::cout << (number_of_nodes)*(number_of_nodes) << std::endl;
    // getting edgelist input from file "edgelist.txt"
    for(long i = 0; i < E; i++)
    {
        // reading from file
        fscanf(fp,"%s",buf1);
        fscanf(fp,"%s",buf2);
        fscanf(fp,"%s",buf3);

        // storing as longegers
        e1 = long(atoi(buf1));
        e2 = long(atoi(buf2));
        cp = long(atoi(buf3));
        std::cout << i << ", " << e1 << ", " << e2 << ", " << cp << std::endl;
        /* Adding edges to the graph 
         * rflow - residual flow is also updated simultaneously
         * So the graph when prepared already has updated residual flow values
         * This is why residual flow is not initialised during preflow
         */
            std::cout << IDX(e1,e2) << std::endl;
            cpu_adjmtx[IDX(e1,e2)] = cp;
            cpu_rflowmtx[IDX(e1,e2)] = cp;    
        

    }

}
