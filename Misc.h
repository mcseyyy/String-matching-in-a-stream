#pragma once
#include<vector>

typedef unsigned int ui;
typedef unsigned long long ull;

template <typename T>
inline bool power_of_2(T x){
	return ((x != 0) && !(x & (x - 1)));
}

inline ui pow2(const ui i){
	return 1 << i;
}

template <typename T>
ui hammging_dist(const std::vector<T> &x,unsigned int x_i, const std::vector<T> &y, unsigned int y_i){
	ui dist(0);
	for (; x_i < x.size() && y_i < y.size(); x_i++, y_i++)
		dist += (x[x_i] != y[y_i]);
}

template <typename T>
ui hammging_dist(const std::vector<T> &x, const std::vector<T> &y){
	return hamming_dist(x, 0, y, 0);
}

template <typename T>
bool match(const std::vector<T> &x, unsigned int x_i, const std::vector<T> &y, unsigned int y_i){
	for (; x_i < x.size() && y_i < y.size(); x_i++, y_i++)
		if (x[x_i] != y[y_i])
			return false;
	return true;
}

template <typename T>
ui match(const std::vector<T> &x, const std::vector<T> &y){
	return match(x, 0, y, 0);
}

template <typename T>
//		OPTIMISE THIS FOR GOD'S SAKE....
ui exact_period(const std::vector<T> &str){
	auto m = str.size();
	for (ui i(0); i < m; i++)
		if (match(str, i, str, 0))
			return m - i;
	return 0;
}

bool is_prime(ull n) {
	
	if (!(n % 2) || !(n % 3))
		return false;
	ull root = ull(sqrt(n));
	for (ull i = 5, j = 7; i <= root; i += 6){
		if (n%i == 0)
			return false;
	}
	for (ull i = 7; i <= root; i += 6){
		if (n%i == 0)
			return false;
	}
	return true;
}

ull prime_bigger_than(ull n){
	while (true){
		if (is_prime(n))
			return n;
		n++;
	}
}
template <typename T>
std::vector<T> initialise_r(const T r1, const T p){
	std::vector<T> r(ui(p - 1));
	r[0] = 1;
	r[1] = r1;

	for (ui i = 2; i < p - 1; i++){
		r[i] = (r[i - 1] * r1) % p;

	}
	return r;
}