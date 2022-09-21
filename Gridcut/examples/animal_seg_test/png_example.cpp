
#include <cstdio>
#include <cmath>

#include <Image.h>
#include <GridCut/GridGraph_2D_4C.h>

#define K 1000
#define SIGMA2 0.012f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)

int main(int argc,char** argv)
{
  typedef GridGraph_2D_4C<short,short,int> Grid;

  const Image<float> image = imread<float>("giraffe.png");
    //   const Image<RGB> scribbles = imread<RGB>("scribbles.png");
  
  const int width  = image.width();
  const int height = image.height();

  Grid* grid = new Grid(width,height);
  float max = 0;
  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {   
        if(image(x,y) > max)
            max = image(x,y);
        
        // if ((x>(width/2) && (x<(width/2) + 10)) && ((y>(height/2) && (y<(height/2) + 10))))
        if (image(x,y) > 0.9 * max)
            grid->set_terminal_cap(grid->node_id(x,y),0,K);
        else if (image(x,y) < 0.5  * max)
            grid->set_terminal_cap(grid->node_id(x,y),K,0);
        else
            grid->set_terminal_cap(grid->node_id(x,y),0,0);
      
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

    printf("%f",image(10,10));
  grid->compute_maxflow();


  Image<RGB> output(width,height);

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      output(x,y) = image(x,y)*(grid->get_segment(grid->node_id(x,y)) ? RED : BLUE);
    }
  }

  delete grid;

  imwrite(output,"output.png");  

  printf("The result was written to \"output.png\".\n");
  
  return 0;
}
