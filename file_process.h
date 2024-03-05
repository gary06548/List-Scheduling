#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H

#include <fstream>
#include <vector>
#include "data_structure.h"

using namespace std;
using namespace data_structure;

//------- read input file and write output file -------
class File_Process 
{
private:
	// transfer sentence to substrings
	vector <string> sentence_to_substrings(string input) // Ex: save "Hello World" as "Hello" and "World"
	{
		vector <string> output;
		int start_pos = 0;
	
		for(int i=0; i<input.length(); i++) 
		{
			if(input[i] == ' ')
			{
				if(input[start_pos] != ' ' && start_pos != i) output.push_back( input.substr(start_pos, i-start_pos) );
				start_pos = i+1;
			}
			else if(i == input.length()-1) // final character -> save word and break the loop
			{
				output.push_back( input.substr(start_pos, i+1-start_pos) );
				break;
			}
		}
	
		return output;
	}

	// transfer string to integer number
	int StoI(string num)
	{
		int out = 0;
		
		for(unsigned int i=0; i<num.length(); i++)
			out = out*10 + (int)num[i] - 48;
		
		return out;
	}

public:
	// read input file
	void in_file(vector < vector <string> > &lvs, char *argv)
	{
		ifstream infile(argv, ios::in);
		string in;
		vector <string> substring_data;
	
		if(!infile) // if file not exist -> exit
		{
			cerr<<"File could not be opened"<<endl;
			exit(1);
		}
		else
		{
			while(!infile.eof()) // read this file until reading every line in this file
			{
				getline(infile,in);
				substring_data = sentence_to_substrings(in); // seprate tis line into words
				lvs.push_back(substring_data);
			}
		}
	}
	
	// write output file
	void out_file(vector <Nodeptr> list_parent, int resource[2], string limit_latency, char* argv)
	{
		ofstream outfile (argv, ios::out);
		outfile<<resource[0]<<endl<<resource[1]<<endl; // print the number of adder and multiplier
		
		int level = 0;
		int limit_time = StoI(limit_latency);
		
		//--------------------  print resource node at each time slot --------------------
		while(level <= limit_time)
		{
			bool print_flag = true; // determine first print or not
			
			for(unsigned int i=1; i<list_parent.size(); i++)
			{
				if(list_parent[i]->head_type == "+" || list_parent[i]->head_type == "*")
				{
					Nodeptr nptr = list_parent[i];
					
					if(level >= nptr->start_time && level <= nptr->start_time + nptr->head_latency - 1)
					{
						if(print_flag)
						{
							print_flag = false;
							outfile<<nptr->head;
						} 
						else outfile<<" "<<nptr->head;
					}
				}
			}
			
			if(level < limit_time - 1) outfile<<endl;
			level++;
		}
		
		return;
	}

};

#endif




