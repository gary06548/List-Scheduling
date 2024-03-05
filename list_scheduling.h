#ifndef LIST_SCHEDULING_H
#define LIST_SCHEDULING_H

#include <queue>
#include <vector>
#include <algorithm>
#include "data_structure.h"
#include "graph.h"
#define IntMax 2147483647

using namespace std;
using namespace data_structure;

//========= execute list scheduling by graph ==========
class List_Scheduling 
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
	
	// calculate ALAP time of each node
	void cal_alap(vector <Nodeptr> &list_child, vector <Nodeptr> &list_parent, string limit_latency)
	{
		queue <int> parent_index; // store all elements in each column of list_parent, to traverse in next level
		parent_index.push(list_child.size()-1); // push LNOP node into queue
		list_parent[ list_child.size()-1 ]->alap_time = StoI(limit_latency); // initialize LNOP's ALAP time
		
		while(!parent_index.empty())
		{
			Nodeptr temp_parent = list_parent[ parent_index.front() ]->parent;
			
			queue <int> check_index; // store all nodes traversed in this level, to check these nodes' flag in list_child
			
			while(temp_parent != NULL)
			{
				parent_index.push( temp_parent->tail );
				check_index.push( temp_parent->tail );
				temp_parent->alap_state = true; // have been traversed
				temp_parent = temp_parent->parent;
			}
			
			while(!check_index.empty())
			{
				Nodeptr temp_child = list_child[ check_index.front() ]->child;
				check_index.pop();
				int min_time = IntMax;
				
				while(temp_child != NULL)
				{
					int diff = list_parent[ temp_child->head ]->alap_time - temp_child->tail_latency;
						
					if(min_time > diff)
					{
						min_time = diff;
					}
					
					if(temp_child->alap_state == false)
					{
						break;
					}
					else if(temp_child->child == NULL) // final child
					{
						list_parent[ temp_child->tail ]->alap_time = min_time;
					}
					
					temp_child = temp_child->child;
				}
				
			}
			
			parent_index.pop();
		}
	}
	
	// return true if all nodes are scheduled, otherwise return false
	bool sche_all(bool list_flag[], unsigned int size)
	{
		for(unsigned int i=0; i<size; i++)
		{
			if( i == size-1 ) return true; // skip LNOP
			else if(!list_flag[i]) return false;
		}
	}
	
	// initialize list_flag
	void list_flag_init(bool list_flag[], unsigned int size)
	{
		for(unsigned int i=0; i<size; i++)
			list_flag[i] = false;
	}
	
	// remove each node's data, include: start_time, sche_state
	void remove_node(Nodeptr nptr)
	{
		if(nptr->head_type == "i" || nptr->head_type == "UNOP") nptr->start_time = 0;
		else nptr->start_time = IntMax;
		
		nptr->sche_state = false;
	}

	// refresh graph to schedule again
	void refresh_graph(vector <Nodeptr> &list_child, vector <Nodeptr> &list_parent)
	{
		for(unsigned int i=0; i<list_child.size(); i++)
		{
			Nodeptr temp_child = list_child[i];
			
			while(temp_child != NULL)
			{
				remove_node(temp_child);
				temp_child = temp_child->child;
			}
		}
		
		for(unsigned int i=0; i<list_parent.size(); i++)
		{
			remove_node(list_parent[i]);
		}
	}
	
	// copy resource data into resource_copy and update resource array
	void array_process(int resource[2])
	{
		// update data in resource
		if(resource[0] > 50) resource[0] = (resource[0] / 5)*4 + 1;
		else resource[0] = (resource[0] / 2) + 1;
		
		if(resource[1] > 50) resource[1] = (resource[1] / 5)*4 + 1;
		else resource[1] = (resource[1] / 2) + 1;
	}

	// compare slack, return true if node_1's slack is smaller then node_2, otherwise return false 
	static bool compare_slack(Slack_and_ID n1, Slack_and_ID n2)
	{
		return n1.slack < n2.slack;
	}
	
	// copy the data in list_parent, include its start_time, head_type and head_latency
	vector <Nodeptr> graph_copy(vector <Nodeptr> list_parent)
	{
		Graph graph;
		vector <Nodeptr> best_ans;
		vector <string> NodeType;
		NodeType.push_back("null");
		
		for(unsigned int i=0; i<list_parent.size(); i++)
		{
			Nodeptr nptr = list_parent[i];
			
			best_ans.push_back( graph.NewNode(nptr->tail, nptr->head, NodeType) );
			
			best_ans.back()->start_time = nptr->start_time;
			best_ans.back()->head_type = nptr->head_type;
			best_ans.back()->head_latency = nptr->head_latency;
		}
		
		return best_ans;
	}

	// schedual all nodes in graph
	bool list_scheduling(vector <Nodeptr> &list_child, vector <Nodeptr> &list_parent, string limit_latency, int resource[2], bool &exe)   
	{
		if(!exe) // only first scheduling needs calculate ALAP time 
		{
			cal_alap(list_child, list_parent, limit_latency);
			exe = true;
		}
		
		if(list_parent[0]->alap_time < 0) return false; // can't be scheduled
		
		vector <int> next_level_index; // store all indexes to traverse list_child
		
		bool list_parent_flag[ list_parent.size() ]; // record all indexes which have been scheduled
		list_flag_init(list_parent_flag, list_parent.size()); 
		bool list_child_flag[ list_child.size() ]; // record all indexes which have been scheduled
		list_flag_init(list_child_flag, list_child.size()); 
		
		list_parent_flag[0] = true;
		next_level_index.push_back(0); // push UNOP node into queue
		list_parent[0]->start_time = 0; // initialize UNOP's starting time
		bool flag = false;
		int level = 0;
		vector <int> mul_level; // store mul_num in level, level-1, level-2
		mul_level.push_back(0); // add two zero value into this vector
		mul_level.push_back(0);
		
		while( !sche_all(list_parent_flag, list_parent.size()) ) 
		{
			vector <int> unsche_index; // store all nodes whose schedule state are ready
			vector <int> child_index(next_level_index); // store all elements in each row of list_child, to traverse in next level
			vector <Slack_and_ID> add_sk, mul_sk; // store nodes which can be scheduled but slack != 0
			int add_num = 0, mul_num = 0; // number of resource
			
			// traverse nodes from list_child and record that these nodes have been traversed
			while(!child_index.empty())
			{
				if( !list_child_flag[ child_index.back() ] ) // this node of list_child hasn't been traversed
				{
					list_child_flag[ child_index.back() ] = true;
					
					Nodeptr temp_child = list_child[ child_index.back() ]->child;
					
					while(temp_child != NULL)
					{
						next_level_index.push_back( temp_child->head );
						temp_child->sche_state = true; // have been traversed
						temp_child = temp_child->child;
					}
				}
				
				child_index.pop_back();
			}	
			
			// check from the list_parent if any of the node's predecessors have all been traversed
			for(unsigned int i=0; i<list_parent.size(); i++)
			{
				if(!list_parent_flag[i])
				{
					Nodeptr temp_parent = list_parent[i]->parent;
					
					while(temp_parent != NULL)
					{
						int cur_id = temp_parent->tail; // the value of current ptr's tail
						int exe_time = level - list_parent[cur_id]->start_time; // time from scheduled to current level
						bool complete = ( exe_time >= list_parent[cur_id]->head_latency );         
						
						if(temp_parent->sche_state == false || !complete)
						{
							break;
						}
						else if(temp_parent->parent == NULL) // final child
						{
							unsche_index.push_back(temp_parent->head);// put this node into "ready state" vector
						}
						
						temp_parent = temp_parent->parent;
					}
				}
			}
			
			// compute each node's slack, if slack = 0 then schedule this node
			for(unsigned int i=0; i<unsche_index.size(); i++)
			{
				string node_type = list_parent[ unsche_index[i] ]->head_type;
				int slack = list_parent[ unsche_index[i] ]->alap_time - level;
				
				if(slack == 0)
				{
					if(node_type == "+") add_num++;
					else if(node_type == "*") mul_num++;
					
					list_parent[ unsche_index[i] ]->start_time = level;
					list_parent_flag[ unsche_index[i] ] = true;
				}
				else
				{
					if(node_type != "+" && node_type != "*") continue;
					
					Slack_and_ID temp_node;
					temp_node.id = list_parent[ unsche_index[i] ]->head;
					temp_node.slack = slack;
					
					if(node_type == "+") add_sk.push_back(temp_node);
					else if(node_type == "*") mul_sk.push_back(temp_node);
				}
			}
			
			// update the number of needed resource
			if(add_num > resource[0]) 
			{
				resource[0] = add_num;
			}
			else // schedule additional Sk whose slack != 0 but ready to schedule
			{
				int avali_res = resource[0] - add_num; // the number of avaliable resource to use
				
				sort(add_sk.begin(), add_sk.end(), compare_slack); // small to big
				
				for(unsigned i=0; i<add_sk.size(); i++)
				{
					if(i < avali_res) // schedule avali_res nodes whose slack are minimum in add_sk
					{
						list_parent[ add_sk[i].id ]->start_time = level;
						list_parent_flag[ add_sk[i].id ] = true;
					}
					else
					{
						break;
					}
				}
			}
			
			mul_level.push_back(mul_num);
			
			/* updating the number of multipliers requires recording the total number used in the 
			current level and the previous two levels, and finding the maximum value during iteration.
			Because multiplier need 3 cycles to complete its task.*/
			if(mul_level.size() == 3)
			{
				int mul_sum = mul_level[0] + mul_level[1] + mul_level[2];
				
				if(mul_sum > resource[1])
				{
					resource[1] = mul_sum;
				} 
				else // there are additional multipliers available
				{
					int avali_res = resource[1] - mul_sum; // the number of avaliable resource to use
					
					sort(mul_sk.begin(), mul_sk.end(), compare_slack); // small to big
					
					for(unsigned i=0; i<mul_sk.size(); i++)
					{
						if(i < avali_res) // schedule avali_res nodes whose slack are minimum in mul_sk
						{
							list_parent[ mul_sk[i].id ]->start_time = level;
							list_parent_flag[ mul_sk[i].id ] = true;
							mul_level[2]++; // update the number of multiplier used in current level
						}
						else
						{
							break;
						}
					}
				}
				
				mul_level.erase(mul_level.begin());
			}
			
			next_level_index = unsche_index;
			
			if(!flag) flag = true;
			else level += 1;
		}
		
		return true;
	}

public:	
	// do list scheduling ten times to find best result
	void best_answer(vector <Nodeptr> list_child, vector <Nodeptr> list_parent,
	 				 vector <Nodeptr> &best_ans, int best_resource[2], string limit_latency)
	{
		int resource[2] = {1, 1}; // resource[0] is adder, while resource[1] is multiplier
		bool exe_flag = false; // determine whether list scheduling has been executed or not
		int count = 0; 
		
		while(true)
		{	
			array_process(resource);
			
			if( !list_scheduling(list_child, list_parent, limit_latency, resource, exe_flag) )
				cout<<"Scheduling failed!!!"<<endl;
			
			if(resource[0] + resource[1] < best_resource[0] + best_resource[1])
			{
				best_resource[0] = resource[0];
				best_resource[1] = resource[1];
				
				best_ans = graph_copy(list_parent);
			}
			
			if(count++ > 100) break;
			
			refresh_graph(list_child, list_parent);
		}
	}
};

#endif




