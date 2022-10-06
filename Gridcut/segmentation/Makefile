all: bench-GridCut

bench-GridCut: bench-GridCut.cpp dataset.h mfi.cpp mfi.h timer.cpp timer.h goldberg_grid.cpp
	g++ -I./code/GridCut/include -DNDEBUG -O3 -march=native -mtune=generic -o bench-GridCut bench-GridCut.cpp mfi.cpp  timer.cpp

clean:
	rm -f bench-GridCut bench-BK301

