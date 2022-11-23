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
#include <cmath>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <Image.h>
#include <GridCut/GridGraph_2D_4C.h>
#include <chrono>
using namespace std;

#define K 1000
#define SIGMA2 0.012f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)



int main(int argc,char** argv)
{
  chrono::steady_clock sc;

  typedef GridGraph_2D_4C<short,short,int> Grid;

  // const Image<float> image = imread<float>("image.png");
  // const Image<RGB> scribbles = imread<RGB>("scribbles.png");
  
  const Image<float> image = imread<float>("elephant.png");
  const Image<RGB> scribbles = imread<RGB>("elephant-scribbles.png");

  const int width  = image.width();
  const int height = image.height();

  Grid* grid = new Grid(width,height);
  
  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      grid->set_terminal_cap(grid->node_id(x,y),
                             scribbles(x,y)==BLUE ? K : 0,
                             scribbles(x,y)==RED  ? K : 0);

      if (x<width-1)
      {
        const short cap = WEIGHT(image(x,y)-image(x+1,y));

        grid->set_neighbor_cap(grid->node_id(x  ,y),+1,0,cap);
        grid->set_neighbor_cap(grid->node_id(x+1,y),-1,0,cap);
      }

      if (y<height-1)
      {
        const short cap = WEIGHT(image(x,y)-image(x,y+1));

        grid->set_neighbor_cap(grid->node_id(x,y  ),0,+1,cap);
        grid->set_neighbor_cap(grid->node_id(x,y+1),0,-1,cap);
      }
    }
  }

  auto start = sc.now();
  grid->compute_maxflow();
  auto end = sc.now();
  double time_span = static_cast<chrono::duration<double>>(end - start).count();



  Image<RGB> output(width,height);

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      output(x,y) = image(x,y)*(grid->get_segment(grid->node_id(x,y)) ? RED : BLUE);
    }
  }

  int maxflow = grid->get_flow();

  delete grid;

  imwrite(output,"output.png");  

  printf("The result was written to \"output.png\".\nmax flow is %d\n The time is %f\n",maxflow, time_span);
  
  return 0;
}
