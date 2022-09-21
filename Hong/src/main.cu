#include"../include/parallel_graph.cuh"
#include"../include/serial_graph.h"
#include <cstdio>
#include <string>
#define INPUT_FILE "edgelist.txt"

long format_longput(int argc, char **argv, long *lengths)
{
    long V, E, source = 1, sink = 2;
    long a, b, cap;
    char *split_str_ptr;
    bool p_flag = false, vertex_flag = false;

    std::ofstream write_file (INPUT_FILE);
    // write_file << "test:" << argc << ", " << argv[1] << std::endl;
    // Open read file
    std::ifstream myfile (argv[1]);
    // std::cout << "file path: " << argv[1] << std::endl;
    std::string myline;
    // Read file
    if ( myfile.is_open() ) {
        while ( myfile ) {
            std::getline (myfile, myline);
            char *line = new char[myline.length() + 1];
            strcpy(line, myline.c_str());
            split_str_ptr = strtok(line," ");
            // Read split line
            for (long i = 0; split_str_ptr != NULL; i++)  
            {  
                // If its the first line comment
                if((i == 0) && (strcmp("p", split_str_ptr) == 0))
                    p_flag = true;
                else if(p_flag && i==2)
                    V = std::stoi(split_str_ptr);      
                else if(p_flag && i==3)
                {
                    E = std::stoi(split_str_ptr);
                    p_flag = false;
                }          
                else if ((i == 0) && (strcmp("a", split_str_ptr) == 0))
                    vertex_flag = true;
                else if ((i == 1) && vertex_flag)
                    a = std::stoi(split_str_ptr);
                else if ((i == 2) && vertex_flag)
                    b = std::stoi(split_str_ptr);
                else if ((i == 3) && vertex_flag)
                    cap = std::stoi(split_str_ptr);

                

                split_str_ptr = strtok(NULL, " "); 
            }
            if(vertex_flag)// && !(a == 1 || b == 2))
            {

                write_file << a << " " << b << " " << cap << '\n';
                vertex_flag = false;
            }
        }
    }
    argc = 5;
    lengths[0] = V;
    lengths[1] = E;
    lengths[2] = source;
    lengths[3] = sink;
    write_file.close();
    myfile.close();
    return argc;
}

long run_hong(int argc, long *lengths)
{
    // checking if sufficient number of arguments (4) are passed in CLI
    if(argc != 5)
    {
        printf("Invalid number of arguments passed during execution\n");
        exit(0);
    }

    // // reading the arguments passed in CLI
    // long V = atoi(argv[1]);
    // long E = atoi(argv[2]);
    // long source = atoi(argv[3]);
    // long sink = atoi(argv[4]);
    long V = lengths[0];
    long E = lengths[1];
    long source = lengths[2];
    long sink = lengths[3];

    std::cout << "V = " << V << " E = " << E << " V^2 " << V*V << std::endl;

    // declaring variables to store graph data on host as well as on CUDA device global memory 
    long *cpu_height,*gpu_height;
    long *cpu_excess_flow,*gpu_excess_flow;
    long *Excess_total;
    long *cpu_adjmtx,*gpu_adjmtx;
    long *cpu_rflowmtx,*gpu_rflowmtx;
    
    // allocating host memory
    cpu_height = (long*)malloc(V*sizeof(long));
    cpu_excess_flow = (long*)malloc(V*sizeof(long));
    cpu_adjmtx = (long*)malloc(V*V*sizeof(long));
    cpu_rflowmtx = (long*)malloc(V*V*sizeof(long));
    Excess_total = (long*)malloc(sizeof(long));

    // allocating CUDA device global memory
    cudaMalloc((void**)&gpu_height,V*sizeof(long));
    cudaMalloc((void**)&gpu_excess_flow,V*sizeof(long));
    cudaMalloc((void**)&gpu_adjmtx,V*V*sizeof(long));
    cudaMalloc((void**)&gpu_rflowmtx,V*V*sizeof(long));

    std::cout << " 1 " << std::endl;

    // readgraph
    readgraph(V,E,source,sink,cpu_height,cpu_excess_flow,cpu_adjmtx,cpu_rflowmtx);

    std::cout << " 2 " << std::endl;
    // prlong(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);

    // time start

    // invoking the preflow function to initialise values in host
    preflow(V,source,sink,cpu_height,cpu_excess_flow,cpu_adjmtx,cpu_rflowmtx,Excess_total);

    //prlong(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);

    // copying host data to CUDA device global memory
    cudaMemcpy(gpu_height,cpu_height,V*sizeof(long),cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_excess_flow,cpu_excess_flow,V*sizeof(long),cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_adjmtx,cpu_adjmtx,V*V*sizeof(long),cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_rflowmtx,cpu_rflowmtx,V*V*sizeof(long),cudaMemcpyHostToDevice);

    // push_relabel()
    push_relabel(V,source,sink,cpu_height,cpu_excess_flow,cpu_adjmtx,cpu_rflowmtx,Excess_total,gpu_height,gpu_excess_flow,gpu_adjmtx,gpu_rflowmtx);
    
    // store value from serial implementation
    long serial_check = check(V,E,source,sink);

    // prlong values from both implementations
    printf("The maximum flow value of this flow network as calculated by the parallel implementation is %ld\n",cpu_excess_flow[sink]);
    printf("The maximum flow of this flow network as calculated by the serial implementation is %ld\n",serial_check);
    
    // prlong correctness check result
    if(cpu_excess_flow[sink] == serial_check)
    {
        printf("Passed correctness check\n");
    }
    else
    {
        printf("Failed correctness check\n");
    }

    // free device memory
    cudaFree(gpu_height);
    cudaFree(gpu_excess_flow);
    cudaFree(gpu_adjmtx);
    cudaFree(gpu_rflowmtx);
    
    // free host memory
    free(cpu_height);
    free(cpu_excess_flow);
    free(cpu_adjmtx);
    free(cpu_rflowmtx);
    free(Excess_total);
    
    // return 0 and end program
    return 0;

}

int main(int argc, char **argv)
{
    long lengths[4];
    argc = format_longput(argc, argv, lengths);
    // long lengths[4] = {164922, 802681, 1, 2};
    run_hong(argc, lengths);
    // return 0 and end program
    return 0;
}