ex2-segmentation: ex2-segmentation.cpp
	g++ -I../../include -I../include -o ex2-segmentation ex2-segmentation.cpp

colors: ex2-colors.cpp
	g++ -I../../include -I../include -o colors ex2-colors.cpp

parallel: parallel_solver.cpp
	g++ -I../../include -I../include  -pthread -o parallel parallel_solver.cpp
	
clean:
	rm -f ex2-segmentation ex2-colors parallel

