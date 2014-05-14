CFLAGS = \
		-Wall \
		-g 
		

OBJ= \
		graph.o \
		graph_b.o \
		aig.o \
		aiger.o

PROF=

all: mainRef 

mainRef: $(OBJ) mainRef.o 
	g++ $(CFLAGS) -o xfpgeniusRef $(OBJ) mainRef.o -lboost_graph

graph.o: graph.cpp graph.hpp vertex.hpp
	g++ $(CFLAGS) -c graph.cpp $(PROF)

graph_b.o: graph_b.cpp graph_b.hpp 
	g++ $(CFLAGS) -c graph_b.cpp $(PROF)

mainRef.o: mainRef.cpp graph.o aig.o
	g++ $(CFLAGS) -c mainRef.cpp $(PROF) 

aig.o: aig.cpp aig.hpp graph.o
	g++ $(CFLAGS) -c aig.cpp $(PROF)

aiger.o: aiger.c aiger.h 
	gcc $(CFLAGS) -c aiger.c 

test: 
	g++ -o testProgram $(OBJ) test.cpp -lboost_graph

clean:
	rm -f *.o *fpgenius* *out* testProgram cnf

