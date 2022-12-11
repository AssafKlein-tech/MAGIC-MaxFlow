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

// (v^2e c) 
#define K 1000
#define SIGMA2 930.0f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))

//#define PI 3.14159265

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)

short l1(RGB p1, RGB p2)
{
  return ((p1.b - p2.b) + (p1.g - p2.g) + (p1.r - p2.r));
}

short l2(RGB p1, RGB p2)
{
  return sqrt(pow((p1.b - p2.b),2) + pow((p1.g - p2.g),2) + pow((p1.r - p2.r),2));
}

short linf(RGB p1, RGB p2)
{
  return max((p1.b - p2.b),max((p1.g - p2.g),(p1.r - p2.r)));
}

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

class Lab
{
public: 
  float L;
  float a;
  float b;

  Lab(RGB p)
  {
    float X = 0.607 * p.r + 0.174 * p.g + 0.200 * p.b;
    float Y = 0.299 * p.r + 0.587 * p.g + 0.114 * p.b;
    float Z = 0.000 * p.r + 0.066 * p.g + 1.116 * p.b;
    L = 116 * pow(Y,(1/3)) - 16;
    a = 500 * (pow(X,(1/3)) - pow(Y,(1/3)));
    b = 200 * (pow(Y,(1/3)) - pow(Z,(1/3)));
  }
};


short Lab_dist(RGB p1, RGB p2)
{
  Lab lab1 = Lab(p1);
  Lab lab2 = Lab(p2);


  return sqrt(pow((lab1.L- lab2.L),2) + pow((lab1.a- lab2.a),2) + pow((lab1.b - lab2.b),2));
  //return sqrt(pow((I1- I2),2) + pow(S1,2) + pow(S2,2) - 2*S1*S2*cos(H2-H1));
}



int main(int argc,char** argv)
{


  chrono::steady_clock sc;

  typedef GridGraph_2D_4C<short,short,int> Grid;

  // const Image<float> image = imread<float>("image.png");
  // const Image<RGB> scribbles = imread<RGB>("scribbles.png");
  
  const Image<RGB> image = imread<RGB>("colors1.png");
  const Image<RGB> scribbles = imread<RGB>("colors1-scribbles.png");

  const int width  = scribbles.width();
  const int height = scribbles.height();
  cout << "width: " << width << " height: " << height << endl;

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
        const short cap = WEIGHT(hsi(image(x,y),image(x+1,y)));

        grid->set_neighbor_cap(grid->node_id(x  ,y),+1,0,cap);
        grid->set_neighbor_cap(grid->node_id(x+1,y),-1,0,cap);
      }

      if (y<height-1)
      {
        const short cap = WEIGHT(hsi(image(x,y),image(x,y+1)));

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
  int cutsize = 0;

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      if (grid->get_segment(grid->node_id(x,y)))
        output(x,y) = RGB(min((image(x,y).r + 0.5),1.0),image(x,y).g,image(x,y).b);
      else
      {
        output(x,y) = RGB(image(x,y).r,image(x,y).g,min((image(x,y).b + 0.5),1.0));
      }
       
      if (!grid->get_segment(grid->node_id(x,y)))
         cutsize+=1;
    }
  }

  int maxflow = grid->get_flow();

  delete grid;

  imwrite(output,"output.png");  

  printf("The result was written to \"output.png\".\nmax flow is %d\n The time is %f\n cutsize is %d\n",maxflow, time_span, cutsize);
  
  return 0;
}
