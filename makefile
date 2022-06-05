CXX=g++
RM= rm -f
CXXFLAGS= -Wall -g -std=c++11
EXEC=prog.exe

$(EXEC): main.o goldberg.o
	$(CXX) $(CXXFLAGS) main.o -o $(EXEC)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -fpic main.cpp

goldberg.o: goldberg.cpp
	$(CXX) $(CXXFLAGS) -c -fpic goldberg.cpp

graph_gen.o: graph_gen.cpp
	$(CXX) $(CXXFLAGS) -c -fpic graph_gen.cpp

clean:
	$(RM) $(EXEC) *.o
