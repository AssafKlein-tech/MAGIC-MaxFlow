#include <cstdio>
#include <cmath>
#include "goldberg_grid.cpp"
#include "graph_gen.cpp"
#include "Image.h"

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)
#define K 1000
#define SIGMA2 900.0f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))
#define SIGMA2_C 930.0f
#define WEIGHT_C(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2_C)))

graph flow, capacity;


short hsi(RGB p1, RGB p2)
{
  float I1 = (p1.r + p1.g + p1.b)*255/3;
  float S1 = 1 - (min(min(p1.r,p1.g), p1.b))*255/I1;
  float H1 =  atan2(sqrt(3)*(p1.g - p1.b),(2*p1.r - p1.g - p1.b));
  float I2 = (p2.r + p2.g + p2.b)*255/3;
  float S2 = 1 - (min(min(p2.r,p2.g), p2.b))*255/I1;
  float H2 =  atan2(sqrt(3)*(p2.g - p2.b),(2*p2.r - p2.g - p2.b));

  //return sqrt(pow((I1- I2),2) + pow((S1- S2),2) + pow((H1 - H2),2));
  return sqrt(pow((I1- I2),2) + pow(S1,2) + pow(S2,2) - 2*S1*S2*cos(H2-H1));
}

void build_undirected_graph_color(const char* image_path,const char* scribbles_path, int* width, int* height)
{
    const Image<RGB> image = imread<RGB>(image_path);  
    const Image<RGB> scribbles = imread<RGB>(scribbles_path);

    const int w  = image.width();
    const int h = image.height();
    *width = w;
    *height = h;
    int index;

    grid::initialize_nodes(w,h);

    for(int y=0;y<h;y++)
    {
        for(int x=0;x<w;x++)
        {
            index = x+y*w + 1;
            grid::set_terminal_cap(x, y, scribbles(x,y)==BLUE ? K : 0, scribbles(x,y)==RED  ? K : 0);

            if (x<w-1)
            {
                const short cap = WEIGHT_C(hsi(image(x,y),image(x+1,y)));

                grid::set_neighbor_cap(x  ,y,+1,0,cap);
                grid::set_neighbor_cap(x+1,y,-1,0,cap);
            }

            if (y< h-1)
            {
                const short cap = WEIGHT_C(hsi(image(x,y),image(x,y+1)));

                grid::set_neighbor_cap(x,y  ,0,+1,cap); //down
                grid::set_neighbor_cap(x,y+1,0,-1,cap); //up
            }
        }
    }                
}

void build_undirected_graph_grey(const char* image_path,const char* scribbles_path, int* width, int* height)
{
    const Image<float> image = imread<float>(image_path);  
    const Image<RGB> scribbles = imread<RGB>(scribbles_path);

    const int w  = image.width();
    const int h = image.height();
    *width = w;
    *height = h;
    int index;

    grid::initialize_nodes(w,h);

    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            index = x + y * w + 1;
            grid::set_terminal_cap(x, y, scribbles(x,y) == BLUE ? K : 0, scribbles(x,y) == RED  ? K : 0);

            if (x < w - 1)
            {
                const short cap = WEIGHT(image(x,y) - image(x+1,y));

                grid::set_neighbor_cap(x  ,y,+1,0,cap);
                grid::set_neighbor_cap(x+1,y,-1,0,cap);
            }

            if (y< h-1)
            {
                const short cap = WEIGHT(image(x,y) - image(x,y+1));

                grid::set_neighbor_cap(x,y  ,0,+1,cap); //down
                grid::set_neighbor_cap(x,y+1,0,-1,cap); //up
            }
        }
    }           
}




void build_undirected_graph(const char* image_path,const char* scribbles_path, int* width, int* height, bool color = false)
{
  if (color) 
    build_undirected_graph_color(image_path, scribbles_path, width, height);
  else
    build_undirected_graph_grey( image_path, scribbles_path, width, height);
}

void print_image(const char* image_path, int width, int height, bool color = false)
{
    Image<RGB> output(width,height);
    int cutsize = 0;
    int index = 0;
    source_cut.sort();
    if(color)
    {
        list<int>::iterator it=source_cut.begin();
        ++it;
        const Image<RGB> image = imread<RGB>(image_path);  
        for (int y=0;y<height;y++)
        {
            for (int x=0;x<width;x++)
            {
                index = 1 + x + y * width;
                if (it != source_cut.end() && *it == index)
                {
                    output(x,y) = RGB(image(x,y).r,image(x,y).g,min((image(x,y).b + 0.5),1.0));
                    ++it;
                }          
                else
                    output(x,y) = RGB(min((image(x,y).r + 0.5),1.0),image(x,y).g,image(x,y).b);
            }  
        }
    }
    else
    {
        list<int>::iterator it=source_cut.begin();
        ++it;
        const Image<float> image = imread<float>(image_path);  
        for (int y=0;y<height;y++)
        {
            for (int x=0;x<width;x++)
            {
                index = 1 + x + y * width;
                if (it != source_cut.end() && *it == index)
                {
                    output(x,y) = image(x,y)* BLUE;
                    ++it;
                }          
                else
                    output(x,y) = image(x,y)* RED;
            }  
        }
    }
    imwrite(output,"output.png");  
    
}