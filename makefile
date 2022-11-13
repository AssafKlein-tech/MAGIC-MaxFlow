CXX=g++ 
RM= rm -f
#CXXFLAGS= -Wall -g -std=c++11
EXEC=prog.exe

$(EXEC): main.o goldberg.o goldberg_grid.o image_read.o
	$(CXX) $(CXXFLAGS) main.o -o $(EXEC)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

goldberg.o: goldberg.cpp
	$(CXX) $(CXXFLAGS) -c goldberg.cpp

goldberg_grid.o: goldberg_grid.cpp
	$(CXX) $(CXXFLAGS) -c goldberg_grid.cpp

graph_gen.o: graph_gen.cpp
	$(CXX) $(CXXFLAGS) -c graph_gen.cpp

image_read.o: image_read.cpp
	$(CXX) $(CXXFLAGS) -c image_read.cpp Image.h

clean:
	$(RM) $(EXEC) *.o
