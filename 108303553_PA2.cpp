#include <iostream>
#include <vector>
#include "file_process.h"
#include "data_structure.h"
#include "graph.h"
#include "list_scheduling.h"

using namespace std;
using namespace data_structure;

int main(int argc, char *argv[])
{
	//----------------------------  data declaration  ----------------------------
	vector < vector <string> > input; // use to store input data
	vector <Nodeptr> list_child, list_parent; // graph can be accessed by the two vectors
	int best_resource[2] = {IntMax/2, IntMax/2}; // index_0 is #adder, while index_1 is #multiplier
	vector <Nodeptr> best_ans; // the best result of list scheduling -> write to output file

	//-------  read iput file and store the data into a 2D string vector  --------
	File_Process file;
	file.in_file(input, argv[1]); // read input file
	
	//-----------------------  create graph by input data  -----------------------
	Graph graph;
	graph.create_graph(input, list_child, list_parent);
	
	//---------------------  use graph to do list scheduling  --------------------
	List_Scheduling list_sche;
	list_sche.best_answer(list_child, list_parent, best_ans, best_resource, input[1][2]);

	//----------------------------  write output file  ---------------------------
	file.out_file(best_ans, best_resource, input[1][2], argv[2]);
	
	return 0;
}



