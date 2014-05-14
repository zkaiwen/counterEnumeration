/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
 @
 @      graph.hpp
 @	
 @      @AUTHOR:Kevin Zeng
 @      Copyright 2012 – 2013 
 @      Virginia Polytechnic Institute and State University
 @
 @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef GRAPH_GUARD
#define GRAPH_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include "vertex.hpp"

class Graph
{
	private:
		//Vertex ID, Vertex
		std::map<int, Vertex<std::string>*> m_GraphV;

		//Port name, Vertex ID
		std::map<std::string, int> m_Inputs;
		std::map<std::string, int> m_Outputs;
		std::vector<int> m_Constants;
		std::map<unsigned long, int> m_Luts;

		std::string m_Name;
		int m_Last;
		
	public:	
		//Constructors
		Graph(std::string);
		~Graph();
		

		//Importers
		bool importGraph(std::string, int);
		bool importPrimitive(std::string, int);


		//Iterators
		void getIterators(std::map<int, Vertex<std::string>*>::iterator&, std::map<int,Vertex<std::string>*>::iterator&);
		std::map<int,Vertex<std::string>*>::iterator begin();
		std::map<int,Vertex<std::string>*>::iterator end();


		//Getters
		Vertex<std::string>* getVertex(int);
		void getNumType(std::map<std::string, std::vector<Vertex<std::string>*> >&);
		void getInputs(std::vector<int>&);
		void getOutputs(std::vector<int>&);
		unsigned int getNumVertex();
		unsigned int getNumInputs();
		int getNumGate(std::string);
		int getLast();
		unsigned int getMaxLevel();
		std::string getName();
		void getLUTs(std::map<unsigned long, int>&);
		bool hasLUTs();


		//Setters
		void setName(std::string);
		void setLast(int);
		void setLevels();
		void resetLevels();
		void removeCycles();
		

		//Finders
		int findInPort(std::string);
		int findOutPort(std::string);
		std::string findInPortName(int);
		std::string findOutPortName(int);
		
		
		//Graph Functions
		void addVertex(Vertex<std::string>*);
		Vertex<std::string>* addVertex(int, std::string);
		Vertex<std::string>* addVertex(int);
		Vertex<std::string>* addVertex(int, Vertex<std::string>*);
		void addInput(std::string, int);
		void addOutput(std::string, int);
		void addConstant(int);
		void removeVertex(Vertex<std::string>*);
		void removeVertex(int);
		void substitute(int, Graph*);
		void subLUT(int, Graph*);
		void renumber(int);


		//Search 
		void BFSLevel(std::queue<Vertex<std::string>*, std::list<Vertex<std::string>*> >, std::map<int,bool>&, int, int);
		void DFSlevel(Vertex<std::string>*, std::list<int>&, int);
		int DFScycle(Vertex<std::string>*, std::list<int>&);
		int DFSearchIn(std::list<int>& , std::set<int>&,  Vertex<std::string>*, std::set<int>& );
		int DFSearchOut(std::list<int>& , Vertex<std::string>*, std::set<int>& );


		//Print
		void print();
		void printg();
		void printVertex(int);
		void printV(std::string, std::vector<int>&);
		void printV(std::string, std::vector<int>*);
		void print2DV(std::string, std::vector<std::vector<int> >&);
		void print2DV(std::string, std::vector<std::vector<int>* >&);
};

#endif
