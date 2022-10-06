#include <cstdio>
#include <cstdlib>

#include "GridGraph_2D_4C.h"
#include "GridGraph_3D_6C.h"  
#include "GridGraph_3D_26C.h"

#include "dataset.h"
#include "mfi.h"
#include "timer.h"
#include "goldberg_grid.cpp"

#include <iostream>
#include <fstream>
//#include <stdio.h>
#include <string.h>

using namespace std;

const string DSTPATH = "../../algs.csv";
const string SRCFILES = "./data/";

double t1;
double t2;

#define CLOCK_START()    { timerReset();    t1 = timerGet(); }
#define CLOCK_STOP(TIME) { t2 = timerGet(); *TIME = t2-t1;   }

void writeTime(string path, float time, int size,const  char * type, const char* alg)
{
    std::ofstream outfile;
    outfile.open(path, std::ios_base::app);
    outfile << size << "," << time << "," <<type << "," << alg << "\n"; 
    outfile.close();
}

template<typename type_terminal_cap,typename type_neighbor_cap>
void run_GridCut_2D_4C(MFI* mfi,unsigned char* out_label,int* out_maxflow,double* time_init,double* time_maxflow_grid, double* time_maxflow_goldberg,double* time_output)
{
  //cout << "Grid cut enterd" << endl;
  const int w = mfi->width;
  const int h = mfi->height;
  
  const type_terminal_cap* cap_source = (type_terminal_cap*)mfi->cap_source;
  const type_terminal_cap* cap_sink   = (type_terminal_cap*)mfi->cap_sink;
  
  const type_neighbor_cap* cap_neighbor[4] = { (type_neighbor_cap*)(mfi->cap_neighbor[0]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[1]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[2]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[3]) };

  typedef GridGraph_2D_4C<type_terminal_cap,type_neighbor_cap,int> GraphType;

  CLOCK_START();
  GraphType* graph = new GraphType(w,h); 

  grid::initialize_nodes(w,h);

  for(int y=0;y<h;y++)
  for(int x=0;x<w;x++)
  {
    const int node = graph->node_id(x,y);
   
    graph->set_terminal_cap(node,cap_source[x+y*w],cap_sink[x+y*w]);

    grid::set_terminal_cap(x, y, cap_source[x+y*w], cap_sink[x+y*w]);

    
    if (x>0  )
    {
      graph->set_neighbor_cap(node,-1, 0,cap_neighbor[MFI::ARC_LE][x+y*w]);
      grid::set_neighbor_cap(x,y,-1, 0,cap_neighbor[MFI::ARC_LE][x+y*w]);
    } 
    if (x<w-1)
    {
      graph->set_neighbor_cap(node,+1, 0,cap_neighbor[MFI::ARC_GE][x+y*w]);
      grid::set_neighbor_cap(x,y,+1, 0,cap_neighbor[MFI::ARC_LE][x+y*w]);
    } 
    if (y>0  )
    {
      graph->set_neighbor_cap(node, 0,-1,cap_neighbor[MFI::ARC_EL][x+y*w]);
      grid::set_neighbor_cap(x,y, 0,-1,cap_neighbor[MFI::ARC_EL][x+y*w]);
    } 
    if (y<h-1)
    {
      graph->set_neighbor_cap(node, 0,+1,cap_neighbor[MFI::ARC_EG][x+y*w]);
      grid::set_neighbor_cap(x,y, 0,+1,cap_neighbor[MFI::ARC_EG][x+y*w]);
    } 
  }                
  CLOCK_STOP(time_init);
 
  CLOCK_START();
  graph->compute_maxflow();
  CLOCK_STOP(time_maxflow_grid);

  cout << "Gridcut time " << time_maxflow_grid << endl;

  int goldberg_maxflow;
  CLOCK_START();
  goldberg_maxflow = grid::goldberg_grid(w,h);
  CLOCK_STOP(time_maxflow_goldberg);

  CLOCK_START();
  *out_maxflow = graph->get_flow();

  if (goldberg_maxflow != *out_maxflow)
     std::cout << "Error on computing max flow: Grids - " << *out_maxflow << "Ours: " << goldberg_maxflow << std::endl;

  for(int y=0;y<h;y++)
  for(int x=0;x<w;x++)
  {
    out_label[x+y*w] = graph->get_segment(graph->node_id(x,y));
  }
  delete graph;  
  CLOCK_STOP(time_output);
}

template<typename type_terminal_cap,typename type_neighbor_cap>
void run_GridCut_3D_6C(MFI* mfi,unsigned char* out_label,int* out_maxflow,double* time_init,double* time_maxflow,double* temp,double* time_output)
{
  const int w = mfi->width;
  const int h = mfi->height;
  const int d = mfi->depth;
  
  const type_terminal_cap* cap_source = (type_terminal_cap*)mfi->cap_source;
  const type_terminal_cap* cap_sink   = (type_terminal_cap*)mfi->cap_sink;
  
  const type_neighbor_cap* cap_neighbor[6] = { (type_neighbor_cap*)(mfi->cap_neighbor[0]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[1]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[2]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[3]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[4]),
                                               (type_neighbor_cap*)(mfi->cap_neighbor[5]) };
    
  typedef GridGraph_3D_6C<type_terminal_cap,type_neighbor_cap,int> GraphType;

  CLOCK_START();
  GraphType* graph = new GraphType(w,h,d); 

  for(int z=0;z<d;z++)

  
  for(int y=0;y<h;y++)
  for(int x=0;x<w;x++)
  {
    const int node = graph->node_id(x,y,z);

    graph->set_terminal_cap(node,cap_source[x+y*w+z*(w*h)],cap_sink[x+y*w+z*(w*h)]);
    
    if (x>0  ) graph->set_neighbor_cap(node,-1, 0, 0,cap_neighbor[MFI::ARC_LEE][x+y*w+z*(w*h)]);
    if (x<w-1) graph->set_neighbor_cap(node,+1, 0, 0,cap_neighbor[MFI::ARC_GEE][x+y*w+z*(w*h)]);
    if (y>0  ) graph->set_neighbor_cap(node, 0,-1, 0,cap_neighbor[MFI::ARC_ELE][x+y*w+z*(w*h)]);
    if (y<h-1) graph->set_neighbor_cap(node, 0,+1, 0,cap_neighbor[MFI::ARC_EGE][x+y*w+z*(w*h)]);
    if (z>0  ) graph->set_neighbor_cap(node, 0, 0,-1,cap_neighbor[MFI::ARC_EEL][x+y*w+z*(w*h)]);
    if (z<d-1) graph->set_neighbor_cap(node, 0, 0,+1,cap_neighbor[MFI::ARC_EEG][x+y*w+z*(w*h)]);
  }
  CLOCK_STOP(time_init);

  CLOCK_START();
  graph->compute_maxflow();
  CLOCK_STOP(time_maxflow)
  
  CLOCK_START();
  *out_maxflow = graph->get_flow();

  for(int z=0;z<d;z++)
  for(int y=0;y<h;y++)
  for(int x=0;x<w;x++)
  {
    out_label[x+y*w+z*(w*h)] = graph->get_segment(graph->node_id(x,y,z));
  }
  
  delete graph;
  CLOCK_STOP(time_output);
}

template<typename type_terminal_cap,typename type_neighbor_cap>
void run_GridCut_3D_26C(MFI* mfi,unsigned char* out_label,int* out_maxflow,double* time_init,double* time_maxflow,double* temp,double* time_output)
{
  const int w = mfi->width;
  const int h = mfi->height;
  const int d = mfi->depth;
  
  const type_terminal_cap* cap_source = (type_terminal_cap*)mfi->cap_source;
  const type_terminal_cap* cap_sink   = (type_terminal_cap*)mfi->cap_sink;
  
  const type_neighbor_cap* cap_neighbor[26] = { (type_neighbor_cap*)(mfi->cap_neighbor[ 0]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 1]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 2]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 3]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 4]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 5]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 6]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 7]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 8]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[ 9]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[10]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[11]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[12]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[13]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[14]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[15]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[16]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[17]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[18]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[19]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[20]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[21]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[22]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[23]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[24]),
                                                (type_neighbor_cap*)(mfi->cap_neighbor[25]) };                                                      
       
  typedef GridGraph_3D_26C<type_terminal_cap,type_neighbor_cap,int> GraphType;

  CLOCK_START();
  GraphType* graph = new GraphType(w,h,d); 

  for(int z=0;z<d;z++)
  for(int y=0;y<h;y++)
  for(int x=0;x<w;x++)
  {
    const int node = graph->node_id(x,y,z);

    graph->set_terminal_cap(node,cap_source[x+y*w+z*(w*h)],cap_sink[x+y*w+z*(w*h)]);
    
    if (x<w-1 && y<h-1 && z<d-1) graph->set_neighbor_cap(node,+1,+1,+1,cap_neighbor[MFI::ARC_GGG][x+y*w+z*(w*h)]);
    if (x<w-1          && z<d-1) graph->set_neighbor_cap(node,+1, 0,+1,cap_neighbor[MFI::ARC_GEG][x+y*w+z*(w*h)]);
    if (x<w-1 && y>0   && z<d-1) graph->set_neighbor_cap(node,+1,-1,+1,cap_neighbor[MFI::ARC_GLG][x+y*w+z*(w*h)]);
    if (x<w-1 && y<h-1         ) graph->set_neighbor_cap(node,+1,+1, 0,cap_neighbor[MFI::ARC_GGE][x+y*w+z*(w*h)]);
    if (x<w-1                  ) graph->set_neighbor_cap(node,+1, 0, 0,cap_neighbor[MFI::ARC_GEE][x+y*w+z*(w*h)]);
    if (x<w-1 && y>0           ) graph->set_neighbor_cap(node,+1,-1, 0,cap_neighbor[MFI::ARC_GLE][x+y*w+z*(w*h)]);
    if (x<w-1 && y<h-1 && z>0  ) graph->set_neighbor_cap(node,+1,+1,-1,cap_neighbor[MFI::ARC_GGL][x+y*w+z*(w*h)]);
    if (x<w-1          && z>0  ) graph->set_neighbor_cap(node,+1, 0,-1,cap_neighbor[MFI::ARC_GEL][x+y*w+z*(w*h)]);
    if (x<w-1 && y>0   && z>0  ) graph->set_neighbor_cap(node,+1,-1,-1,cap_neighbor[MFI::ARC_GLL][x+y*w+z*(w*h)]);
    if (         y<h-1 && z<d-1) graph->set_neighbor_cap(node, 0,+1,+1,cap_neighbor[MFI::ARC_EGG][x+y*w+z*(w*h)]);
    if (                  z<d-1) graph->set_neighbor_cap(node, 0, 0,+1,cap_neighbor[MFI::ARC_EEG][x+y*w+z*(w*h)]);
    if (         y>0   && z<d-1) graph->set_neighbor_cap(node, 0,-1,+1,cap_neighbor[MFI::ARC_ELG][x+y*w+z*(w*h)]);
    if (         y<h-1         ) graph->set_neighbor_cap(node, 0,+1, 0,cap_neighbor[MFI::ARC_EGE][x+y*w+z*(w*h)]);

    if (x>0   && y>0   && z>0  ) graph->set_neighbor_cap(node,-1,-1,-1,cap_neighbor[MFI::ARC_LLL][x+y*w+z*(w*h)]);
    if (x>0            && z>0  ) graph->set_neighbor_cap(node,-1, 0,-1,cap_neighbor[MFI::ARC_LEL][x+y*w+z*(w*h)]);
    if (x>0   && y<h-1 && z>0  ) graph->set_neighbor_cap(node,-1,+1,-1,cap_neighbor[MFI::ARC_LGL][x+y*w+z*(w*h)]);
    if (x>0   && y>0           ) graph->set_neighbor_cap(node,-1,-1, 0,cap_neighbor[MFI::ARC_LLE][x+y*w+z*(w*h)]);
    if (x>0                    ) graph->set_neighbor_cap(node,-1, 0, 0,cap_neighbor[MFI::ARC_LEE][x+y*w+z*(w*h)]);
    if (x>0   && y<h-1         ) graph->set_neighbor_cap(node,-1,+1, 0,cap_neighbor[MFI::ARC_LGE][x+y*w+z*(w*h)]);
    if (x>0   && y>0   && z<d-1) graph->set_neighbor_cap(node,-1,-1,+1,cap_neighbor[MFI::ARC_LLG][x+y*w+z*(w*h)]);
    if (x>0            && z<d-1) graph->set_neighbor_cap(node,-1, 0,+1,cap_neighbor[MFI::ARC_LEG][x+y*w+z*(w*h)]);
    if (x>0   && y<h-1 && z<d-1) graph->set_neighbor_cap(node,-1,+1,+1,cap_neighbor[MFI::ARC_LGG][x+y*w+z*(w*h)]);
    if (         y>0   && z>0  ) graph->set_neighbor_cap(node, 0,-1,-1,cap_neighbor[MFI::ARC_ELL][x+y*w+z*(w*h)]);
    if (                  z>0  ) graph->set_neighbor_cap(node, 0, 0,-1,cap_neighbor[MFI::ARC_EEL][x+y*w+z*(w*h)]);
    if (         y<h-1 && z>0  ) graph->set_neighbor_cap(node, 0,+1,-1,cap_neighbor[MFI::ARC_EGL][x+y*w+z*(w*h)]);
    if (         y>0           ) graph->set_neighbor_cap(node, 0,-1, 0,cap_neighbor[MFI::ARC_ELE][x+y*w+z*(w*h)]);
  }
  CLOCK_STOP(time_init);

  CLOCK_START();
  graph->compute_maxflow();
  CLOCK_STOP(time_maxflow);

  CLOCK_START();  
  *out_maxflow = graph->get_flow();

  for(int z=0;z<d;z++)
  for(int y=0;y<h;y++)
  for(int x=0;x<w;x++)
  {
    out_label[x+y*w+z*(w*h)] = graph->get_segment(graph->node_id(x,y,z));
  }  
  
  delete graph;
  CLOCK_STOP(time_output);
}

int main(int argc,char** argv) 
{
  void (*run_GridCut[4][27][3][3])(MFI*,unsigned char*,int*,double*,double*,double*, double*);                                               \

  run_GridCut[2][ 4][MFI::TYPE_UINT8 ][MFI::TYPE_UINT8 ] = run_GridCut_2D_4C<unsigned char ,unsigned char >;
  run_GridCut[2][ 4][MFI::TYPE_UINT16][MFI::TYPE_UINT8 ] = run_GridCut_2D_4C<unsigned short,unsigned char >;
  run_GridCut[2][ 4][MFI::TYPE_UINT32][MFI::TYPE_UINT8 ] = run_GridCut_2D_4C<unsigned int,  unsigned char >;
  run_GridCut[2][ 4][MFI::TYPE_UINT8 ][MFI::TYPE_UINT16] = run_GridCut_2D_4C<unsigned char ,unsigned short>;
  run_GridCut[2][ 4][MFI::TYPE_UINT16][MFI::TYPE_UINT16] = run_GridCut_2D_4C<unsigned short,unsigned short>;
  run_GridCut[2][ 4][MFI::TYPE_UINT32][MFI::TYPE_UINT16] = run_GridCut_2D_4C<unsigned int,  unsigned short>;
  run_GridCut[2][ 4][MFI::TYPE_UINT8 ][MFI::TYPE_UINT32] = run_GridCut_2D_4C<unsigned char ,unsigned int  >;
  run_GridCut[2][ 4][MFI::TYPE_UINT16][MFI::TYPE_UINT32] = run_GridCut_2D_4C<unsigned short,unsigned int  >;
  run_GridCut[2][ 4][MFI::TYPE_UINT32][MFI::TYPE_UINT32] = run_GridCut_2D_4C<unsigned int,  unsigned int  >;
  run_GridCut[3][ 6][MFI::TYPE_UINT8 ][MFI::TYPE_UINT8 ] = run_GridCut_3D_6C<unsigned char ,unsigned char >;
  run_GridCut[3][ 6][MFI::TYPE_UINT16][MFI::TYPE_UINT8 ] = run_GridCut_3D_6C<unsigned short,unsigned char >;
  run_GridCut[3][ 6][MFI::TYPE_UINT32][MFI::TYPE_UINT8 ] = run_GridCut_3D_6C<unsigned int,  unsigned char >;
  run_GridCut[3][ 6][MFI::TYPE_UINT8 ][MFI::TYPE_UINT16] = run_GridCut_3D_6C<unsigned char ,unsigned short>;
  run_GridCut[3][ 6][MFI::TYPE_UINT16][MFI::TYPE_UINT16] = run_GridCut_3D_6C<unsigned short,unsigned short>;
  run_GridCut[3][ 6][MFI::TYPE_UINT32][MFI::TYPE_UINT16] = run_GridCut_3D_6C<unsigned int,  unsigned short>;
  run_GridCut[3][ 6][MFI::TYPE_UINT8 ][MFI::TYPE_UINT32] = run_GridCut_3D_6C<unsigned char ,unsigned int  >;
  run_GridCut[3][ 6][MFI::TYPE_UINT16][MFI::TYPE_UINT32] = run_GridCut_3D_6C<unsigned short,unsigned int  >;
  run_GridCut[3][ 6][MFI::TYPE_UINT32][MFI::TYPE_UINT32] = run_GridCut_3D_6C<unsigned int,  unsigned int  >;
  run_GridCut[3][26][MFI::TYPE_UINT8 ][MFI::TYPE_UINT8 ] = run_GridCut_3D_26C<unsigned char ,unsigned char >;
  run_GridCut[3][26][MFI::TYPE_UINT16][MFI::TYPE_UINT8 ] = run_GridCut_3D_26C<unsigned short,unsigned char >;
  run_GridCut[3][26][MFI::TYPE_UINT32][MFI::TYPE_UINT8 ] = run_GridCut_3D_26C<unsigned int,  unsigned char >;
  run_GridCut[3][26][MFI::TYPE_UINT8 ][MFI::TYPE_UINT16] = run_GridCut_3D_26C<unsigned char ,unsigned short>;
  run_GridCut[3][26][MFI::TYPE_UINT16][MFI::TYPE_UINT16] = run_GridCut_3D_26C<unsigned short,unsigned short>;
  run_GridCut[3][26][MFI::TYPE_UINT32][MFI::TYPE_UINT16] = run_GridCut_3D_26C<unsigned int,  unsigned short>;
  run_GridCut[3][26][MFI::TYPE_UINT8 ][MFI::TYPE_UINT32] = run_GridCut_3D_26C<unsigned char ,unsigned int  >;
  run_GridCut[3][26][MFI::TYPE_UINT16][MFI::TYPE_UINT32] = run_GridCut_3D_26C<unsigned short,unsigned int  >;
  run_GridCut[3][26][MFI::TYPE_UINT32][MFI::TYPE_UINT32] = run_GridCut_3D_26C<unsigned int,  unsigned int  >;
    
  const char* dataset_path = argc==2 ? argv[1] : ".";
    
  int num_instances = (sizeof(instances)/sizeof(Instance));
  
  printf("instance                            time-init  time-maxflow_grid  time-maxflow_goldberg  time-output  total\n");
  int* sizes =(int *) malloc(num_instances * sizeof(int)); 

  for(int i=0;i<num_instances;i++)
  {
    double sum_time_init = 0.0;
    double sum_time_maxflow_grid = 0.0;
    double sum_time_maxflow_goldberg = 0.0;
    double sum_time_output = 0.0;
    
    for(int j=0;j<instances[i].count;j++)
    {
      char filename[1024];
      
      if (instances[i].count==1)
      {
        sprintf(filename,instances[i].filename,dataset_path);
      }
      else
      {
        sprintf(filename,instances[i].filename,dataset_path,j);
      }

      MFI* mfi = mfi_read(filename);
      
      if (!mfi)
      {
        printf("FAILED to read instance %s\n",filename);
        return 1;
      }
  
      unsigned char* label = (unsigned char*)malloc(mfi->width*mfi->height*mfi->depth);
          
      int maxflow = -1;

      double time_init;
      double time_maxflow_grid;
      double time_maxflow_goldberg;
      double time_output;

      run_GridCut[mfi->dimension]
                 [mfi->connectivity]
                 [mfi->type_terminal_cap]
                 [mfi->type_neighbor_cap](mfi,label,&maxflow,&time_init,&time_maxflow_grid, &time_maxflow_goldberg,&time_output);

      sum_time_init    += time_init;
      sum_time_maxflow_grid += time_maxflow_grid;
      sum_time_maxflow_goldberg += time_maxflow_goldberg;
      sum_time_output  += time_output;

      if (maxflow != mfi->maxflow)
      {
        printf("INVALID maxflow value returned for instance %s\n",filename);
        return 1;
      }
      sizes[i] = mfi->height * mfi-> width;
      free(label);
        
      mfi_free(mfi);      
    }

    double sum_time_total = sum_time_init + sum_time_maxflow_grid + sum_time_maxflow_goldberg + sum_time_output;

    printf("%-38s % 6.4f        % 6.4f    % 6.4f    % 6.4f % 6.4f\n",
           instances[i].name,sum_time_init,sum_time_maxflow_grid,sum_time_maxflow_goldberg,sum_time_output,sum_time_total);

    writeTime(DSTPATH, sum_time_maxflow_grid, sizes[i] , instances[i].name, "gridcut");
    writeTime(DSTPATH, sum_time_maxflow_goldberg, sizes[i] , instances[i].name, "goldberg");
    
  }
  free(sizes);

  return 0;  
}