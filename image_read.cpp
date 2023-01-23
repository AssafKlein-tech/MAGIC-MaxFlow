#include <cstdio>
#include <cmath>
#include "goldberg_grid.cpp"
#include "graph_gen.cpp"
#include "Image.h"

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)
#define K 1000

graph flow, capacity;

float hsi(RGB p1, RGB p2)
{
  float I1 = (p1.r + p1.g + p1.b)/3;
  float S1 = 1 - (min(min(p1.r,p1.g), p1.b))/I1;
  float H1 =  atan2(sqrt(3)*(p1.g - p1.b),(2*p1.r - p1.g - p1.b));
  if (p1.g  < p1.b)
    H1 = 2*M_PI - H1;
  float I2 = (p2.r + p2.g + p2.b)/3;
  float S2 = 1 - (min(min(p2.r,p2.g), p2.b))/I1;
  float H2 =  atan2(sqrt(3)*(p2.g - p2.b),(2*p2.r - p2.g - p2.b));
  if (p2.g  < p2.b)
    H2 = 2*M_PI - H2;
  //cout << "I1 " << I1 << " S1 " << S1<< " H1 " << H1 << " , I2 " << I2 << " S2 " << S2 <<" H2 "<<  H2 << " " << endl;
  if(std::isnan(H2) || std::isnan(H1) || std::isnan(S1) || std::isnan(S2))
    return pow((I1 - I2),2);
  return (pow((I1- I2),2) + pow(S1,2) + pow(S2,2) - 2*S1*S2*cosf((H2-H1)));
}

float get_dist(float p1, float p2)
{
    return pow(p1-p2, 2);
}

float get_dist(RGB p1, RGB p2)
{
    return hsi(p1, p2);
}

short weight(float dist, float sigma)
{
    return (short)(10+K*std::exp((-(dist)/(2*sigma))));
}

template <typename T>
float get_sigma(const char* image_path)
{
    const Image<T> image = imread<T>(image_path);
    int w = image.width();
    int h = image.height();
    unsigned long edge_num = 0;
    float diff;
    float temp_avg = 0;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (x<w-1)
            {
                diff = get_dist(image(x,y),image(x+1,y));
                temp_avg += (diff - temp_avg)/(++edge_num);
                //cout << "diff " << diff << endl;

            }

            if (y< h-1)
            {
                diff = get_dist(image(x,y),image(x,y+1));
                temp_avg += (diff - temp_avg)/(++edge_num);
            }
        }
    }
    return temp_avg;
}

template <typename T>
void build_undirected_graph(const char* image_path,const char* scribbles_path, int* width, int* height)
{
    const float sigma = get_sigma<T>(image_path);
    cout << sigma << endl;
    const Image<T> image = imread<T>(image_path);  
    const Image<RGB> scribbles = imread<RGB>(scribbles_path);

    const int w  = image.width();
    const int h = image.height();
    *width = w;
    *height = h;

    grid::initialize_nodes(w,h);
    short cap;
    for(int y=0;y<h;y++)
    {
        for(int x=0;x<w;x++)
        {
            grid::set_terminal_cap(x, y, scribbles(x,y)==BLUE ? K : 0, scribbles(x,y)==RED  ? K : 0);

            if (x<w-1)
            {
                cap = weight(get_dist(image(x,y),image(x+1,y)),sigma);

                grid::set_neighbor_cap(x  ,y,+1,0,cap);
                grid::set_neighbor_cap(x+1,y,-1,0,cap);
            }

            if (y< h-1)
            {
                cap = weight(get_dist(image(x,y),image(x,y+1)),sigma);

                grid::set_neighbor_cap(x,y  ,0,+1,cap); //down
                grid::set_neighbor_cap(x,y+1,0,-1,cap); //up
            }
        }
    }                
}

void build_undirected_graph(const char* image_path,const char* scribbles_path, int* width, int* height, bool color = false)
{
  if (color) 
    build_undirected_graph<RGB>(image_path, scribbles_path, width, height);
  else
    build_undirected_graph<float>( image_path, scribbles_path, width, height);
}

void print_image(const char* image_path,const char* image_out_path, int width, int height, bool color = false)
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
    imwrite(output,image_out_path);  
    
}