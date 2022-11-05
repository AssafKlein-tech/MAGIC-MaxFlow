#include <cstdio>
#include <cmath>
#include "goldberg_grid.cpp"
#include "graph_gen.cpp"
#include "Image.h"

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)
#define K 1000
#define SIGMA2 0.012f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))

graph flow, capacity;



void build_undirected_graph(const char* image_path,const char* scribbles_path, int* width, int* height, graph* capacity)
{
    const Image<float> image = imread<float>(image_path);
    const Image<RGB> scribbles = imread<RGB>(scribbles_path);

    const int w  = image.width();
    const int h = image.height();
    *width = w;
    *height = h;
    int index;
    int n = w * h;
    int size = n+2;
    (*capacity).assign(size, vi(size, 0));

    grid::initialize_nodes(w,h);

    for(int y=0;y<h;y++)
    {
        for(int x=0;x<w;x++)
        {
            index = x+y*w + 1;
            grid::set_terminal_cap(x, y, scribbles(x,y)==BLUE ? K : 0, scribbles(x,y)==RED  ? K : 0);
            (*capacity)[0][index] = scribbles(x,y)==BLUE ? K : 0;
            (*capacity)[index][n+1] = scribbles(x,y)==RED  ? K : 0;


            if (x<w-1)
            {
                const short cap = WEIGHT(image(x,y)-image(x+1,y));

                grid::set_neighbor_cap(x  ,y,+1,0,cap);
                (*capacity)[index][index + 1] = cap;
                grid::set_neighbor_cap(x+1,y,-1,0,cap);
                (*capacity)[index + 1][index] = cap;
            }

            if (y< h-1)
            {
                const short cap = WEIGHT(image(x,y)-image(x,y+1));

                grid::set_neighbor_cap(x,y  ,0,+1,cap);
                (*capacity)[index][index + w] = cap;
                grid::set_neighbor_cap(x,y+1,0,-1,cap);
                (*capacity)[index + w][index] = cap;
            }
        }
    }                

}