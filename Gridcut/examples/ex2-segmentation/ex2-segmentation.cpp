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
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))
#define P  (int)2

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)

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

  int width  = scribbles.width();
  int height = scribbles.height();


  cout << "width: " << width << " height: " << height << endl;
  
  vector<Grid> grids = vector<Grid>(P,Grid(width,height));

  short cap;

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      cout << "fgsg" <<endl;
      for (int i=0;i<P; ++i)
      {
        cout << x <<"," << y << "   " << grids[i].node_id(x  ,y) <<endl;
        grids[i].set_terminal_cap(grids[i].node_id(x,y),
                             scribbles(x,y)==BLUE ? K : 0,
                             scribbles(x,y)==RED  ? K : 0);
      }
      cout << "2" <<endl;

      if (x<width-1)
      {
        

        for (int i=0;i<P; ++i)
        {
          cap = WEIGHT(image(x,y)-image(x+1,y));
          cout << x <<"," << y << "   " << grids[i].node_id(x  ,y) <<endl;
          grids[i].set_neighbor_cap(grids[i].node_id(x  ,y),+1,0,cap);
          cout << i <<endl;
          grids[i].set_neighbor_cap(grids[i].node_id(x+1,y),-1,0,cap);
        }
      }
      cout << "3" <<endl;
      if (y<height-1)
      {
        cap = WEIGHT(image(x,y)-image(x,y+1));
        for (int i=0;i<P; ++i)
        {
          grids[i].set_neighbor_cap(grids[i].node_id(x,y  ),0,+1,cap);
          grids[i].set_neighbor_cap(grids[i].node_id(x,y+1),0,-1,cap);
        }
      }
      cout << "4" <<endl;
    }
  }

  

  grids[0].compute_maxflow();
  //cout << grids[0].get_flow() << endl;
  grids[1].compute_maxflow();
  cout << grids[1].get_flow() << endl;
  std::vector<std::thread> grid_threads;
  auto start = sc.now();
  for (int i=0;i<P; ++i)
  {
    grids[i].compute_maxflow();
    cout << grids[i].get_flow() << endl;
  }
  /**
  //creat P threads
  for (int i=0;i<P; ++i)
  {
     grid_threads.push_back(std::thread(&Grid::compute_maxflow,&grids[i]));
  }
  cout<< grid_threads.size() << endl;
  int count = 0;
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
  **/
  auto end = sc.now();
  double time_span = static_cast<chrono::duration<double>>(end - start).count();


/**
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

  int maxflow = grid->get_flow();
  **/

  //imwrite(output,out_path);  

  printf(" The time is %f\n",time_span);
  
  return 0;
}
