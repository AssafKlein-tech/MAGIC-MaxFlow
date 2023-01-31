/*
  ex2-segmentation
  ================

  This example shows a minimal image segmentation code based on GridCut.
*/

#include <cstdio>
#include <cmath>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <Image.h>
#include <GridCut/GridGraph_2D_4C.h>
#include <utility>
#include <thread>


using namespace std;

// (v^2e c) 
#define K 1000
#define SIGMA2 0.012f


#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)

float get_dist(float p1, float p2)
{
    return pow(p1-p2, 2);
}

short weight(float dist, float sigma)
{
    return (short)(10+K*std::exp((-(dist)/(2*sigma))));
}

template <typename T>
float get_sigma(const string image_path)
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



int main(int argc,char** argv)
{
  chrono::steady_clock sc;
  typedef GridGraph_2D_4C<short,short,int> Grid;

  const string dir = "../../../photos/";
  string photo = "groot";
  const string image_path = dir + "BnW/" + photo +".png";
  const string scribbles_path = dir + "scribbles/" + photo +".png";
  const string out_path = dir + photo +"-out.png";


  const Image<float> image = imread<float>(image_path);
  const Image<RGB> scribbles = imread<RGB>(scribbles_path);

  const float sigma = get_sigma<float>(image_path);

  int width  = scribbles.width();
  int height = scribbles.height();
  
  Grid* grid = new Grid(width,height);


  short cap;

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      grid->set_terminal_cap(grid->node_id(x,y),
                           scribbles(x,y)==BLUE ? K : 0,
                           scribbles(x,y)==RED  ? K : 0);


      if (x<width-1)
      {
        cap = weight(get_dist(image(x,y),image(x+1,y)),sigma);
        
        grid->set_neighbor_cap(grid->node_id(x  ,y),+1,0,cap);
        grid->set_neighbor_cap(grid->node_id(x+1,y),-1,0,cap);
      }
      if (y<height-1)
      {
        cap = weight(get_dist(image(x,y),image(x,y+1)),sigma);
        grid->set_neighbor_cap(grid->node_id(x,y  ),0,+1,cap);
        grid->set_neighbor_cap(grid->node_id(x,y+1),0,-1,cap);
      }
    }
  }

  cout << "width: " << width << " height: " << height << endl;


  auto start = sc.now();
  grid->compute_maxflow();
  auto end = sc.now();

  double time_span = static_cast<chrono::duration<double>>(end - start).count();
  
  Image<RGB> output(width,height);
  int cutsize = 0;

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      output(x,y) = image(x,y)*(grid->get_segment(grid->node_id(x,y)) ? RED : BLUE);
      if (!grid->get_segment(grid->node_id(x,y)))
         cutsize+=1;
    }
  }

  imwrite(output,out_path);  

  printf(" The time is %f\n, the flow is %d\n",time_span, grid->get_flow());
  return 0;
}
