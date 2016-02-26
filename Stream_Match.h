/*
	Implemntation of the O(log n) algorithm for string-matching in a stream by Dany Breslauer, Zvi Galil

	paper: Real-time streaming string-matching
	authors: Dany Breslauer, Zvi Galil
	link to paper (ACM): http://dl.acm.org/citation.cfm?id=2635814
	link to paper (Google Scholar): https://scholar.google.com/citations?view_op=view_citation&hl=en&user=i6ZjofkAAAAJ&citation_for_view=i6ZjofkAAAAJ:MXK_kJrjxJIC
*/

#pragma once
#include <vector>
#include <map>
#include <ctime>
#include "Misc.h"
#include "Fingerprint.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef unsigned int ui;

// claas for storing data about viable occurences (including arithmetic progressions)

template<typename T, typename StrT>
struct Viable_Occurence_Data{
	typedef Fingerprint<T, StrT> FP;
	ui stage,
	   prog_count=1,	//number of prefix_patterns in the progression	
	   period_length;	//length of the period of the pattern's prefix
	FP start_fp,		//fingerprint of the stream including the element before the vialbe occurence
	   end_fp,			//fingerprint of the stream up to the last element in the arithmetic progression
	   period_fp;		// fingerprint of the period
	Viable_Occurence_Data(ui _stage, FP _fp) : stage(_stage), start_fp(_fp),period_fp(_fp),end_fp(_fp){};
};

//class that generates the parameters for stream matching
template<typename T, typename StrT>
struct Stream_Match_Params{
	typedef Stream_Match_Params<T, StrT> SMP;
public:
	T p;
	std::vector<T> r_pow;

	//size of the pattern and alpha; alpha for Rabin Karp Fingerprints
	SMP(ui m, double alpha) {
		alpha = MIN(alpha, 3);				//threshold alpha
		p = prime_bigger_than(ui(pow(m, 2 + alpha))); //find prime number p in O(m^(2+alpha))
		srand(ui(time(NULL))); 
		r_pow = initialise_r<T>(rand() % (p/2+1), p); //generate random number for FPs and preprocess the r_pow vector
	}

	//just the size of the pattern
	SMP(ui m) : SMP(m, 1){};

	//size of the pattern and alpha
	SMP(const std::vector<StrT> &pattern) : SMP(pattern.size(), 1){};

	//pattern and alpha
	SMP(const std::vector<StrT> &pattern, double alpha) : SMP(pattern.size(), alpha){};
};

template<typename T,typename StrT>
class Stream_Match{
	typedef Fingerprint<T, StrT> FP;

	T p;						//prime number
	std::vector<T> *r_pow;		//preprocessed powers of r (r^0,r^1,...,r^p-2);     NOTE: (r^(p-1)) % p == 1
	ui m, log2_m;						//size of the pattern
	FP running_fp;						//fp of the entire stream
	std::multimap<ui, Viable_Occurence_Data<T, StrT>> viable_occurence;									//<next_position_to_be_checked,data_about_the_position_to_be_checked>
	std::vector<typename std::multimap<ui, Viable_Occurence_Data<T, StrT>>::iterator> stage_location;	//iterator to the node in the multimap coresponding to the given stages
	ui n = -1;
	std::vector<FP> prefix_fp;			//stores fingerprints of prefixes of length 2^i \forall i

	// removes a viable occurence from the multimap
	void remove_viable_occurence(){
		auto occurence = viable_occurence.begin();
		if (occurence->second.prog_count == 1){
			//if there is only one match at the given (stage, position in the stream)
			stage_location[occurence->second.stage - 1] = viable_occurence.end();
			viable_occurence.erase(occurence);
			return;
		}
		//else: if the matches are stored in an arithmetic progression
		Viable_Occurence_Data<T, StrT> data = occurence->second;
		data.prog_count--;									//decrease the number of elements in the arithmetic progression
		data.start_fp.concat_right(data.period_fp);			//update the start_fp to skip the period_length characters
		ui next_pos = occurence->first + data.period_length;//compute next position to be checked

		//update the key in the multimap (erase+insert)
		viable_occurence.erase(occurence);			
		auto node_it = viable_occurence.emplace(next_pos, data);
		stage_location[data.stage - 1] = node_it;			//upate location of the stage in the multimap
	}

	// inserts a viable occurence in the multimap given the next stage that should be checked and the fp up to the viable occurence
	void insert_viable_occurence(ui stage, FP start_fp){
		std::multimap<ui, Viable_Occurence_Data<T, StrT>>::iterator it = stage_location[stage];
		
		//compute the next position where a check needs to be made
		ui next_position = (stage + 1 == log2_m) ? n + m - pow2(stage) : n + pow2(stage);

		
		if (it == viable_occurence.end()){
			//no other viable occurences are at the same stage;
			auto node_it = viable_occurence.emplace(next_position, Viable_Occurence_Data<T, StrT>(stage + 1, start_fp));
			stage_location[stage] = node_it;
			return;
		}
		//else: if there are other viable occurences at the same stage
		auto data = it->second;
		data.prog_count++;
		if (data.prog_count == 2){
			//if there are exactly 2 viable occurences at the same stage (including the current one)
			//compute period_length and period_fp and store them
			data.period_length = next_position - it->first;
			data.end_fp = running_fp;
			data.period_fp = start_fp.extract_suffix(data.start_fp);
		}
		else{
			//if there are more than 2 viable occurences
			FP period_check_fp = running_fp;
			period_check_fp.extract_suffix(data.end_fp);
			if (next_position == it->first + data.period_length*(data.prog_count) &&
				period_check_fp == data.end_fp){
				//if the new viable occurence preservers the implied period just add it to the arithmetic progression
				data.end_fp = running_fp;
				data.prog_count++;
				viable_occurence.erase(it);
				auto node_it = viable_occurence.emplace(next_position, data);
				stage_location[data.stage - 1] = node_it;
				return;
			}
			else{
				// implied period lenght and period_fp are not preserved;
				// remove all the viable occurences in the progression (except last one) and add the new one)
				for (ui i = 1; i < data.prog_count; i++)
					data.start_fp.concat_right(data.period_fp);
				data.period_length = next_position - (it->first + data.period_length*(data.prog_count - 1));
				data.period_fp = start_fp.extract_suffix(data.start_fp);
				data.end_fp = running_fp;
				data.prog_count = 2;
				viable_occurence.erase(it);
				auto node_it = viable_occurence.emplace(next_position - data.period_length, data);
				stage_location[data.stage - 1] = node_it;
			}
		}

	}

public:
	

	/* Constructor: Stream_Match (_p, _r, pattern)
	Input arguments:
		- T _p:		prime number used for fingerprints
		- _r:		pointer to a vector<T> that contains the preprocessed numbers in Z_p = {r^0, r^1, r^2, .. r^(p-2)} mod p
		- pattern:	the pattern used for matching
	Note: - technically, the pattern should nto be stored but I did not have the time to preprocess the pattern as a stream; and as always, preprocessing does not matter
		  - the pattern will not be stored
	*/
	Stream_Match(const T _p, std::vector<T> *_r, const std::vector<StrT> &pattern): p(_p), r_pow(_r),running_fp(p,_r){
		
		m = pattern.size();
		log2_m = ui(ceil(log2(m)));
		
		prefix_fp=std::vector<FP>(log2_m+1, running_fp); //initialise the vector of fingerprint of prefixes
		stage_location = std::vector<typename std::multimap<ui, Viable_Occurence_Data<T, StrT>>::iterator>(log2_m + 1, viable_occurence.end());
		
		for (ui i = 0, i_fp = 0; i < m; i++){
			prefix_fp[i_fp].add_element(pattern[i]);
			if (power_of_2(i+1) && i!=m-1){
				i_fp++;
				prefix_fp[i_fp] = FP(prefix_fp[i_fp - 1]);
			}
		}
	}

	Stream_Match(Stream_Match_Params<T, StrT> &params, const std::vector<StrT> &pattern) : Stream_Match(params.p, &params.r_pow, pattern){};
	
	/* stream_element(c)
	- feed an element, c, of the stream;
	- returns true if there is a match with the pattern at the current elements;
	*/
	bool stream_element(StrT c){
		bool MATCH = false;
		n++;
		FP prev_fp = running_fp;
		running_fp.add_element(c);

		while (!viable_occurence.empty()){
			//while there are viable occurences to be checked at this position
			if (viable_occurence.begin()->first != n)
				break;

			//peek+pop the top of the multimap
			Viable_Occurence_Data<T, StrT> data = viable_occurence.begin()->second;
			remove_viable_occurence();
			
			FP intermediate_fp = running_fp.extract_suffix(data.start_fp);
			if (intermediate_fp != prefix_fp[data.stage]) 
				//if there is not a match at this stage discard the viable occurence
				continue;
			
			if (data.stage == log2_m){ 
				//if it is the last stage, save the result
				MATCH = true;
				continue; 
			}
			
			//if all checks passed, insert the viable occurence in the multimap using the appropriate stage
			insert_viable_occurence(data.stage, data.start_fp);
		}
		
		if (running_fp.extract_suffix(prev_fp) == prefix_fp[0]){
			insert_viable_occurence(0, prev_fp);
		}
		
		return MATCH;
	}
};