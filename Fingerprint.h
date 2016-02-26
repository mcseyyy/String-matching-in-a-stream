/*
	Yes, I know that in the header file I should have only the function definitions
	and that I should have the function implementations in a *.cpp file but it
	ended up being too verbose and I was sick an tired of copying 
	"template <typename T,typename StrT>" before every function.

	Just collapse all the functions and you have a nicele-looking header file :D.

	Have an awesome day!
*/

#pragma once
// CLEANUP includes once testing is finished !!!!!!!
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


template <typename T,typename StrT>
// T    - type used to store the fingerprint
// StrT - type of the elements that will be fingerprinted (e.g. use char if you fingerprint a string)
class Fingerprint{
	typedef Fingerprint<T, StrT> FP;
public:
	

	T fp;						//this is the actual fingerprint
	T count = 0;				//number of elements used in the fingerprint
	T p;						//prime number
	std::vector<T> *r_pow=NULL;		//preprocessed powers of r (r^0,r^1,...,r^p-2);     NOTE: (r^(p-1)) % p == 1
	
	

	//basic constructor of an empty fingerprint
	Fingerprint(const T _p, std::vector<T> *_r) : p(_p), r_pow(_r), fp(0){};

	//constructs fingerprint of the vector
	Fingerprint(const T _p, std::vector<T> *_r, const std::vector<StrT> &str) :p(_p), r_pow(_r), fp(0){
		int m = str.size();

		for (auto i=0,j = 1; i < m; i++,j++){
			j = (j == p - 1) ? 0 : j;
			fp = (fp + str[i] * r(j)) % p;
			
		}
		count = m;
	}

	//constructs fingerprint from one character
	Fingerprint(const T _p, std::vector<T> *_r, StrT c) :p(_p), r_pow(_r), count(1), fp((c*(*_r)[1])%_p){};

	//r(i) returns: (r^i) mod p
	T r(T i){
		return (*r_pow)[ui((i) % (p-1))];
	}

	//adds another element to the current fingerprint (at the end)
	// new_fp = old_fp + c^(count+1) mod p
	void add_element(StrT c){
		count++;
		fp = (fp + c * r(count)%p) % p;
	}

	//concatenates 2 fingerprints
	static FP concat(const FP &left, const FP &right){
		FP result(left.p, left.r);
		result.fp = (left.fp + right.fp * left.r(left.count)%left.p) % left.p;
		result.count = left.count + right.count;
		return result;
	}

	//adds a fingerprint to the right of the current fingerprint
	void concat_right(const FP &right){
		fp = (fp + right.fp * r(count)%p) % p;
		count += right.count;
	}

	//adds a fingerprint to the left of the current fingerprint
	void concat_left(const FP &left){
		fp = (left.fp + fp*r(left.count)%p) % p;
		count+= left.count;
	}

	//extracts the prefix of the current fingerprint given its suffix
	FP extract_prefix(const FP &suffix){
		FP result(p, r_pow);
		result.count = count - suffix.count;
		T other_fp = suffix.fp * r(result.count) % p; 
		result.fp = (fp >= other_fp) ? fp - other_fp : p - other_fp + fp;
		return result;
	}

	//extracts the suffix of a fingerprint given its prefix
	FP extract_suffix(const FP &prefix){
		FP result(p, r_pow);
		result.count = count - prefix.count;
		result.fp = (fp >= prefix.fp) ? fp - prefix.fp : p - prefix.fp + fp;
		result.fp = (result.fp * r(p - 1 - prefix.count%(p-1))) % p;
		return result;
	}

	//self-explainatory; assumes same (p,r) pair has been used
	bool operator==(const FP &other){
		return (count == other.count) &&
			(fp == other.fp);
	}

	FP &operator=(const FP &other){
		fp = other.fp;
		count = other.count;
		return *this;
	}

	operator T(){	return fp; }

	void print(){
		std::cout << fp << ' ' << count << '\n';
	}
	
	//friend std::ostream& operator<< (std::ostream &out, const Fingerprint<T,StrT> &FP;)
};

//template <typename T, typename StrT>
//std::ostream& operator<< (std::ostream &out, const Fingerprint<T,StrT> &FP){
//	out << "fp=" << FP.fp << " curr_r=" << FP.curr_r << " count_power=" << FP.count_power << '\n';
//	return out;
//}