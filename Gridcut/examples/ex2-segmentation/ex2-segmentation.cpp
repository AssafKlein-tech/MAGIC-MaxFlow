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
#define WEIGHT(A) (short)(10+K*std::exp((-(A)*(A)/SIGMA2)))
#define P  (int)1

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



int main(int argc,char** argv)
{
  chrono::steady_clock sc;
  typedef GridGraph_2D_4C<short,short,int> Grid;

  const string dir = "../../../photos/";
  string photo = "kangaroo";
  const string image_path = dir + "BnW/" + photo +".png";
  const string scribbles_path = dir + "scribbles/" + photo +".png";
  const string out_path = dir + photo +"-out.png";


  const Image<float> image = imread<float>(image_path);
  const Image<RGB> scribbles = imread<RGB>(scribbles_path);

  const float sigma = get_sigma<float>(image_path);

  int width  = scribbles.width();
  int height = scribbles.height();


  cout << "width: " << width << " height: " << height << endl;
  
  Grid* grid;
  vector<Grid*> grids;
  for (int i = 0; i <P; i++)
  {
    grid = new Grid(width, height);
    grids.push_back(grid);
  }

  short cap;

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {

      for (auto it : grids)
      {
        it->set_terminal_cap(it->node_id(x,y),
                             scribbles(x,y)==BLUE ? K : 0,
                             scribbles(x,y)==RED  ? K : 0);
      }

      if (x<width-1)
      {
        cap = weight(get_dist(image(x,y),image(x+1,y)),sigma);
        for (auto it : grids)
        {
          it->set_neighbor_cap(it->node_id(x  ,y),+1,0,cap);
          it->set_neighbor_cap(it->node_id(x+1,y),-1,0,cap);
        }
      }
      if (y<height-1)
      {
        cap = weight(get_dist(image(x,y),image(x,y+1)),sigma);
        for (auto it : grids)
        {
          it->set_neighbor_cap(it->node_id(x,y  ),0,+1,cap);
          it->set_neighbor_cap(it->node_id(x,y+1),0,-1,cap);
        }
      }
    }
  }

  std::vector<std::thread> grid_threads;
  int count = 0;


  auto start = sc.now();
  //creat P threads
  for (auto it : grids)
  {
    grid_threads.push_back(std::thread(&Grid::compute_maxflow,it));
  }

  //join Pthreads
  while (count < P)
  {
    for (std::thread& t : grid_threads)
      if (t.joinable())
      {
        t.join();
        count++;
      }
  }

  auto end = sc.now();
  double time_span = static_cast<chrono::duration<double>>(end - start).count();
  

  printf(" The time is %f\n",time_span);
  for (auto it : grids)
  {
    delete it;
  }
  return 0;
}
