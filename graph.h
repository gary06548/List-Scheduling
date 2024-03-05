#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "data_structure.h"
#define IntMax 2147483647

using namespace std;
using namespace data_structure;

//------------------  create graph  ------------------
class Graph 
{
private:
	// transfer string to integer number
	int StoI(string num)
	{
		int out = 0;
		
		for(unsigned int i=0; i<num.length(); i++)
			out = out*10 + (int)num[i] - 48;
		
		return out;
	}
	
public:	
	// create a new node in graph
	Nodeptr NewNode(int Tail, int Head, vector <string> NodeType)
	{
		Nodeptr newnode = new Node;
	
		newnode->tail = Tail;
		newnode->head = Head;
		newnode->alap_time = 0;
		newnode->alap_state = false; // haven't been traversed
		newnode->sche_state = false; // haven't been traversed
		newnode->child = NULL;
		newnode->parent = NULL;
		
		if(Tail < NodeType.size()) newnode->tail_type = NodeType[Tail];
		else newnode->tail_type = "null";
		
		if(Head < NodeType.size()) newnode->head_type = NodeType[Head];
		else newnode->head_type = "null";
		
		if(newnode->tail_type == "+") newnode->tail_latency = 1;
		else if(newnode->tail_type == "*") newnode->tail_latency = 3;
		else newnode->tail_latency = 0;
		
		if(newnode->head_type == "+") newnode->head_latency = 1;
		else if(newnode->head_type == "*") newnode->head_latency = 3;
		else newnode->head_latency = 0;
		
		if(newnode->head_type == "i" || newnode->head_type == "UNOP") newnode->start_time = 0;
		else newnode->start_time = IntMax;
			
		return newnode;	
	}

	// create a graph 
	void create_graph(vector < vector <string> > input, vector <Nodeptr> &list_child, vector <Nodeptr> &list_parent)
	{
		vector <string> node_type;
		
		//------------ create nodes into child list ---------------
		// store each node's type first
		node_type.push_back("UNOP");
		
		for(unsigned int i=4; i<input.size(); i++)
		{
			node_type.push_back(input[i][1]);
		}
		
		node_type.push_back("LNOP");
		
		// create upper NOP node in two list
		list_parent.push_back( NewNode(0, 0, node_type) );
		list_child.push_back( NewNode(0, 0, node_type) );
			
		for(unsigned int i=4; i<input.size(); i++)
		{
			list_parent.push_back( NewNode(0, StoI(input[i][0]), node_type) );
			list_child.push_back( NewNode(StoI(input[i][0]), 0, node_type) );
			Nodeptr temp_child = list_child[i-3]; // i-4+'1', because first element is NOP
			
			for(unsigned int j=2; j<input[i].size(); j++)
			{
				temp_child->child = NewNode(StoI(input[i][0]), StoI(input[i][j]), node_type);
				temp_child = temp_child->child;
			}
		}
		
		// create lower NOP node in two list
		list_parent.push_back( NewNode(node_type.size()-1, node_type.size()-1, node_type) ); // push back LNOP
		list_child.push_back( NewNode(node_type.size()-1, node_type.size()-1, node_type) ); // push back LNOP
		
		// temporary NOP child pointer, use to create connection in NOP node 
		Nodeptr up_nop_child = list_child[0];
		
		for(unsigned int i=0; i<list_child.size(); i++)
		{
			if(list_child[i]->tail_type == "i")
			{
				up_nop_child->child = NewNode(0, list_child[i]->tail, node_type); // upper NOP
				up_nop_child = up_nop_child->child;
			}
			else if(list_child[i]->tail_type == "o")
			{
				list_child[i]->child = NewNode(list_child[i]->tail, node_type.size()-1, node_type); // lower NOP
			}
		}
		
		//------------ construct nodes' connection of list_parent ---------------
		for(int i=1; i<list_parent.size(); i++) // start from i = 1, because UNOP have no parent 
		{
			Nodeptr temp_parent = list_parent[i];
			
			for(int j=0; j<list_child.size(); j++)
			{
				Nodeptr temp_child = list_child[j]->child;
				
				while(temp_child != NULL)
				{
					if(temp_child->head == list_parent[i]->head)
					{
						temp_parent->parent = temp_child;
						temp_parent = temp_child;
						//break; // node maybe connect to same successor 2 times or more
					}
					
					temp_child = temp_child->child;
				}
			}
		}
	}

};

#endif




