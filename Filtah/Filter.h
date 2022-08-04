#pragma once
#include <bitset>
#include <iostream>
#include <vector>

class Filter
{
public:
	Filter(int s);

	void Print();
	bool Add(uint_fast64_t num);
	bool Check(uint_fast64_t num);

	inline int GetSize() { return size; }
	inline std::vector<bool>* Vec() { return bits; }

private:

	inline uint_fast64_t HashMethodA(uint_fast64_t num) { return num; }
	inline uint_fast64_t HashMethodB(uint_fast64_t num) { return std::hash<uint_fast64_t>{}(num); }

	const int size = 0;
	//std::bitset<1> * bits;
	std::vector<bool> * bits;
};