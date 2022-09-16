from random import randint
import os

# def loop_detection(C, N):


def gen_graph(min_size, max_size, max_flow_capacity, density_percent):

    N = randint(min_size, max_size)

    # Initialize the C and binary graphs
    binary = [[1 for i in range(N)] for j in range(N)]
    C = [[0 for i in range(N)] for j in range(N)]

    # select default path length (we make sure there is at least 1 legit path)
    path_len = randint(1, N - 2)

    # Set the first column and last row to zeroes becuase we dont expect any
    # vertices there
    for i in range(N):
        binary[i][0] = 0
        binary[N-1][i] = 0
        binary[i][i] = 0

    # Make the default path
    for i in range(path_len):
        C[i][i+1] = randint(1, max_flow_capacity)
        binary[i][i+1] = 0
        binary[i+1][i] = 0
    # Adding the last vertex
    C[path_len][N-1] = randint(1, max_flow_capacity)
    binary[path_len][N-1] = 0
    binary[N-1][path_len] = 0

    # Counts the number of total vertices
    num_vertices = path_len + 1

    # Fill up the graph
    for i in range(N):
        for j in range(N):
            # If its a valid spot for a vertex and we fill the percent requirements
            if (binary[i][j] != 0) and (randint(1,100) < density_percent):
                binary[i][j] = 0
                C[i][j] = randint(1, max_flow_capacity)
                num_vertices += 1

    return C, N, num_vertices

def run_HPF(C, N, num_vertices, max_flow_capacity, write_file_path, results_path, script_path):

    def convert_to_format_HPF_C(C, N, num_vertices, max_flow_capacity):
        # Converting to the HPF C code format
        output = "c ------ Test ------\n"
        output += f"p {N} {num_vertices} 0 {max_flow_capacity+0.001} 1\n"
        output += "n 0 s\n"
        output += f"n {N-1} t\n"
        # Converting the vertices data
        for i in range(N):
            for j in range(N):
                # If its a valid spot for a vertex and we fill the percent requirements
                if C[i][j] != 0:
                    lambda_mul = 0
                    if i == 0:
                        lambda_mul = 20
                    elif j == (N-1):
                        lambda_mul = -20
                    output += f"a {i} {j} {C[i][j]} {lambda_mul}\n"  
        return output

    def write_input_file(data, write_file_path):

        f = open(write_file_path, "w")
        f.write(data)
        f.close()

    def read_result_file(C, N, results_path):

        f = open(results_path, "r")
        sink_cut_list = []
        for line in f:
            if line[0] == 'n':
                if line[-2] == '0':
                    line = (line.split(" "))[1]
                    sink_cut_list.append(int(line))
        f.close()
        print(sink_cut_list)
        # Add up the source set to sink set vertices
        max_flow = 0
        for i in range(N):
            for j in sink_cut_list:
                if i not in sink_cut_list:
                    max_flow += C[i][j]
        print("The max flow is: " + str(max_flow))


    data = convert_to_format_HPF_C(C, N, num_vertices, max_flow_capacity)
    write_input_file(data, write_file_path)
    print("Done converting and writing the input file, running script")
    os.system(f"{script_path} {write_file_path} {results_path}")
    #print("------------------ result proccessing")
    #read_result_file(C, N, results_path)

def run_grid_Cut():
    return

def main():
    
    graph_size = MAX_SIZE

    for i in range(NUMBER_OF_ITERATIONS):
        graph_size += i * MAX_SIZE
        for j in range(GRAPHS_PER_ITERATION):
            # general graphs 
            C, N, num_arcs = gen_graph(graph_size, graph_size + SIZE_RANGE, MAX_FLOW_CAPACITY, DENSITY_PERCENT)
            #print(C)
            run_HPF(C, N, num_arcs, MAX_FLOW_CAPACITY, "./pseudo_max/ido.txt", "./results.txt", "/home/idoassaf/MAGIC-MaxFlow/pseudo_max/bin/pseudo_lifo")

            #grid graphs
            C, N, num_arcs = gen_grid_graph(graph_size, graph_size + SIZE_RANGE, MAX_FLOW_CAPACITY)
            


if __name__ == "__main__":
    main()



min_size = 6
max_size = 7
max_flow_capacity = 100
density_percent = 50

C, N, num_vertices = gen_graph(min_size, max_size, max_flow_capacity, density_percent)
print(C)
run_HPF(C, N, num_vertices, max_flow_capacity, "./hpf/pseudoflow-parametric-cut/src/pseudoflow/c/ido.txt", "./results.txt", "./hpf/pseudoflow-parametric-cut/src/pseudoflow/c/hpf")