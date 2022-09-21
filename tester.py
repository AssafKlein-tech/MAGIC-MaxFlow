from random import randint
import os
import networkx as nx
import pseudoflow
from math import sqrt

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.patches as mpatches

# def loop_detection(C, N):

# Parameters
SIZE_RANGE = 100
MAX_SIZE = 50000
MAX_FLOW_CAPACITY = 200
DENSITY_PERCENT = 50
NUMBER_OF_ITERATIONS = 100
GRAPHS_PER_ITERATION = 10


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
    num_arcs = path_len + 1

    # Fill up the graph
    for i in range(N):
        for j in range(N):
            # If its a valid spot for a vertex and we fill the percent requirements
            if (binary[i][j] != 0) and (randint(1,100) < density_percent):
                binary[i][j] = 0
                C[i][j] = randint(1, max_flow_capacity)
                num_arcs += 1

    return C, N, num_arcs

def gen_grid_graph(min_size, max_size, max_flow_capacity):

    # N = randint(min_size, max_size)
    height = int(sqrt(randint(min_size, max_size)))
    width = int(sqrt(randint(min_size, max_size)))
    N = height * width
    # Adding the source and sink
    N += 2
    # print(height, width, N)
    # Initialize the C and binary graphs
    binary = [[1 for i in range(N)] for j in range(N)]
    C = [[0 for i in range(N)] for j in range(N)]

    # Set the first column and last row to zeroes becuase we dont expect any
    # vertices there
    for i in range(N):
        binary[i][0] = 0
        binary[N-1][i] = 0
        binary[i][i] = 0

    num_arcs = 0
    # Fill up the graph
    for i in range(1, N - 1):
        for j in range(1, N - 1):
            # If its a valid spot for a vertex and we fill the percent requirements
            i_width = (i - 1) % width   
            i_height = (i - 1) // width
            j_width = (j - 1) % width
            j_height = (j - 1) // width 
            # print(i,j)
            # print(i_width, i_height, j_width, j_height)       
            if (binary[i][j] != 0) and (binary[j][i] != 0) and (
            (i_width == j_width and abs(i_height - j_height) <= 1) or (
            i_height == j_height and abs(i_width - j_width) <= 1
            )
            ):
                binary[i][j] = 0
                binary[j][i] = 0
                cap = randint(1, max_flow_capacity)
                C[i][j] = cap
                C[j][i] = cap
                num_arcs += 2
    
    # # Fill up the graph
    # for i in range(height):
    #     for j in range(width):
    #         # If its a valid spot for a vertex and we fill the percent requirements

    #         if (binary[i][j] != 0) and (binary[j][i] != 0) and ((i == j - 1) or (i == j) or (i == j + 1)):
    #             binary[i][j] = 0
    #             binary[j][i] = 0
    #             cap = randint(1, max_flow_capacity)
    #             C[i][j] = cap
    #             C[j][i] = cap
    #             num_arcs += 2
    for i in range(1, N - 1):
        if binary[0][i] != 0:
            binary[0][i] = 0
            C[0][i] = randint(1, max_flow_capacity)
        if binary[i][N - 1] != 0:
            binary[i][N - 1] = 0
            C[i][N - 1] = randint(1, max_flow_capacity)

    return C, N, height, width, num_arcs

def run_HPF(C, N, num_arcs, max_flow_capacity, write_file_path, results_path, script_path):

    def convert_to_format_HPF_C(C, N, num_arcs, max_flow_capacity):
        # Converting to the HPF C code format
        #output = "c ------ Test ------\n"
        output = f"p {N} {num_arcs}\n"
        output += "n 1 s\n"
        output += f"n {N} t\n"
        # Converting the vertices data
        for i in range(N):
            for j in range(N):
                # If its a valid spot for a vertex and we fill the percent requirements
                if C[i][j] != 0:
                    output += f"a {i+1} {j+1} {C[i][j]}\n"  
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
        # print(sink_cut_list)
        # Add up the source set to sink set vertices
        max_flow = 0
        for i in range(N):
            for j in sink_cut_list:
                if i not in sink_cut_list:
                    max_flow += C[i][j]
        print("The max flow is: " + str(max_flow))


    data = convert_to_format_HPF_C(C, N, num_arcs, max_flow_capacity)
    write_input_file(data, write_file_path)
    print("Done converting and writing the input file, running script")
    os.system(f"{script_path} {write_file_path} {results_path}")
    #print("------------------ result proccessing")
    #read_result_file(C, N, results_path)



def run_grid_cut(C, N, height, width, write_file_path, make_path):

    def convert_to_format_grid_cut(C, N, height, width):
        # print(C)
        output = "#include <cstdio>\n"
        output += "#include <GridCut/GridGraph_2D_4C.h>\n"
        output += "#include <sys/time.h>\n"
        output += "#include <sys/resource.h>\n"
        output += "#include <iostream>\n"
        output += "#include <stdlib.h>\n"
        output += "#include <fstream>\n\n"

        output += "double timer (void) {\n"
        output += "struct rusage r;\n"
        output += "getrusage(0, &r);\n"
        output += "return (double) (r.ru_utime.tv_sec + r.ru_utime.tv_usec / (double)1000000);}\n\n"

        output += "void writeTime(char *filename, float time, int size){\n"
        output += "std::ofstream outfile;\n"
        output += "outfile.open(filename, std::ios_base::app);\n"
        temp = "n"
        output += f"outfile << size << \",\" << time << \",Grid cuts serial\\{temp}\";\n"        
        output += "outfile.close();}\n\n"

        output += "int main(int argc, char **argv)\n{\n"
        output += "typedef GridGraph_2D_4C<int,int,int> Grid;\n\n"
        output += f"Grid* grid = new Grid({height},{width});\n"
        for i in range(1, N - 1):
            # If its a valid spot for a vertex and we fill the percent requirements
            i_width = (i - 1) % width   
            i_height = (i - 1) // width
            output += f"grid->set_terminal_cap(grid->node_id({i_width},{i_height}),{C[0][i]},{C[i][N-1]});\n"
        
        
        for i in range(1, N - 1):
            for j in range(1, N - 1):
                i_width = (i - 1) % width   
                i_height = (i - 1) // width
                if (C[i][j] != 0):
                    temp = ""
                    delta = j - i
                    if delta == -1:
                        temp = f"-1, 0,{C[i][j]});\n"
                    elif delta == 1:
                        temp = f"+1, 0,{C[i][j]});\n"
                    elif delta == -width:
                        temp = f"0, -1,{C[i][j]});\n"                    
                    elif delta == width:
                        temp = f"0, +1,{C[i][j]});\n"
                    else:
                        print(delta)
                        continue
                    output += f"grid->set_neighbor_cap(grid->node_id({i_width},{i_height})," + temp          
        
        output += "double start = timer();\n"
        output += "grid->compute_maxflow();\n"
        output += "double end = timer();\n"
        output += f"writeTime(argv[1], (end-start)*1000, {height*width});\n"
        output += f"for(int y=0;y<{height};y++)\n"
        output += "{\n"
        output += f"    for(int x=0;x<{width};x++)\n"
        output += "     {\n"
        #output += "         printf(\"%c\",(grid->get_segment(grid->node_id(x,y)) == 0) ? 'S' : 'T');\n"
        output += "     }\n"
        temp = "n"
        # output += f"printf(\"\\{temp}\");"
        output += "}\n"
        output += "delete grid;\n"
        output += "return 0;}\n"
        return output

    def write_input_file(data, write_file_path):

        f = open(write_file_path, "w")
        f.write(data)
        f.close()

    data = convert_to_format_grid_cut(C, N, height, width)
    write_input_file(data, write_file_path + ".cpp")
    os.system(make_path + "_clean")
    os.system(make_path + "_make")
    os.system(write_file_path + " ./algs.csv")

def convertToCudaCutsFormat(C, N, H, W, dstfile):
    with open(dstfile, "w") as wfile:
        wfile.write(H + " " + W + " " + "2\n")

        #dummy output
        for i in range(H):
            for j in range(W):
                wfile.write(j+ " ")
            wfile.write("\n")

        #sink and target weights
        for i in range(H):
            for j in range(W):
                wfile.write(C[0][1 + j + i * W]+ " ")
            wfile.write("\n")

        for i in range(H):
            for j in range(W):
                wfile.write(C[1 + j + i * W][N-1]+ " ")
            wfile.write("\n")

        #horizontal weights
        for i in range(H):
            for j in range(W - 1):
                wfile.write(C[1 + j+ i*W][1 + j+1 + i*W]+ " ")
            wfile.write("\n")

        #vertical weights
        for i in range(H - 1):
            for j in range(W):
                wfile.write(C[1 + j +  i*W][1 + j + W + i*W]+ " ")
            wfile.write("\n")




def main():
    graph_size = MAX_SIZE

    C, N, height, width , num_arcs = gen_grid_graph(graph_size, graph_size + SIZE_RANGE, MAX_FLOW_CAPACITY)
    run_grid_cut(C, N, height, width, "./Gridcut/examples/assaf_ido_example/assaf_ido_example", "./Gridcut/examples/assaf_ido_example/test")
    #run_HPF(C, N, num_arcs, MAX_FLOW_CAPACITY, "./pseudo_max/ido.txt", "./algs.csv", "/home/idoassaf/MAGIC-MaxFlow/pseudo_max/bin/pseudo_lifo")
    # for i in range(NUMBER_OF_ITERATIONS):
    #     graph_size += i * MAX_SIZE
    #     for j in range(GRAPHS_PER_ITERATION):
    #         # general graphs 
    #         C, N, num_arcs = gen_graph(graph_size, graph_size + SIZE_RANGE, MAX_FLOW_CAPACITY, DENSITY_PERCENT)
    #         #print(C)
    #         run_HPF(C, N, num_arcs, MAX_FLOW_CAPACITY, "./pseudo_max/ido.txt", "./results.txt", "/home/idoassaf/MAGIC-MaxFlow/pseudo_max/bin/pseudo_lifo")

    #         #grid graphs
    #         C, N, num_arcs = gen_grid_graph(graph_size, graph_size + SIZE_RANGE, MAX_FLOW_CAPACITY)
            


if __name__ == "__main__":
    main()



