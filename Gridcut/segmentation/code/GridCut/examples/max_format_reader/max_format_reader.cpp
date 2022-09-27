#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <utility>
#include <GridCut/GridGraph_2D_4C.h>
#include <sys/time.h>
#include <sys/resource.h>

double 
timer (void)
{
  struct rusage r;

  getrusage(0, &r);
  return (double) (r.ru_utime.tv_sec + r.ru_utime.tv_usec / (double)1000000);
}

void writeTime(char *filename, float time, int size)
{
    std::ofstream outfile;
    outfile.open(filename, std::ios_base::app);
    outfile << size << "," << time << ",Grid cuts serial\n"; 
    outfile.close();
}

int main(int argc, char **argv)
{
  // Declarations
  typedef GridGraph_2D_4C<int,int,int> Grid;
  typedef std::pair<int,int> p; 
  Grid* grid;
  bool size_flag = false, got_size = false, vertex_flag = false, source_sink = true;
  int width = 0, height = 0, source = 1, sink = 2, a, b, cap;
  char *split_str_ptr;
  p * cap_array;

  // Open file
  std::ifstream myfile (argv[1]);
  std::cout << "file path: " << argv[1] << std::endl;
  std::string myline;

  int j = 0;
  // Read file
  if ( myfile.is_open() ) {
    while ( myfile ) {
      std::getline (myfile, myline);
      char *line = new char[myline.length() + 1];
      strcpy(line, myline.c_str());
      split_str_ptr = strtok(line," ");

      // Read split line
      for (int i = 0; split_str_ptr != NULL; i++)  
      {  
        // If its the size comment
        if((i == 1) && (strcmp("regulargrid", split_str_ptr) == 0))
          size_flag = true;
        else if(size_flag && i==2)
          width = std::stoi(split_str_ptr);
        else if(size_flag && i==3)
        {
          height = std::stoi(split_str_ptr);
          size_flag = false;
        }
        else if ((i == 0) && (strcmp("a", split_str_ptr) == 0))
          vertex_flag = true;
        else if ((i == 1) && vertex_flag)
          a = std::stoi(split_str_ptr);
        else if ((i == 2) && vertex_flag)
          b = std::stoi(split_str_ptr);
        else if ((i == 3) && vertex_flag)
          cap = std::stoi(split_str_ptr);

        split_str_ptr = strtok(NULL, " "); 
      }  

      // Got size for the first time
      if(!got_size && width && height)
      {
        grid = new Grid(width,height);
        cap_array = new p[width*height];
        got_size = true;
      }

      // Deal with the vertex
      if(vertex_flag)
      {
        if(a == 1)
          {
            cap_array[b - 3].first = cap;
            // std::cout << (b - 3) << ",cap=" << cap << std::endl;
          }
        else if(b == 2)
          cap_array[a - 3].second = cap;
        else if (source_sink)
        {
          for(int i = 0; i < width*height; i++)
          {
            grid->set_terminal_cap(
              grid->node_id(i % width, i / width),
              cap_array[i].first,
              cap_array[i].second);

              // std::cout << i << ",x=" << i % width << ",y=" << i / width << ",s" << cap_array[i].first << "," << cap_array[i].second << std::endl;
          }
          source_sink = false;
          j = -1;
        }
        else
        {
          int xa = (a - 3) % width, ya = (a - 3) / width;
          int xb = (b - 3) % width, yb = (b - 3) / width;
          if(xa == xb - 1 && ya == yb)
            grid->set_neighbor_cap(grid->node_id(xa, ya),+1, 0,cap);
          if(xa == xb + 1 && ya == yb)
            grid->set_neighbor_cap(grid->node_id(xa, ya),-1, 0,cap); 
          // if(j == 2)
          if(xa == xb && ya == yb - 1)
            grid->set_neighbor_cap(grid->node_id(xa, ya),0,+1,cap);
          // if(j == 3)
          if(xa == xb && ya == yb + 1)
            grid->set_neighbor_cap(grid->node_id(xa, ya),0,-1,cap);
        }
      }

      vertex_flag = false;
      delete [] line;
      j = (j + 1) % 4;
    }
  }
  myfile.close();
  
  double start = timer();
  grid->compute_maxflow();
  double end = timer();
  writeTime(argv[2], (end-start)*1000, width*height);
  printf("Min-cut partition:\n");

  for(int y=0;y<height;y++)
  {  
    for(int x=0;x<width;x++)  
    {
      printf("%c",(grid->get_segment(grid->node_id(x,y)) == 0) ? 'S' : 'T');
    }
    printf("\n");
  }
  printf("%d",grid->get_flow());
  delete grid;
  
  return 0;
}