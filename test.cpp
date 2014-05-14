
/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  MAIN.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef MAIN_GUARD
#define MAIN_GUARD
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vf2_sub_graph_iso.hpp>

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "graph_b.hpp"
#include "cutenumeration.hpp"
#include "cutfunction.hpp"
#include <iostream>
	
void printStatement(std::string statement){
	time_t now; 
	struct tm *current;
	now = time(0);
	current = localtime(&now);
	printf("\n\n=============================================================\n");
	printf("[%02d:%02d:%02d]\t%s\n", current->tm_hour, current->tm_min, current->tm_sec, statement.c_str());
	printf("=============================================================\n");

}
	
//Unit Test Classes
void testGraphBoost();	

int main(){
	
	//Test Graph Boost
	testGraphBoost();


	return 0;	
}


/**************************************************************
* 
* Unit Test Function for the class: GraphBoost
*
*/
void testGraphBoost(){
	printStatement("Testing Class:\tGraphBoost...");
	
	printf(" - Constructor:\n");	
	std::cout<<" --- "<<"Creating Graph with 10 Vertices\n";
	GraphBoost* gb = new GraphBoost(10);
	assert(gb->getNumVertices() == 10);
	delete gb;

	std::cout<<" --- "<<"Creating Graph with -1 Vertices\n";
	gb = new GraphBoost(-1);
	assert(gb->getNumVertices() == 0);
	delete gb;
	printf("\n -- PASS------------------\n");
	
	printf("\n\n----------------------------------------------------------------------\n");
	printf("\n - Function: addVertex:\n");	
	std::cout<<" --- "<<"Creating Graph with 10 Vertices\n";
	gb = new GraphBoost(10);
	assert(gb->getNumVertices() == 10);
	
	std::cout<<" --- "<<"Adding 1 vertex to the graph\n";
	int v = gb->addVertex();
	assert(gb->getNumVertices() == 11);
	assert(v == 10);

	std::cout<<" --- "<<"Adding 5 vertex to the graph\n";
	v = gb->addVertex();
	assert(v == 11);
	v = gb->addVertex();
	assert(v == 12);
	v = gb->addVertex();
	assert(v == 13);
	v = gb->addVertex();
	assert(v == 14);
	v = gb->addVertex();
	assert(v == 15);
	assert(gb->getNumVertices() == 16);
	printf("\n -- PASS------------------\n");

/*
	printf("----------------------------------------------------------------------\n");
	printf("\n - Function: setVID and getVID:\n");	
	std::cout<<" --- "<<"Adding 1-6 vertex labels to label squared\n";
	gb->setVID(1, 1);
	gb->setVID(2, 4);
	gb->setVID(3, 9);
	gb->setVID(4, 16);
	gb->setVID(5, 25);
	gb->setVID(6, 36);

	std::cout<<" --- "<<"Checking labels to see if they are label squared\n";
	assert(gb->getVID(1) == 1);
	assert(gb->getVID(2) == 4);
	assert(gb->getVID(3) == 9);
	assert(gb->getVID(4) == 16);
	assert(gb->getVID(5) == 25);
	assert(gb->getVID(6) == 36);
	std::cout<<" --- "<<"Checking unset labels\n";
	assert(gb->getVID(7) == 0);
	assert(gb->getVID(8) == 0);
	assert(gb->getVID(9) == 0);
	std::cout<<" --- "<<"Checking Out of bounds vertex reference\n";
	assert(gb->getVID(200) == 0);

	printf("\n -- PASS------------------\n");
	*/

	
	printf("\n\n----------------------------------------------------------------------\n");
	printf("\n - Function: addVertex:\n");	
	std::cout<<" --- "<<"Removing vertices \n";
	gb->removeVertex(0);
	assert(gb->getNumVertices() == 15);  
	std::cout<<" --- "<<"Removing invalid vertex \n";
	gb->removeVertex(100);
	std::cout<<"v: "<<gb->getNumVertices()<<"\n";
	assert(gb->getNumVertices() == 15);
	std::cout<<" --- "<<"Removing a removed vertex \n";
	gb->removeVertex(0);
	assert(gb->getNumVertices() == 14);
	printf("\n -- PASS------------------\n");
	
	
	
	printf("\n\n----------------------------------------------------------------------\n");
	printf("\n - Function: addEdge, getAdjacentOut:\n");	
	std::cout<<" --- "<<"Adding Edges\n";
	gb->addEdge(0,1);
	gb->addEdge(0,8);
	gb->addEdge(0,6);
	gb->addEdge(1,2);
	gb->addEdge(2,3);
	gb->addEdge(3,4);
	gb->addEdge(4,5);
	gb->addEdge(5,6);
	gb->addEdge(6,7);
	gb->addEdge(6,0);
	gb->addEdge(7,8);

	std::cout<<" --- "<<"Checking Adjacencies\n";
	std::vector<int> adj; 
	gb->getAdjacentOut(0, adj);
	assert(adj.size() == 3);
	adj.clear();
	gb->getAdjacentOut(6, adj);
	assert(adj.size() == 2);
	adj.clear();
	gb->getAdjacentOut(2, adj);
	assert(adj.size() == 1);
	adj.clear();
	gb->getAdjacentOut(8, adj);
	assert(adj.size() == 0);
	adj.clear();
	printf("\n -- PASS------------------\n");
}

#endif
