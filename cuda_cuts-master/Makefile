CC=/usr/local/cuda-11.7/bin/nvcc

DEBUG=0

ifneq ($(DEBUG), 0)
CFLAGS=-O0 -g -G -DDEBUG=1
else
CFLAGS=-O3 -g -lineinfo
endif

CFLAGS+=-Xcompiler=-Wall -maxrregcount=32 -arch=sm_75
#CFLAGS+=`pkg-config  --cflags --libs`
#CFLAGS+= pkg-config opencv 
#CFLAGS+='--cflags'
# Use to find out shared memory size
 CFLAGS+=--ptxas-options=-v 

FILES=gpu_run #CudaCuts

all: $(FILES)

gpu_run: gpu_run.o CudaCuts.o CudaCuts_kernel.o
	/usr/local/cuda-11.7/bin/nvcc --link  $(CFLAGS) $^ -o $@

#CudaCuts: CudaCuts.o 
#/usr/local/cuda-11.7/bin/nvcc --link  $(CFLAGS) $^ -o $@

#CudaCuts_kernel: CudaCuts_kernel.o 
#	/usr/local/cuda-11.7/bin/nvcc --link  $(CFLAGS) $^ -o $@

CudaCuts.o: CudaCuts.cu CudaCuts.h 

CudaCuts_kernel.o: CudaCuts_kernel.cu CudaCuts.h

gpu_run.o: gpu_run.cu CudaCuts.h


%.o: %.cu
	/usr/local/cuda-11.7/bin/nvcc  --compile -dc $< $(CFLAGS) -o $@

clean::
	rm -f *.o $(FILES)
