/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  graph_b.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#include "graph_b.hpp"


GraphBoost::GraphBoost(int numVertices)	{
	if(numVertices < 0){
		numVertices = 0;
		printf("[ WARNING ] -- Graph cannot have negative vertices. Setting Empty Graph\n");
	}
	graph_t tmp = (numVertices);
	circuit = tmp;

	//for(int i = 0; i < getNumVertices(); i++)
	//	circuit[i].vID = 0;
}

GraphBoost::GraphBoost(){
}

		
void GraphBoost::addEdge(int src, int dst){
	add_edge(src, dst, circuit);
}

int GraphBoost::addVertex(){
	vertex_t vertex = add_vertex(circuit);
	//circuit[vertex].vID = 0;
	return (int) vertex;
}

int GraphBoost::addVertex(int label){
	vertex_t vertex = add_vertex(circuit);
	//circuit[vertex].vID = label;
	return (int) vertex;
}

bool GraphBoost::removeVertex(unsigned int vertex){
	if(vertex >= num_vertices(circuit)){
		printf("[ ERROR ] -- GraphBoost::removeVertex\n");
		printf("          --- vertex: %u\tmaxVal: %d\n", vertex, (int)num_vertices(circuit)); 
		return false;
	}
	clear_vertex(vertex, circuit);
	remove_vertex(vertex, circuit);
	return true; 
}

unsigned int GraphBoost::getNumVertices(){	
	return num_vertices(circuit);
}

void GraphBoost::getGraph(graph_t& graph){
	graph = circuit;
}

void GraphBoost::getAdjacentOut(int invertex, std::set<int>& adj){
	if(invertex >= (int) num_vertices(circuit))
		return;

	boost::graph_traits<graph_t>::adjacency_iterator it, adj_end;
	vertex_t v = vertex(invertex, circuit);
	for(boost::tie(it, adj_end) = adjacent_vertices(v, circuit);  it != adj_end; it++){
		adj.insert(*it);
	}
}

void GraphBoost::getAdjacentIn(int invertex, std::set<int>& adj){
	if(invertex >= (int) num_vertices(circuit))
		return;

	graph_t::inv_adjacency_iterator it, adj_end;
	vertex_t v = vertex(invertex, circuit);
	for(boost::tie(it, adj_end) = inv_adjacent_vertices(v, circuit);  it != adj_end; it++){
		adj.insert(*it);
	}
}

bool GraphBoost::isAdjacent(int src, int dst){

	if(src>= (int) num_vertices(circuit))
		return false;
	if(dst>= (int) num_vertices(circuit))
		return false;

	boost::graph_traits<graph_t>::adjacency_iterator it, adj_end;
	vertex_t v = vertex(src, circuit);
	for(boost::tie(it, adj_end) = adjacent_vertices(v, circuit);  it != adj_end; it++){
		if((int) *it == dst)
			return true;
	}
	return false;
}

/*		
void GraphBoost::setVID(int vertex, int label){
	circuit[vertex].vID = label;
}

int GraphBoost::getVID(int vertex){	
	return circuit[vertex].vID;
}
*/

void GraphBoost::print(){
	boost::graph_traits<graph_t>::adjacency_iterator it, adj_end;
	for(unsigned int i = 0; i < num_vertices(circuit);i++){
		printf("SRC: %4d\tDEST: ", i);
		vertex_t v = vertex(i, circuit);
		for(boost::tie(it, adj_end) = adjacent_vertices(v, circuit);  it != adj_end; it++){
			printf("%lu ", *it);
		}
		
		printf("\n");

	}
}


  // Default print_callback
  template <typename Graph1,
            typename Graph2>
  struct vf2_callback{
   	/*******************
	* mapping List of a list of mappings.
	* Even Index is subgraph
	* Odd index is current graph	
	*/ 
    vf2_callback(const Graph1& graph1, const Graph2& graph2, std::vector<std::vector<int> >&  mapping)
      : graph1_(graph1), graph2_(graph2), map(mapping){}
    
    template <typename CorrespondenceMap1To2,
              typename CorrespondenceMap2To1>
    bool operator()(CorrespondenceMap1To2 f, CorrespondenceMap2To1) const {
		std::vector<int> small;
		std::vector<int> large;
      
      // Print (sub)graph isomorphism map
	  /*
      BGL_FORALL_VERTICES_T(v, graph1_, Graph1) 
        std::cout << '(' << get(vertex_index_t(), graph1_, v) << ", " 
                  << get(vertex_index_t(), graph2_, get(f, v)) << ") ";
      
      std::cout << std::endl;
	  */

	//Store mapping into vector to pass back
	BGL_FORALL_VERTICES_T(v, graph1_, Graph1){ 
		small.push_back(get(vertex_index_t(), graph1_, v));
		large.push_back(get(vertex_index_t(), graph2_, get(f,v)));
	}
      
	map.push_back(small);
	map.push_back(large);
	
      
      return true;
    }
    
  private:
    const Graph1& graph1_;
    const Graph2& graph2_;
	std::vector<std::vector<int> >& map;
  };

bool GraphBoost::vf2(GraphBoost* subgraph){
	printf("[ * ] -- Performing VF2 Subgraph Isomorphism\n");
	graph_t subcircuit;
	subgraph->getGraph(subcircuit);

	// Create callback to print mappings
	std::vector<std::vector<int> > mapping;
	vf2_callback<graph_t, graph_t> callback(subcircuit, circuit, mapping);

	// Print out all subgraph isomorphism mappings between graph1 and graph2.
	// Vertices and edges are assumed to be always equivalent.
	vf2_subgraph_iso(subcircuit, circuit, callback);


	printf("(Vertex in subgraph) - (Vertex in graph)\n");
	for(unsigned int i = 0; i < mapping.size(); i=i+2){
		printf("\t");
		for(unsigned int j = 0; j  < mapping[i].size(); j++){
			printf("%d-%d  ", mapping[i][j], mapping[i+1][j]);	
		}
		printf("\n");
	}
	

	
	return true;
}




















