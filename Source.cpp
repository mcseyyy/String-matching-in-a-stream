#include "Misc.h"
#include "Fingerprint.h"
#include "Stream_Match.h"
#include<iostream>
//using namespace std;

typedef unsigned long long ull;
typedef unsigned int       ui;
typedef unsigned char      uc;

/*
	This file will mainly be used to test the implementations of other classes
*/





int main()
{	
	std::vector<uc> pattern = { 'a', 'a', 'b', 'a', 'a', 'b' };
	
	//get the parameters 
	Stream_Match_Params<ull, uc> params(pattern);
	
	std::vector<uc> stream = {  'r', 'a','a','b','a','a', 'b', 'a', 'a', 'b', 'a', 'a', 'a', 'b', 'b', 'c', 'e', 'a', 'b', 't', 't' };

	Stream_Match<ull, uc> dict(params, pattern);
	for (ui i = 0; i < stream.size(); i++)
		if (dict.stream_element(stream[i]))
			std::cout << "Match ending at the current element: " <<i<< std::endl;


	



}

