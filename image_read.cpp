#include <cstdio>
#include <cmath>
#include "goldberg_grid.cpp"
#include "Image.h"

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)
#define K 1000
#define SIGMA2 0.012f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))



void build_undirected_graph(const char* image_path,const char* scribbles_path, int* width, int* height)
{
    const Image<float> image = imread<float>(image_path);
    const Image<RGB> scribbles = imread<RGB>(scribbles_path);

    const int w  = image.width();
    const int h = image.height();
    *width = w;
    *height = h;

    grid::initialize_nodes(w,h);

    for(int y=0;y<h;y++)
    {
        for(int x=0;x<w;x++)
        {
            grid::set_terminal_cap(x, y, scribbles(x,y)==BLUE ? K : 0, scribbles(x,y)==RED  ? K : 0);

            if (x<w-1)
            {
                const short cap = WEIGHT(image(x,y)-image(x+1,y));

                grid::set_neighbor_cap(x  ,y,+1,0,cap);
                grid::set_neighbor_cap(x+1,y,-1,0,cap);
            }

            if (y< h-1)
            {
                const short cap = WEIGHT(image(x,y)-image(x,y+1));

                grid::set_neighbor_cap(x,y  ,0,+1,cap);
                grid::set_neighbor_cap(x,y+1,0,-1,cap);
            }
        }
    }                

}