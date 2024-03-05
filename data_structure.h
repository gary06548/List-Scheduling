#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <iostream>

using namespace std;

//------- define data structure of graph node -------
namespace data_structure
{
	struct Node
	{
		int tail; // tail id
		int head; // head id
		int tail_latency;  
		int head_latency;  
		int start_time;
		int alap_time; // as late as possible
		bool alap_state; // flag that show this node is teaversed or not in ALAP Calculating
		bool sche_state; // flag that show this node is teaversed or not in Scheduling
		string tail_type; // adder, multiplier, input, output
		string head_type; // adder, multiplier, input, output
		struct Node *parent;
		struct Node *child;
	};
	
	struct Slack_and_ID
	{
		int id; // the ID of this node
		int slack;	// the slack of this node
	};
	
	typedef Node* Nodeptr; // name Node* as Nodeptr
};

#endif
