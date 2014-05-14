/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @      vertex.hpp
  @      
  @      @AUTHOR:        Kevin Zeng
  @      Copyright 2012 – 2013 Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#ifndef VERTEX_GUARD
#define VERTEX_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

template<class T>
class Vertex
{
	private:
		T m_Type;
		int m_ID;    //Unique ID assigned
		int m_Level;
		unsigned long m_LUT;

		std::vector<Vertex<T>*> m_Input;	//Set of vertices connected into the current vertex
		std::vector<std::string> m_InPorts;

		std::map<std::string, std::vector<Vertex<T>*> > m_Output;
	public:

		Vertex(){
			m_Input.clear();
			m_Output.clear();
			m_Level = -1;
		}


		Vertex(int vID, T type){
			m_Type = type;		
			m_ID = vID;
			m_Input.clear();
			m_Output.clear();
			m_Level = -1;
		}

		Vertex(int vID){
			m_Type = "";		
			m_ID = vID;
			m_Input.clear();
			m_Output.clear();
			m_Level = -1;
		}


		Vertex(const Vertex& rhs){
			printf("VERTEX CCONSTRUCTOR\n");
			m_Type = rhs.m_Type;
			m_ID = rhs.m_ID;

			for(unsigned int i = 0; i < rhs.m_Input.size(); i++)
				m_Input.push_back(rhs.m_Input[i]);
			for(unsigned int i = 0; i < rhs.m_Output.size(); i++)
				m_Output.push_back(rhs.m_Output[i]);
		}

		void getInPorts(std::vector<std::string>& portlist){
			portlist = m_InPorts;
		}

		void setLevel(int level){
			m_Level = level;
		}

		void setLUT(std::string func){
			m_LUT = 0;
			unsigned long base = 1;
			//Convert function string into an integer
			for(int i = func.size()-1; i >= 0; i--){
				if( func[i] <= '9'){
					m_LUT += (func[i]-48)*base;
				}
				else{
					m_LUT +=(func[i]-55)*base;
				}
				base *= 16;
			}
		}

		unsigned long getLUT(){
			return m_LUT;
		}

		int getLevel(){
			return m_Level;
		}

		int getNumInputs(){
			return m_Input.size();
		}


		void addInput(Vertex<T>* input){
			m_Input.push_back(input);
		}

		void addInPort(std::string port){
			m_InPorts.push_back(port);
		}

		void addOutput(Vertex<T>* output, std::string port){
			m_Output[port].push_back(output);
		}

		std::string removeOutputValue(int outputVal){
			typename std::map<std::string, std::vector<Vertex<T>*> >::iterator it;
			for(it = m_Output.begin(); it != m_Output.end(); it++){
				for(unsigned int i = 0; i < it->second.size(); i++){
					if(it->second[i]->getVertexID() == outputVal){
						it->second.erase(it->second.begin() + i);
						std::string port = it->first;
						if(it->second.size() == 0)
							m_Output.erase(it->first);
						return port;
					}
				}
			}
			printf("Remove output value %d Not found in output list\n", outputVal);
			exit(1);
			return "";
		}

		int removeInputValue(int inputVal){
			for(unsigned int i = 0; i < m_Input.size(); i++){
				if(m_Input[i]->getVertexID() == inputVal){
					m_Input.erase(m_Input.begin() + i);
					return i;
				}
			}

			printf("Remove input value %d Not found in input list\n", inputVal);
			exit(1);

			return -1;
		}

		void removeInPortValue(int index){
			m_InPorts.erase(m_InPorts.begin() + index);
		}

		void getOutput(std::vector<Vertex<T>*> &v){
			typename std::map<std::string, std::vector<Vertex<T>*> >::iterator it;
			//if(getVertexID() == 3295){
			//printf("Number of Outports %d\n", m_Output.size());
			//printf("Gate %s\n", m_Type.c_str());
			//}
			for(it = m_Output.begin(); it != m_Output.end(); it++){
				//if(getVertexID() == 3295){
				//printf("here");
				//printf("PORT: %s\tNumber of out %d\n", it->first.c_str(), it->second.size());
				//}
				for(unsigned int i = 0; i < it->second.size(); i++){
					//if(getVertexID() == 3295)
					//	printf("PUSH OUTPUT %d\n", it->second[i]->getVertexID());
					v.push_back(it->second[i]);
				}
			}
		}

		void getInput(std::vector<Vertex<T>*> &v){
			v = m_Input;
		}

		void getPortOutput(std::string port, std::vector<Vertex<T>*>&v){
			v = m_Output[port];
		}

		void getInputPorts(std::vector<std::string>&v){
			v = m_InPorts;
		}


		void getOutputPorts(std::vector<std::string> &v){
			typename std::map<std::string, std::vector<Vertex<T>*> >::iterator it;
			for(it = m_Output.begin(); it != m_Output.end(); it++){
				v.push_back(it->first);
			}
		}

		std::string  getOutputPortName(int id){
			typename std::map<std::string, std::vector<Vertex<T>*> >::iterator it;
			for(it = m_Output.begin(); it != m_Output.end(); it++){
				for(unsigned int i = 0; i < it->second.size(); i++){
					if(it->second[i]->getVertexID() == id)
						return it->first; 
				}
			}

			printf("Output ID %d not found\n", id);
			exit(1);
		}

		void getOutputPortID(std::string port, std::vector<Vertex<T>*>& ids){
			if(m_Output.find(port) == m_Output.end()){
				printf("NO OUTPUT PORT FOUND WITH LABEL: %s\n", port.c_str());
				exit(1);
			}

			ids = m_Output[port];
		}

		std::string  getInputPortName(int id){
			for(unsigned int index = 0; index < m_Input.size(); index++){
				if(m_Input[index]->getVertexID() == id)
					return m_InPorts[index];
			}

			printf("Input ID %d not found\n", id);
			exit(1);

		}
		
		int getInputPortID(std::string port){
			for(unsigned int index = 0; index < m_InPorts.size(); index++){
				if(m_InPorts[index] == port)
					return m_Input[index]->getVertexID();
			}
		//	printf("[WARNING] -- Input name %s not found\n", port.c_str());
			return -1;


		}

		unsigned int getOVSize(){
			unsigned int size = 0;
			typename std::map<std::string, std::vector<Vertex<T>*> >::iterator it;
			for(it = m_Output.begin(); it != m_Output.end(); it++){
				size += it->second.size();
			}
			return size;
		}

		unsigned int getIVSize(){
			return m_Input.size();
		}

		int getVertexID(){
			return m_ID;
		}

		void setVertexID(int ID){
			m_ID = ID;
		}

		T getType(){
			return m_Type;
		}

		void setType(T type){
			m_Type = type;
		}

};

#endif












