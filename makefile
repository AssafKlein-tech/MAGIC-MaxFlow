CXX=g++
RM= rm -f
CXXFLAGS= -Wall -g -std=c++11
EXEC=prog.exe

$(EXEC): main.o goldberg.o goldberg_grid.o
	$(CXX) $(CXXFLAGS) main.o -o $(EXEC)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -fpic main.cpp

goldberg.o: goldberg.cpp
	$(CXX) $(CXXFLAGS) -c -fpic goldberg.cpp

goldberg_grid.o: goldberg_grid.cpp
	$(CXX) $(CXXFLAGS) -c -fpic goldberg_grid.cpp

graph_gen.o: graph_gen.cpp
	$(CXX) $(CXXFLAGS) -c -fpic graph_gen.cpp

clean:
	$(RM) $(EXEC) *.o
