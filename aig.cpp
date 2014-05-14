/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      aig.cpp
  @      
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @      Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "aig.hpp"




/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     Constructors
####*
############################################################## 
##############################################################*/

//Static Variables
std::string AIG::s_SourcePrim = "";

/*******************************************************
 *  Constructor
 *    Empty
 ********************************************************/
AIG::AIG(){
	printf("[AIG]--Constructor\n");
	m_Aiger = aiger_init();
	m_Last = 0;
	
}



/*******************************************************
 *  Destructor
 *    Deletes allocated space. Does not delete AIG Graph
 ********************************************************/
AIG::~AIG(){
	clearAIG();
}




/*******************************************************
 *  clearAIG 
 *    Clears the AIG graph completely
 ********************************************************/
void AIG::clearAIG(){
	m_HashTable.clear();
	m_Outputs.clear();
	m_GateMap.clear();
	aiger_reset(m_Aiger);
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *    GETTER METHODS 
####*
############################################################## 
##############################################################*/



/*******************************************************
 *  getOutputs 
 *    Gets the PO of the AIG
 *
 *    @PARAM: 
 *      * output: Reference to the output vector
 ********************************************************/
void AIG::getOutputs(std::vector<unsigned>& output){
	output = m_Outputs;
}



/*******************************************************
 *  getOutputSize
 *    Gets the output size of the AIG
 *
 *    @RETURN: Number of outputs of the AIG 
 ********************************************************/
unsigned int AIG::getOutputSize(){
	return m_Aiger->num_outputs; 
}



/*******************************************************
 *  getInputSize
 *    Gets the Input size of the AIG
 *
 *    @RETURN: Number of inputs of the AIG 
 ********************************************************/
unsigned int AIG::getInputSize(){
	return m_Aiger->num_inputs;
}



/*******************************************************
 *  getSize
 *    Gets the size of the AIG
 *
 *    @RETURN: Size of the AIG 
 ********************************************************/
unsigned int AIG::getSize(){
	return m_Aiger->num_ands; 
}



/*******************************************************
 *  getChild1 
 *    Gets the first child given a node
 *
 *    @PARAM: 
 *      * index: Node to get child 1 from
 *
 *    @RETURN: Node ID of Child1
 ********************************************************/
unsigned AIG::getChild1(unsigned index){
	//printf("index %d\n", index);
	index= (index - ((m_Aiger->num_inputs+1) *2))/2;	
	//printf("index %d\n", index);
	return m_Aiger->ands[index].rhs0; 
}



/*******************************************************
 *  getChild2
 *    Gets the first child given a node
 *
 *    @PARAM: 
 *      * index: Node to get child 2 from
 *
 *    @RETURN: Node ID of Child2
 ********************************************************/
unsigned AIG::getChild2(unsigned index){
	//printf("index %d\n", index);
	index= (index - ((m_Aiger->num_inputs+1) *2))/2;	
	//printf("index %d\n", index);
	return m_Aiger->ands[index].rhs1;
}



/*******************************************************
 *  getGNode 
 *    Gets the Graph Node ID give an AIG node ID
 *
 *    @PARAM: 
 *      * aigID: Node of the AIG ID
 *
 *    @RETURN Graph Node ID
 ********************************************************/
unsigned AIG::getGNode(unsigned aigID){
	std::map<int,unsigned >::iterator it;
	for(it = m_GateMap.begin(); it != m_GateMap.end(); it++)
		if(it->second == aigID)
			return it->first;

	printf("Cannot find Graph node for AIG ID: %d\n", aigID);
	printMap();
	exit(1);
}




/*******************************************************
 *  getGNode 
 *    Gets the Graph Node ID give an AIG node ID
 *
 *    @PARAM: 
 *      * aigID: Node of the AIG ID
 *
 *    @RETURN Graph Node ID
 ********************************************************/
std::vector<unsigned >  AIG::findNode(unsigned c1, unsigned c2){
	std::vector<unsigned > nodes;
	for(unsigned int i = 0; i < m_Aiger->num_ands; i++){
		unsigned child1 = m_Aiger->ands[i].rhs0;
		unsigned child2 = m_Aiger->ands[i].rhs1;

		//Negate to get the nodes
		child1 = child1 & 0xFFFFFFFE;	
		child2 = child2 & 0xFFFFFFFE;	
			
		if((child1 == c1 &&
					child2 == c2) ||
				(child2 == c1 &&
				 child1 == c2)){
			nodes.push_back(i);
		}
	}
	return nodes;
}




bool AIG::hasChild(unsigned child, unsigned exception){
	unsigned int ex = exception;
	child = child & 0xFFFFFFFE;	
	if(ex > m_Aiger->num_ands) return false;

	for(unsigned int i = 0; i < ex; i++){
		unsigned child1 = m_Aiger->ands[i].rhs0;
		unsigned child2 = m_Aiger->ands[i].rhs1;

		child1 = child1 & 0xFFFFFFFE;	
		child2 = child2 & 0xFFFFFFFE;	

		if(child1 == child || child2 == child)
			return true;
	}

	for(unsigned int i = ex + 1; i < m_Aiger->num_ands; i++){
		unsigned child1 = m_Aiger->ands[i].rhs0;
		unsigned child2 = m_Aiger->ands[i].rhs1;

		child1 = child1 & 0xFFFFFFFE;	
		child2 = child2 & 0xFFFFFFFE;	

		if(child1 == child || child2 == child)
			return true;
	}

	return false;

}




/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     SETTER METHODS
####*
############################################################## 
##############################################################*/
void AIG::setPrimSource(std::string source){
	s_SourcePrim = source;
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     FILE METHODS
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  importAIG 
 *    Imports an AIG given a file
 *
 *    @PARAMS: 
 *      * fileName: Name of file to read AIG from
 ********************************************************/
void AIG::importAIG(std::string fileName){
	printf("[AIG]--Reading in AIG information for %s\n", fileName.c_str());

	//Check to see if AIG is empty. If empty, clear it
	if(getSize() != 0){
		printf("AIG is not empty. Overwriting graph...\n");
		clearAIG();
	}

	FILE* file;
	file = fopen(fileName.c_str(), "r");

	const char* msg;
	msg = aiger_read_from_file(m_Aiger, file);
	if(msg != NULL)
		printf("!! ERROR READING AIGER FILE: %s\n", msg);

	printf("Import Complete***********\n\n");
}







/*******************************************************
 *  exportAIG 
 *    Exports an AIG to a given file
 *
 *    @PARAMS: 
 *      * fileName: Name of file to write AIG to
 ********************************************************/
void AIG::writeAiger(std::string filename, bool isBinary){
	printf("[AIG]--Writing AIG to file...AIGER Format\n");
	enum aiger_mode mode;
	if(isBinary)
		mode = aiger_binary_mode;
	else
		mode = aiger_ascii_mode;

	//File
	FILE* file;
	file= fopen(filename.c_str(), "w");

	//Return code is zero if error
	int returnCode = aiger_write_to_file(m_Aiger, mode, file);
	if(returnCode == 0)
		printf("Error with writing to file...\n");
	else
		printf("Write success:\t%s\n", filename.c_str());
}









/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *    ALGORITHM METHODS 
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  convertGraph2AIG 
 *    Converts a given graph to an AIG
 *
 *    @PARAMS: 
 *      * ckt:    Graph to convert to AIG
 ********************************************************/
void AIG::convertGraph2AIG(Graph* ckt, bool sub){
	printf("[AIG]--Converting Graph to AIG\n");

	//Overwrite AIG if one exists
	if(getSize() != 0){
		printf("AIG is not empty. Overwriting AIG...\n");
		clearAIG();
	}

	//Primitive AIG gates
	std::string xorAIGFile_c = s_SourcePrim + "xor";
	std::string orAIGFile_c = s_SourcePrim + "or";
	std::string norAIGFile_c = s_SourcePrim + "nor";
	std::string nandAIGFile_c = s_SourcePrim + "nand";
	std::string andAIGFile_c = s_SourcePrim + "and";
	std::string aoiAIGFile_c = s_SourcePrim + "aoi";
	std::string oaiAIGFile_c = s_SourcePrim + "oai";
	std::string mxAIGFile_c = s_SourcePrim + "mux";
	std::string addAIGFile_c = s_SourcePrim + "add";

	std::map<int, Vertex<std::string>*>::iterator it;
	std::vector<Vertex<std::string>*> gNodeOutput;
	std::vector<int> toBeDeleted;
	std::vector<int> flipflops;




	//Go through each node in the circuit and covert it to its
	//  AND/INV representative
	int last = ckt->getLast()+1;
	for(it = ckt->begin(); it != ckt->end(); it++){
		std::string circuitType;

		if(it->first > last)
			break;

		//Read in the necessary primitive type
		//printf("CONVERTING V%d to AIG PRIM\n", it->first);
		//printf("GATE: %s\n", it->second->getType().c_str());
		std::string gateType = it->second->getType();
		if(gateType.find("XORCY") != std::string::npos)
			circuitType = xorAIGFile_c + "cy";
		else if(gateType.find("XOR") != std::string::npos)
			circuitType = xorAIGFile_c;
		else if(gateType.find("NAND") != std::string::npos)
			circuitType = nandAIGFile_c;
		else if(gateType.find("AND") != std::string::npos ||
				gateType.find("IN") != std::string::npos){
			if(it->second->getIVSize() > 2)
				circuitType = andAIGFile_c;
			else
				continue;
		}
		else if(gateType.find("VCC") != std::string::npos ||
				gateType.find("GND") != std::string::npos){
			ckt->addConstant(it->first);
			continue;
		}
		else if(gateType.find("NOR") != std::string::npos)
			circuitType = norAIGFile_c;
		else if(gateType.find("OR") != std::string::npos)
			circuitType = orAIGFile_c;
		else if(gateType.find("FF") != std::string::npos || 
				gateType.find("FD") != std::string::npos ||
				gateType.find("LAT") != std::string::npos){
			if(!sub){

			handleFF(it->first, ckt);	
			//flipflops.push_back(it->first);
			toBeDeleted.push_back(it->first);
			}
			continue;
		}
		else if(gateType.find("MUXCY") != std::string::npos)
			circuitType = s_SourcePrim + "muxcy";
		else if(gateType.find("MUXF") != std::string::npos)
			circuitType = s_SourcePrim + "muxf";
		else if(gateType.find("AOI") != std::string::npos)
			circuitType = aoiAIGFile_c;
		else if(gateType.find("OAI") != std::string::npos)
			circuitType = oaiAIGFile_c;
		else if(gateType.find("MX") != std::string::npos)
			circuitType = mxAIGFile_c;
		else if (gateType.find("LUT") != std::string::npos){
			printf("LUT\n");
			//handleFF(it->first, ckt);	
			//toBeDeleted.push_back(it->first);
			continue;
		}
		else if (gateType.find("RAM") != std::string::npos
		 || gateType.find("DSP") != std::string::npos){
			handleFF(it->first, ckt);	
			//toBeDeleted.push_back(it->first);
			continue;
		}
		else if(gateType.find("ADD") != std::string::npos)
			circuitType = addAIGFile_c;
		else if(gateType.find("TBUF") != std::string::npos)
			exit(1);
		else if(gateType.find("BUF") != std::string::npos){
			toBeDeleted.push_back(it->first);
			std::vector<Vertex<std::string>*> in;
			std::vector<Vertex<std::string>*> out;
			it->second->getOutput(out);
			it->second->getInput(in);

			std::string outputPortName = in[0]->removeOutputValue(it->first);

			for(unsigned int i = 0; i < out.size(); i++){
				int index = out[i]->removeInputValue(it->first);
				out[i]->addInput(in[0]);

				std::vector<std::string> outPorts;
				out[i]->getInPorts(outPorts);
				out[i]->addInPort(outPorts[index]);
				out[i]->removeInPortValue(index);

				in[0]->addOutput(out[i], outputPortName);
			}

			continue;
		}
		else if(gateType.find("TINV") != std::string::npos)
			exit(1);
		else{
			printf("Unknown Gate Type\tI%sI\nPress y to resume, n to quit\n", gateType.c_str());

			char inputchar;
			std::cin>>inputchar;
			if(inputchar == 'y')
				continue;
			else
				exit(1);
		}


		//printf("CIRCUIT PRIM: %s\n", circuitType.c_str());
		int gateSizeNum = it->second->getIVSize();
		char gateSize = '0' + gateSizeNum;

		circuitType = circuitType + gateSize + ".g";
		//printf("CIRCUIT PRIM: %s\n", circuitType.c_str());

		Graph* primCkt = new Graph(circuitType);
		primCkt->importPrimitive(circuitType, ckt->getLast() +1);

		//Substitute the node with the primitive type
		ckt->substitute(it->first, primCkt);
		toBeDeleted.push_back(it->first);

		//printf("LAST INDEX: %d\n\n", aindex);
		//printf("VERTEXID: %d\n", it->first);
		//if(ckt->getLast() > 40805){
		//	ckt->printg();
		//	exit(1);
		//}	
		//ckt->printg();
		//printf("\n\n");
	}

	//printf("CONVERTED CIRCUIT\n");
	//ckt->printg();
	//printf("\n\n");



	//Delete substituted nodes from the graph
	for(unsigned int i = 0; i < toBeDeleted.size(); i++)
		ckt->removeVertex(toBeDeleted[i]);







	/******************************************************
	 ********        PERFORM AIG CALCULATION      **********
	 *******************************************************/

	//Set Levels
	//printf("aig size:%d\n", ckt->getNumVertex());
	ckt->resetLevels();
	if(sub)
		return;
	//printf("MAX LEVEL: %d\n", ckt->getMaxLevel());
	//for(unsigned int i = 0; i < m_Outputs.size(); i++){
	//				printf("%d ", m_Outputs[i]);
	//}
	//ckt->print();


	//CONSTANT 0 AND 1
	printf(" * Creating AIG Datastructure\n");

	//Given an ID from graph, find the representing AIG node ID

	//Order the vertices by level
	std::map<int, std::vector<Vertex<std::string>*> > vLevel;
	for(it = ckt->begin(); it != ckt->end(); it++){
		//printf("ID:\t%dGate type:\t%s\n", it->first, it->second->getType().c_str());
		vLevel[it->second->getLevel()].push_back(it->second);
	}
	//exit(1);


	//Create PI for input LEVEL 0
	for(unsigned int i = 0; i < vLevel[0].size(); i++){
		if(vLevel[0][i]->getType() == "VCC")
			m_GateMap[vLevel[0][i]->getVertexID()] = 1;
		else if(vLevel[0][i]->getType() == "GND")
			m_GateMap[vLevel[0][i]->getVertexID()] = 0;
		else{
			unsigned int input= create_input();
			m_GateMap[vLevel[0][i]->getVertexID()] = input; 
			//printf("INPUT: %u\n", input);
		}
	}

	//printf("vLevel size: %d\n", vLevel.size());
	unsigned int vlevelSize = vLevel.size();
	unsigned int maxLevel = ckt->getMaxLevel();
	//Create the rest of the AIG Graph LEVEL 1 and UP
	for(unsigned int i = 1; i <= maxLevel; i++){
		//printf("\n\n\n\n\n\n\n\nLEVEL %d, SIZE: %d\n", i, vLevel[i].size());
		if(vLevel.size() != vlevelSize){
			printf("Size mismatch\n");
			exit(1);
		}
		for(unsigned int j = 0; j < vLevel[i].size(); j++){
			//printf("VERTEX ID: %d\n", vLevel[i][j]->getVertexID());
			if(vLevel[i][j]->getType().find("AND") != std::string::npos){
				//printf("GATE: AND\n");
				std::vector<Vertex<std::string>*> in;
				vLevel[i][j]->getInput(in);
				//printf("VIN: %d %d\n", in[0]->getVertexID(), in[1]->getVertexID());

				unsigned output = create_and2(m_GateMap[in[0]->getVertexID()], m_GateMap[in[1]->getVertexID()]);
				//printf("AIG: %d- %d %d\n", output, m_GateMap[in[0]->getVertexID()], m_GateMap[in[1]->getVertexID()]);
				int vertexID = vLevel[i][j]->getVertexID();
				m_GateMap[vertexID] = output;
			}
			else if(vLevel[i][j]->getType().find("INV") != std::string::npos){
				//printf("GATE: INVERTER\n");
				std::vector<Vertex<std::string>*> in;
				vLevel[i][j]->getInput(in);
				//printf("VIN: %d\n", in[0]->getVertexID());
				if(m_GateMap[in[0]->getVertexID()] == 0)
					m_GateMap[vLevel[i][j]->getVertexID()] = 1;
				else if(m_GateMap[in[0]->getVertexID()] == 1)
					m_GateMap[vLevel[i][j]->getVertexID()] = 0;
				else{
					if(m_GateMap[in[0]->getVertexID()]%2 == 0)
						m_GateMap[vLevel[i][j]->getVertexID()] = m_GateMap[in[0]->getVertexID()] + 1;
					else
						m_GateMap[vLevel[i][j]->getVertexID()] = m_GateMap[in[0]->getVertexID()] - 1;

				}
				if(vLevel[i][j]->getOVSize() == 0 ){

					//printf("INTYPE: %s\tINID:%d\tAIGID:%d\n", in[0]->getType().c_str(), in[0]->getVertexID(), m_GateMap[in[0]->getVertexID()]);
					m_Outputs.push_back(m_GateMap[in[0]->getVertexID()]);
					aiger_add_output(m_Aiger, m_GateMap[vLevel[i][j]->getVertexID()], 0);
				}

				continue;
			}
			else
			{
				printf("Unknown Gate type:\t%s\n", vLevel[i][j]->getType().c_str());
				exit(1);
			}

			//CHeck to see if node has an output
			if(vLevel[i][j]->getOVSize() == 0){
				//printf("NO OUTPUT\n");
				m_Outputs.push_back(m_GateMap[vLevel[i][j]->getVertexID()]);
				aiger_add_output(m_Aiger, m_GateMap[vLevel[i][j]->getVertexID()], 0);
			}
		}
	}
	//ckt->printg();
	//exit(1);


	//printf("Imported AIG Graph\n");
	//print();

	//printf("OUTPUTS\n");
	//for(unsigned int i = 0; i < m_Outputs.size(); i++){
	//	printf("%d ", m_Outputs[i]);
	//}
	//printf("\n");

	printf("[AIG]--Checking AIG...");
	const char* msg;
	msg = aiger_check(m_Aiger);
	if(msg != NULL){
		printf("FAIL!!\n%s\n", msg);
	}
	else
		printf("SUCCESS!!\n");
	print();
	printf("[AIG]--Conversion Complete\n");

	std::string cktname = ckt->getName();
	ckt->getName().rfind("/");
	int index = cktname.rfind("/")+1;
	cktname = cktname.substr(index, cktname.length()-index-1);
	printf("FILE NAME: %s\n", cktname.c_str());
	writeAiger("aiger/" + cktname + "aag", true);
}





/*******************************************************
 *  handleFF
 *    Removes FF and treates IO as PI and PO of circuit
 *
 *    @PARAMS: 
 *      * node: FF node in Graph
 ********************************************************/
void AIG::handleFF(int node, Graph* ckt){
	//printf("Removing FF...\nInputs to FF will go to PO. Gates the FF goes into will be switched for PI\n");
	std::vector<Vertex<std::string>*> output;
	std::vector<Vertex<std::string>*> input;


	//Make the output's become PI
	Vertex<std::string>* in;
	std::stringstream ss;
	in = ckt->addVertex(ckt->getLast()+1, "IN");
	ss<< "FF" << ckt->getNumInputs();
	ckt->addInput(ss.str(), in->getVertexID());
	ckt->getVertex(node)->getOutput(output);
	ckt->getVertex(node)->getInput(input);
	//printf("INPUT SIZE: %d\n", (int) input.size());
	//printf("OUTPUTSIZE: %d\n", (int) output.size());
	//printf("\nReplacing PI for those gates connected to the FF's output\n"); 
	for(unsigned i = 0; i < output.size(); i++){

		//Add the output to primary input
		//printf("- adding OUT%d to PI OUT\n", output[i]->getVertexID());

		if(output[i]->getVertexID() != node)
			in->addOutput(output[i], "O");
		//else
		//printf("Output is itself...skip\n");


		//Remove FF from input of the output node
		//printf("- removing FF from OUT%d IN\n", output[i]->getVertexID());
		int index = output[i]->removeInputValue(node);

		//printf("- adding PI to OUT%d IN\n", output[i]->getVertexID());
		output[i]->addInput(in);


		std::vector<std::string> outPorts;
		output[i]->getInPorts(outPorts);
		//printf("Adding port for Input %s\n", outPorts[index].c_str());
		output[i]->addInPort(outPorts[index]);
		//printf("Removing old input port index\n\n");
		output[i]->removeInPortValue(index);
	}



	for(unsigned i = 0; i < input.size(); i++){
		input[i]->removeOutputValue(node);
	}
}



/*******************************************************
 *  create_and2
 *    Creates an AND node in the AIG
 *
 *    @PARAMS: 
 *      * e1: AIG Node of the first input
 *      * e2: AIG Node of the second input
 *
 *    @RETURN: AND node of AIG that was created
 ********************************************************/
unsigned AIG::create_and2(unsigned e1, unsigned e2){
	m_Last+=2;
	aiger_add_and(m_Aiger, m_Last, e1, e2); 

	return m_Last;
}




/*******************************************************
 *  create_input 
 *    Creates a new input node in the AIG
 *
 *    @RETURN: INPUT node of AIG that was created
 ********************************************************/
unsigned AIG::create_input(){
	//Create an empty vertex
	m_Last+=2;
	aiger_add_input(m_Aiger, m_Last, 0); 

	//Return location of created input
	return m_Last;
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     PRINT METHODS
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  printHash
 *    Prints hash table used by the AIG
 ********************************************************/
void AIG::printHash(){
	std::map<std::vector<unsigned>, unsigned>::iterator it;
	printf("Printing Hash Table\tSIZE: %d\n", (int) m_HashTable.size());
	for(it = m_HashTable.begin(); it!= m_HashTable.end(); it++)
		printf("KEY: %d %d\tVAL: %d\n", it->first[0], it->first[1], it->second);
}



/*******************************************************
 *  print
 *    Prints contents of the AIG 
 ********************************************************/
void AIG::print(){
	printf("[AIG] Data structure\n");
	printf("\nInputs:\n");
	for(unsigned int i = 0; i < m_Aiger->num_inputs; i++){
		printf("%u ", m_Aiger->inputs[i].lit);	
	}
	printf("\nOutputs:\n");
	for(unsigned int i = 0; i < m_Aiger->num_outputs; i++){
		printf("%u ", m_Aiger->outputs[i].lit);	
	}
	printf("\nAnd Gates:\n");
	for(unsigned int i = 0; i < m_Aiger->num_ands; i++)
		printf("%u - %u %u\n", m_Aiger->ands[i].lhs, m_Aiger->ands[i].rhs0, m_Aiger->ands[i].rhs1);	
}



/*******************************************************
 *  printMap
 *    Prints circuit mapping to AIG
 ********************************************************/
void AIG::printMap(){
	printf("\n\nPrinting Circuit mapping to AIG\n");
	std::map<int,unsigned>::iterator it;
	for(it = m_GateMap.begin(); it != m_GateMap.end(); it++){
		printf("AIG NODE: %d\tCKT NODE: %d\n", it->second, it->first);
	}
}







