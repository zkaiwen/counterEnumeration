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

using namespace boost;

//Counter
void counterIdentification(Graph* ckt);
int DFS(std::list<int>& mark, Vertex<std::string>* start, Graph* ckt, std::vector<std::vector<int> >& clauses);
void replaceLUTs(Graph* ckt);


//Flags
bool verbose;
std::string s_source;

void printStatement(std::string statement){
		time_t now; 
		struct tm *current;
		now = time(0);
		current = localtime(&now);
		printf("\n\n=============================================================\n");
		printf("[%02d:%02d:%02d]\t%s\n", current->tm_hour, current->tm_min, current->tm_sec, statement.c_str());
		printf("=============================================================\n");

}




int main( int argc, char *argv[] )
{
		printf("\n\n*************************************************************\n");
		printf("*************************************************************\n");
		printf("*\n");
		printf("*	Counter Enumeration\n");
		printf("*	  -ECE 5506 Final Project\n");
		printf("* \n");
		printf("*\tVirginia Polytechnic Institute and State University\n");
		printf("*\tAUTHOR:\tKevin Zeng\n");
		printf("*\tCopyright 2014\n");
		printf("* \n");
		printf("*************************************************************\n");
		printf("*************************************************************\n\n\n\n\n\n\n\n\n\n");
		if(argc < 5)
		{

				printf("./xfpgeniusRef <input G file> <primitive file> <location of prim> <database file>\n\n\n");
				return 0;
		}

		//Declarations
		std::string referenceCircuit = argv[1];
		std::string primBase = argv[2];
		std::string primFunction = argv[3];
		std::string database = argv[4];
		std::string option= "";

		timeval cnt_b, cnt_e;

		double elapsedTime;
		bool counterID = false;

		std::ifstream indb;

		std::map<std::string, std::set<unsigned long> > pDatabase;
		std::map<std::string, double> similarity;

		//database file name, function count
		std::map<std::string, std::map<unsigned long, int> >functionCountMap;

		std::map<std::string, std::map<unsigned long, int> >::iterator fcmit;
		std::map<unsigned long, int>::iterator fcit;







		//Get options
		if(argc == 6){ 
				option = argv[5];
				printf("OPTIONS: %s\n", argv[5]);
		}

		verbose = false;

		if(option.find("v") != std::string::npos)
				verbose = true;
		if(option.find("c") != std::string::npos)
				counterID = true;




		/*************************************
		 *
		 *
		 * PROCESS Reference circuit
		 *
		 *
		 **************************************/
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		printf("\n\n\n\n\n\n\n\n\n\n# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
		printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
		printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
		printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
		std::map<std::string, std::set<unsigned long> > rDatabase;

		printStatement("IMPORTING REFERENCE CIRCUIT:\t" + referenceCircuit + "\n");
		Graph* ckt2 = new Graph(referenceCircuit);
		ckt2->importGraph(referenceCircuit, 0);



		//Counter Identification
		gettimeofday(&cnt_b, NULL);
		s_source = primFunction;
		counterIdentification(ckt2);
		gettimeofday(&cnt_e, NULL);

		if(counterID){
				delete ckt2;
				elapsedTime = (cnt_e.tv_sec - cnt_b.tv_sec) * 1000.0;
				elapsedTime += (cnt_e.tv_usec - cnt_b.tv_usec) / 1000.0;
				printf("EXECUTION TIME -- COUNTER IDENTIFICATION:\n");
				printf(" --  %f ms\n\n", elapsedTime);
				return 0;
		}


		delete ckt2;
		return 0;

}




void counterIdentification(Graph* ckt){
		printf("\n\n\n");
		printStatement("Counter Identification-------------------------------");

		std::set<int> ffset;
		std::set<int>::iterator itff;

		//map of <ff id label, vid in the graph>
		std::map<int, int> vmap;
		std::map<int, int> gb2ffmap;
		std::map<int, int>::iterator mapit;
		std::map<int, Vertex<std::string>*>::iterator it;
		std::vector<int> lsb;
		GraphBoost* lcg = new GraphBoost();
		int ffcount= 0;

		//Find all the flip flops in the circuit
		printf(" -- Finding possible counter FF\n");
		for(it = ckt->begin(); it != ckt->end(); it++){
				if(it->second->getType().find("FD") != std::string::npos){
						ffcount++;

						//Check to see if Q loops back to D (Possible counter)		
						std::list<int> mark;
						std::set<int> ffsetsingle;
						std::set<int> ffFound;
						int vID = it->second->getVertexID();
						ffsetsingle.insert(vID);
						int dport = it->second->getInputPortID("D");

						//check to see if the input is an inverter
						if(ckt->getVertex(dport)->getType() == "INV"){
								//printf("INVERTER INPUT- FF: %d\n", vID);
								std::vector<Vertex<std::string>*> inv_in;
								ckt->getVertex(dport)->getInput(inv_in);
								if(inv_in[0]->getVertexID() == vID){
										//printf("LSB OF COUNTER\n");
										lsb.push_back(vID);
								}
						}

						ckt->DFSearchIn(mark, ffsetsingle, ckt->getVertex(dport), ffFound);
						if(ffFound.size() > 0){
								ffset.insert(vID);
								//printf("POSSIBLE C FF: %d\n", vID);
								int lcgVID = lcg->addVertex(vID);
								vmap[vID] = lcgVID;
								gb2ffmap[lcgVID] = vID;
						}

				}
		}

		assert(vmap.size() == ffset.size());
		printf("done --------------\n\n\n");

		/*printf("PRINTING VMAP\n");
		for(mapit = vmap.begin(); mapit != vmap.end(); mapit++){
				printf("FF: %d - GID: %d\n", mapit->first, mapit->second);
		}
		*/



		printf(" -- Build LCG of Reference Circuit\n");
		//list of map<FF, set of FF that leads into FF
		for(itff = ffset.begin(); itff != ffset.end(); itff++){
				Vertex<std::string>* vff = ckt->getVertex(*itff);

				std::set<int> ffFound; 
				std::list<int> mark;
				ckt->DFSearchOut(mark, vff, ffFound);
				std::set<int>::iterator itffp;

				int src= vmap[vff->getVertexID()];
				for(itffp = ffFound.begin(); itffp != ffFound.end(); itffp++){
						std::map<int,int>::iterator vmapit;
						vmapit = vmap.find(*itffp);
						if(vmapit != vmap.end())
								lcg->addEdge(src, vmapit->second);
				}
		}
		printf("done --------------\n\n\n");
		//lcg->print();

		//Build counter model starting with AT MOST 64 bit counter. 
		//unsigned int initSize = lcg->getNumVertices();

		std::set<int> adjChild;
		std::set<int> adjChild2;
		std::set<int>::iterator ititem;
		std::set<int>::iterator iti;
		std::list<std::vector<int> > candidateFF;
		std::list<std::vector<int> >::iterator it_cand;

		replaceLUTs(ckt);
		AIG* aigraph2 = new AIG();	
		aigraph2->setPrimSource(s_source);
		aigraph2->convertGraph2AIG(ckt, true);
		printf("Conversion Finished\n");

		printf(" -- Searching for Candidate Counters in LCG\n");
		for(unsigned int ff = 0; ff < lcg->getNumVertices(); ff++){
				//printf("Test starting at 67...\n");

				adjChild.clear();
				lcg->getAdjacentOut(ff, adjChild);
				/*	
				printf("SRC: %d\tDEST: ", ff);
				for(iti = adjChild.begin(); iti != adjChild.end(); iti++){
						printf("%d ", *iti);
				}
				printf("\n****************************************************\n\n");
				*/

				//For each adjacent node to the src
				std::set<int> currentCountSet; 
				for(iti = adjChild.begin(); iti != adjChild.end(); iti++){

						//Skip if same
						if(*iti == (int)ff) continue;

						std::vector<int> currentCount;
						currentCount.push_back(ff);
						currentCountSet.insert(ff);

						adjChild2.clear();
						lcg->getAdjacentOut(*iti, adjChild2);
						/*printf("SRC: %d\tDEST: ", *iti);
						for(ititem = adjChild2.begin(); ititem != adjChild2.end(); ititem++)
								printf("%d ", *ititem);
						printf("\n");
						*/
						bool fail = false;
						for(ititem = adjChild2.begin(); ititem != adjChild2.end(); ititem++){
								unsigned int insertIndex; 
								for(unsigned int i = 0; i < currentCount.size() ; i++){
										if(!lcg->isAdjacent(currentCount[i], *ititem)){
												//printf("%d-%d is not adjacent\n", currentCount[i], *ititem);
												fail = true;
												insertIndex = i;
												break;
										}
								}

								if(fail){
										fail = false;
										for(unsigned int i = insertIndex; i < currentCount.size() ; i++){
												if(!lcg->isAdjacent(*ititem, currentCount[i])){
														//printf("%d-%d is not adjacentback\n", *ititem, currentCount[i]);
														fail = true;
														break;
												}
										}

										if(!fail)
												currentCount.insert(currentCount.begin() + insertIndex, *ititem);
								}
								else
										currentCount.push_back(*ititem);
						}
						
						for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
							if(it_cand->size()< currentCount.size()){
								break;
							}
						}
						candidateFF.insert(it_cand, currentCount);

/*						printf("POSSIBLE COUNTER LIST\n");
						for(unsigned int i = 0; i < currentCount.size() ; i++){
								printf("%d ", currentCount[i]);
						}
						printf("\n\n");
						*/
				}
		}
		printf("CANDIDATE COUNTERS ******************************\n");
		for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
				for(unsigned int i = 0; i < it_cand->size() ; i++){
						int gFFVertex =  gb2ffmap[(*it_cand)[i]];
						printf("%d ", gFFVertex);
				}
				printf("\n");

		}

		//Get Combinational fanin for each

		std::set<int> gFF;
		/*printf(" * Latches in Counter...\n");

		  for(unsigned int j = 0; j < candidateFF[0].size(); j++){
		  gFF.insert(gb2ffmap[candidateFF[0][j]]);
		  printf("%d ", gb2ffmap[candidateFF[0][j]]);
		  }
		  printf("\n");
		 */

		printf("\n * Searching for clauses...\n");
		int combo = 0;
		std::vector<int> satisfiable;
		std::set<int> foundLatches;


		for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
				std::vector<std::vector<int> > clauses;	

				if(it_cand->size() < 3){
					candidateFF.erase(it_cand);
					if(candidateFF.size() == 0)
						break;
					it_cand--;
					continue;

				}

				bool alreadyFoundLatch = false;
				for(unsigned int i = 0; i < it_cand->size(); i++){
					if(foundLatches.find((*it_cand)[i]) != foundLatches.end()){
						alreadyFoundLatch = true;
						break;
					}
				}
				if(alreadyFoundLatch){
					candidateFF.erase(it_cand);
					if(candidateFF.size() == 0)
						break;
					it_cand--;
					continue;
				}

				for(unsigned int i = 0; i < it_cand->size(); i++){
						/*
						   printf(" * Latches in Counter...\n");

						   for(unsigned int j = 0; j < i+1; j++){
					   gFF.insert(gb2ffmap[candidateFF[q][j]]);
						   printf("%d ", gb2ffmap[candidateFF[q][j]]);
						   }
						 */

						int gFFVertex =  gb2ffmap[(*it_cand)[i]];
						//printf("\nFF: %d\t", gFFVertex);
						std::list<int> markx;
						int in2FF = ckt->getVertex(gFFVertex)->getInputPortID("D");
						//printf("Input ID: %d\n", in2FF);

						combo+= DFS(markx,ckt->getVertex(in2FF), ckt, clauses);
						//printf("\n\n");
				}


				printf("\n\nExporting all clauses...\n");
				std::stringstream ss;
				std::stringstream cc1;
				std::stringstream cc2;

				ss << "p cnf " << it_cand->size()+combo << " " << clauses.size()<< "\n";
				for(unsigned int i = 0; i < clauses.size(); i++){
						for(unsigned int j = 0; j < clauses[i].size(); j++){
								ss << clauses[i][j] << " " ;
						}
						ss << "0\n";
				}

				//When the bit changes to 1
				printf("creating clause for when counter bit changes to 1\n");

				std::vector<int> inputCandidate;
				inputCandidate.reserve(it_cand->size());

				for(unsigned int i = 0; i < it_cand->size();  i++){
						int gFFVertex =  gb2ffmap[(*it_cand)[i]];
						int in2FF = ckt->getVertex(gFFVertex)->getInputPortID("D");
						inputCandidate.push_back(in2FF);
						//printf("Input ID: %d\n", in2FF);
				}


				int isSat = 1;
				//Check when bit goes low to high
				printf("Performing Low to High Bit transition test\n");
				/*
				for(unsigned int i = 0; i < it_cand->size();  i++){
					int gFFVertex =  gb2ffmap[(*it_cand)[i]];
					//printf("%d ", gFFVertex);

				}
				*/
				//printf("\n");
				for(unsigned int i = 0; i < it_cand->size();  i++){
					
						int gFFVertex =  gb2ffmap[(*it_cand)[i]];
						cc2.str("");
						cc2<<-1*gFFVertex<<" 0\n";
						cc2<<inputCandidate[i]<<" 0\n";
						//for(int j = i+1; j < candidateFF[q].size(); j++)
						for(unsigned int j = 0; j < i; j++)
						{
								int gFFVertex2 =  gb2ffmap[(*it_cand)[j]];
								cc2<<gFFVertex2<<" 0\n";
								cc2<<-1 * inputCandidate[j]<<" 0\n";
						}
						//printf("%s", ss.str().c_str());
						//printf("%s\n\n", cc2.str().c_str());
						std::ofstream fs("cnf");
						fs<<ss.str()<<cc2.str();
						fs.close();
						

						//Run Minisat
						std::system("./minisat cnf out");
						std::ifstream is("out");
						std::string sat;
						is>>sat;
						is.close();
						if(sat == "UNSAT"){
								isSat = 0;
								break;
						}
						else if(sat != "SAT")
								printf("UNKNOWN LINE IN MINISAT OUTPUT FILE\n");
				}

				//Check when bit stays high
				if(isSat){
				printf("Performing High stay High test\n");
				//Omit the first bit since the first bit always toggles
				for(unsigned int i = 1; i < it_cand->size();  i++){
						int gFFVertex =  gb2ffmap[(*it_cand)[i]];
						cc2.str("");
						cc2<<gFFVertex<<" 0\n";
						cc2<<inputCandidate[i]<<" 0\n";
						bool into = false;
						for(unsigned int j = 0; j < i; j++)
						{
								int gFFVertex2 =  gb2ffmap[(*it_cand)[j]];
								cc2<<-1* gFFVertex2<<" ";
								into = true;
						}
						if(into)
							cc2<<"0\n";
						std::ofstream fs("cnf");
						fs<<ss.str()<<cc2.str();
						fs.close();
						//printf("%s\n\n", cc2.str().c_str());
						

						//Run Minisat
						std::system("./minisat cnf out");
						std::ifstream is("out");
						std::string sat;
						is>>sat;
						is.close();
						if(sat == "UNSAT"){
								isSat = 0;
								break;
						}
						else if(sat != "SAT")
								printf("UNKNOWN LINE IN MINISAT OUTPUT FILE\n");

						
				}
				}

				//CHeck to see if counter is satisfiable
				if(isSat){
					for(unsigned int i = 0; i < it_cand->size();  i++){
						foundLatches.insert((*it_cand)[i]);
					}
				}

				satisfiable.push_back(isSat);
		}

		int numCounter = 0;
		int index = 0; 
		printf("RESULTS------------------------------\n");
		for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
				for(unsigned int j = 0; j < it_cand->size(); j++){
						int gFFVertex =  gb2ffmap[(*it_cand)[j]];
						printf("%d ", gFFVertex);
				}
				printf(" ---------------\t");
				if(satisfiable[index] == 1){
						printf("SAT\n");
						numCounter++;
				}
				else
						printf("UNSAT\n");
				index++;
		}
		printf("NUM OF COUNTERS: %d\n", numCounter);
		printf("NUM OF LATCHES:  %d\n", ffcount);
}







void replaceLUTs(Graph* ckt){
		printf("-- Replacing LUTS with combinational logic\n");
		std::map<int, Vertex<std::string>*>::iterator it;
		std::vector<int> tobedeleted;
		for(it = ckt->begin(); it != ckt->end(); it++){
			if(it->second->getType().find("LUT") != std::string::npos){
				tobedeleted.push_back(it->first);
				std::vector<Vertex<std::string>*> inputs;
				it->second->getInput(inputs);
		
				if(inputs.size() == 1){
					//printf("LUT SIZE 1\n");
					unsigned long function = it->second->getLUT();
					if(function == 2){
						std::vector<Vertex<std::string>*> linput;
						std::vector<Vertex<std::string>*> loutput;

						it->second->getInput(linput);
						it->second->getOutput(loutput);

						assert(linput.size() == 1);
						std::string outportname = linput[0]->removeOutputValue(it->second->getVertexID());
					
						for(unsigned int i = 0; i < loutput.size(); i++){
							std::string portname = loutput[i]->getInputPortName(it->second->getVertexID());
							int index = loutput[i]->removeInputValue(it->second->getVertexID());
							loutput[i]->removeInPortValue(index);
							loutput[i]->addInput(linput[0]);
							loutput[i]->addInPort(portname);

							linput[0]->addOutput(loutput[i],outportname);

						}


					}
					else{
						printf("FUNCTION IS INVERTER\n");
						exit(1);
					}

					continue;
				}
				std::vector<std::string> inports;
				it->second->getInputPorts(inports);

				//LSB-MSB
				std::vector<int> lutin;
				std::string pname = "I ";
				char startChar =  48;

				int numbers = 1;
				for(unsigned int i = 0; i < inputs.size(); i++){
						numbers*=2;
						pname[1] = startChar + i;
						printf("%s:", pname.c_str());
						for(unsigned int j = 0; j < inports.size(); j++){
								if(inports[j] == pname){
										lutin.push_back(inputs[j]->getVertexID());
										printf("%d ", inputs[j]->getVertexID());
								}
						}
				}
				printf("\n");

				unsigned long function = it->second->getLUT();
				//printf("%lx Possible numbers: %d\n", function, numbers); 
				Graph* lutgraph = new Graph ("LUT");
				//Pre-Invert inputs for later use;
				//printf("ASSIGNING INPUTS\n");
				for(unsigned int i = 0; i < lutin.size(); i++){
					std::stringstream ss; 
					ss<<"I"<<i;
					Vertex<std::string>* vin = lutgraph->addVertex(i, "IN");
					Vertex<std::string>* vinv = lutgraph->addVertex(lutin.size()+i, "INV");
				
					vinv->addInput(vin);
					vinv->addInPort("I");
					
					vin->addOutput(vinv, "O");

					lutgraph->addInput(ss.str(), i);
					//printf("LUTIN: %d\tLUTGID: %d\tPORTNAME: %s\n", lutin[i], i, ss.str().c_str());
				}

				int andIndexStart = lutgraph->getNumVertex();
				int count = 0;

				for(int i = 0; i < numbers; i++){
					//If there is a 1 in the truth table
					if((0x1 & function) == 1){
						count++;
						std::stringstream ss; 
						ss<<"AND"<<lutin.size();
						Vertex<std::string>* aGate = lutgraph->addVertex(lutgraph->getNumVertex(), ss.str());

						int mask = 1;
						for(unsigned int j = 0; j < lutin.size(); j++){
							std::stringstream portname; 
							portname<<"I"<<j;
							aGate->addInPort(portname.str());

							if((mask & i) > 0){
								aGate->addInput(lutgraph->getVertex(j));
								lutgraph->getVertex(j)->addOutput(aGate, "O");
							}
							else{
								aGate->addInput(lutgraph->getVertex(lutin.size()+j));
								lutgraph->getVertex(lutin.size()+j)->addOutput(aGate, "O");
							}
							mask = mask << 1;
							
						}
					}
						
					function= function>> 1;
				}
				if(count> 36){
					printf("NUMBER OF ONES IN TRUTH TABLE EXCEED 36. Please Adjust Code\n");
				}


				//Count how many and gates there are 
				int numOrGates = (lutgraph->getNumVertex() - andIndexStart) / 6;
				int numAndGatesLeft = (lutgraph->getNumVertex() - andIndexStart) % 6;
				int orIndexStart = lutgraph->getNumVertex();
				
				for(int i = 0; i < numOrGates; i++){
					Vertex<std::string>* oGate = lutgraph->addVertex(lutgraph->getNumVertex(), "OR6");
							
					for(int q = 0; q < 6; q++){
						std::stringstream portname; 
						portname<<"I"<<q;
						oGate->addInPort(portname.str());

						oGate->addInput(lutgraph->getVertex(andIndexStart+(6*i)+q));
						lutgraph->getVertex(andIndexStart+(6*i)+q)->addOutput(oGate, "O");
					}
				}

				std::stringstream orgatename;
				Vertex<std::string>* oGate;
				if(numAndGatesLeft > 1){
				orgatename<<"OR"<<numAndGatesLeft;

				oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());
							
				for(int q = 0; q < numAndGatesLeft; q++){
					std::stringstream portname; 
					portname<<"I"<<q;
					oGate->addInPort(portname.str());

					oGate->addInput(lutgraph->getVertex(andIndexStart+(6*numOrGates)+q));
					lutgraph->getVertex(andIndexStart+(6*numOrGates)+q)->addOutput(oGate, "O");
				}

				}

				//COmbine or gates
				int numOrGatesLeft = lutgraph->getNumVertex()-orIndexStart;
				if(numOrGates > 0 && numOrGatesLeft > 1){
					orgatename.str("");
					orgatename<<"OR"<< numOrGatesLeft;
					oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());
							
					for(int q = 0; q < numOrGatesLeft; q++){
						std::stringstream portname; 
						portname<<"I"<<q;
						oGate->addInPort(portname.str());

						oGate->addInput(lutgraph->getVertex(orIndexStart+q));
						lutgraph->getVertex(orIndexStart+q)->addOutput(oGate, "O");
					}

				}
				
				lutgraph->addOutput("O", lutgraph->getNumVertex()-1);

				
				
				//printf("PRINTING LUT GRAPH\n");
				//lutgraph->print();
				lutgraph->renumber(ckt->getLast() + 1);
				//lutgraph->print();
				ckt->substitute(it->second->getVertexID(), lutgraph);
				//ckt->print();

				//Go through each entry in the truth table
				/*
				for(int i = 0; i < numbers; i++){
						//printf("COMPARE:  MASK: %lx FUNCTION: %lx  I:%d\n", mask, function, i);
						//See if the bit is zero
						if((mask & function) == 0){

								//Store the clause
								int clauseMask = 1;
								std::vector<int> clause;
								for(unsigned int j = 0; j < lutin.size(); j++){
										if((clauseMask & i) > 0)	
												clause.push_back(lutin[j]*-1);
										else
												clause.push_back(lutin[j]);

										clauseMask = clauseMask << 1;
								}
								clause.push_back(-1 * start->getVertexID());
								printf("CLAUSE: LUT: ");
								for(unsigned int j = 0; j < clause.size(); j++){
										printf("%d ", clause[j]);
								}
								printf("\n");
								clauses.push_back(clause);

						}
						function= function>> 1;
				}
				*/
		}
	}

	for(unsigned int i = 0; i < tobedeleted.size(); i++){
		ckt->removeVertex(tobedeleted[i]);	
	}


	
}








int DFS(std::list<int>& mark, Vertex<std::string>* start, Graph* ckt, std::vector<std::vector<int> >& clauses){
		mark.push_back(start->getVertexID());

		std::string type = start->getType();

		std::vector<Vertex<std::string>*> inputs;
		start->getInput(inputs);

		//EXTRACT CLAUSES 	
		if(type == "INV"){
				//printf("INV\n");
				std::vector<int> clause1;
				std::vector<int> clause2;
				clause1.push_back(-1*inputs[0]->getVertexID());
				clause1.push_back(-1 * start->getVertexID());
				//printf("CLAUSE: INV: %d %d\n", clause1[0], clause1[1]);

				clause2.push_back(inputs[0]->getVertexID());
				clause2.push_back(start->getVertexID());
				//printf("CLAUSE: INV: %d %d\n", clause2[0], clause2[1]);

				clauses.push_back(clause1);
				clauses.push_back(clause2);
		}
		else if(type.find("AND") != std::string::npos){
				//	printf("AND\n");
				std::vector<int> clause1;
				std::vector<int> clause2;
				std::vector<int> clause3;
				clause1.push_back(-1*inputs[0]->getVertexID());
				clause1.push_back(-1*inputs[1]->getVertexID());
				clause1.push_back(start->getVertexID());
				//printf("CLAUSE: AND: %d %d\n", clause1[0], clause1[1]);

				clause2.push_back(inputs[0]->getVertexID());
				clause2.push_back(-1 * start->getVertexID());
				//printf("CLAUSE: AND: %d %d\n", clause2[0], clause2[1]);

				clause3.push_back(inputs[1]->getVertexID());
				clause3.push_back(-1 * start->getVertexID());
				//	printf("CLAUSE: AND: %d %d\n", clause3[0], clause3[1]);

				clauses.push_back(clause1);
				clauses.push_back(clause2);
				clauses.push_back(clause3);

		}
		else if (type.find("XOR") != std::string::npos){
				printf("XOR\n");
				std::vector<int> clause1;
				std::vector<int> clause2;
				std::vector<int> clause3;
				std::vector<int> clause4;
				clause1.push_back(inputs[0]->getVertexID());
				clause1.push_back(inputs[1]->getVertexID());
				clause1.push_back(-1 * start->getVertexID());

				clause2.push_back(-1 * inputs[0]->getVertexID());
				clause2.push_back(-1 * inputs[1]->getVertexID());
				clause2.push_back(-1 * start->getVertexID());

				clause3.push_back(-1 * inputs[0]->getVertexID());
				clause3.push_back(inputs[1]->getVertexID());
				clause3.push_back(start->getVertexID());

				clause4.push_back(inputs[0]->getVertexID());
				clause4.push_back(-1 * inputs[1]->getVertexID());
				clause4.push_back(start->getVertexID());

				clauses.push_back(clause1);
				clauses.push_back(clause2);
				clauses.push_back(clause3);
				clauses.push_back(clause4);
		}

		else if (type.find("MUX") != std::string::npos){
				printf("MUX\n");
				std::vector<std::string> inports;
				start->getInputPorts(inports);
				int s, in0, in1;

				for(unsigned int i = 0; i < inports.size(); i++){
						if(inports[i] == "DI")
								in0 = inputs[i]->getVertexID();
						else if(inports[i] == "CI")
								in1 = inputs[i]->getVertexID();
						else
								s = inputs[i]->getVertexID();
				}

				std::vector<int> clause1;
				std::vector<int> clause2;
				std::vector<int> clause3;
				std::vector<int> clause4;

				clause1.push_back(in0);
				clause1.push_back(in1);
				clause1.push_back(s);
				clause1.push_back(-1 * start->getVertexID());

				clause2.push_back(in0);
				clause2.push_back(-1 * in1);
				clause2.push_back(s);
				clause2.push_back(-1 * start->getVertexID());

				clause3.push_back(in0);
				clause3.push_back(in1);
				clause3.push_back(-1* s);
				clause3.push_back(-1 * start->getVertexID());

				clause4.push_back(-1 * in0);
				clause4.push_back(in1);
				clause4.push_back(-1 * s);
				clause4.push_back(-1 * start->getVertexID());

				clauses.push_back(clause1);
				clauses.push_back(clause2);
				clauses.push_back(clause3);
				clauses.push_back(clause4);
		}
		else if(type.find("VCC")!= std::string::npos){
				std::vector<int> clause;
				clause.push_back(start->getVertexID());
				clauses.push_back(clause);
		}
		else if(type.find("GND")!= std::string::npos){
				std::vector<int> clause;
				clause.push_back(-1 * start->getVertexID());
				clauses.push_back(clause);
		}
		else if (type.find("IN")!= std::string::npos){

		}
		else{
				printf("TYPE UNKNOWN: %s\n", type.c_str());
				exit(1);
		}


		//SEARCH FOR NEXT
		int combo = 0; 
		for(unsigned int i = 0; i < inputs.size(); i++){
				if(inputs[i]->getType().find("FD") == std::string::npos){
						combo+= DFS(mark,inputs[i], ckt, clauses);
				}
		}

		return combo;
}





#endif
