#pragma once

#include <vector>
#include <chrono>
#include <time.h>
#include "FileHandler.h"

class Filter
{
public:
	Filter(int size, std::string genome, int keysize);

	void Print();
	bool Add(uint_fast64_t num);
	bool Check(uint_fast64_t num);

	inline int GetSize() { return size; }

	void PopulateFilter(std::string filename, int keysize);

	int ConvertSequenceToInt(std::string* file, int s, int* e, uint_fast64_t* ref, int* adjustments);
	int ConvertSequenceToIntOPT(std::string* file, int s, uint_fast64_t* ref);

	inline void PrintSequence(uint_fast64_t seq, int size)
	{
		std::string s(size, 0);
		for (int i = size - 1; i >= 0; --i)
		{
			int c = 3 & seq;
			switch (c)
			{
			case 0:  s[i] = 'A';	break;
			case 1:  s[i] = 'C';	break;
			case 2:  s[i] = 'G';	break;
			case 3:  s[i] = 'T';	break;
			default:
				return;
			}
			seq = seq >> 2;
		}
		std::cout << s << std::endl;
	}
	inline std::string GetSequenceAsString(uint_fast64_t seq, int size)
	{
		std::string s(size, 0);
		for (int i = size - 1; i >= 0; --i)
		{
			int c = 3 & seq;
			switch (c)
			{
			case 0:  s[i] = 'A';	break;
			case 1:  s[i] = 'C';	break;
			case 2:  s[i] = 'G';	break;
			case 3:  s[i] = 'T';	break;
			default:
				return "";
			}
			seq = seq >> 2;
		}
		return s;
	}

private:

	inline uint_fast64_t HashMethodA(uint_fast64_t num) { return num; }
	inline uint_fast64_t HashMethodB(uint_fast64_t num) { return std::hash<uint_fast64_t>{}(num); }
	inline uint_fast64_t HashMethodC(uint_fast64_t num) { return num; }
	inline uint_fast64_t HashMethodD(uint_fast64_t num) { return num; }

	const int size = 0;
	const int keysize = 0;
	uint_fast64_t bitmask = 0;
	std::vector<bool>* bits;
};