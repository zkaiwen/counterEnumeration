
/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  graph_b.hpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef GRAPH_B_GUARD 
#define GRAPH_B_GUARD 
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vf2_sub_graph_iso.hpp>
#include <set>
#include <stdlib.h>
#include <stdio.h>

using namespace boost;
/*
struct vProperties{
	int vID;
};
*/

class GraphBoost{
	public:

	//	typedef property<vIndex_t, int> vertex_property;
		// Using a vecS graphs => the index maps are implicit.
	//typedef adjacency_list<vecS, vecS, directedS, vProperties > graph_t;
	typedef adjacency_list<vecS, vecS, bidirectionalS> graph_t;
	typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
	typedef boost::graph_traits<graph_t>::edge_descriptor edge_t;
	typedef boost::graph_traits<graph_t>::out_edge_iterator out_edge_iterator;


		//Functions
		GraphBoost();	
		GraphBoost(int);	//num vertices in the graph

		void addEdge(int, int);	//src, dst

		int addVertex();
		int addVertex(int);
		bool removeVertex(unsigned int);

		unsigned int getNumVertices(); 
		void getGraph(graph_t&);
		void getAdjacentIn(int, std::set<int>& );
		void getAdjacentOut(int, std::set<int>&);
		bool isAdjacent(int, int);

//		void setVID(int, int);
//		int getVID(int);

		bool vf2(GraphBoost*);

		void print();
			


			
	private:

		graph_t circuit; 	
};

#endif
