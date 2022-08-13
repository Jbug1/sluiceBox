#pragma once
#include <bitset>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <chrono>
#include <bit>
#include <unordered_set>
#include <time.h>

class Filter
{
public:
	Filter(int s);

	void Print();
	bool Add(uint_fast64_t num);
	bool Check(uint_fast64_t num);

	inline int GetSize() { return size; }
	inline std::vector<bool>* Vec() { return bits; }

	void PopulateFilter(std::string filename, int keysize);

	inline int ConvertSequenceToInt(std::string* file, int s, int * e, uint_fast64_t* ref)
	{
		*ref = 0;
		while (s <= *e)
		{
			switch ((*file)[s])
			{
			case 'A':	*ref = (*ref) << 2; *ref |= 0;	break;
			case 'C':	*ref = (*ref) << 2; *ref |= 1;	break;
			case 'G':	*ref = (*ref) << 2; *ref |= 2;	break;
			case 'T':	*ref = (*ref) << 2; *ref |= 3;	break;
			case 10:
			case 13:
						*e += 1; break;
			default:
				std::cout << int((*file)[s]) << std::endl;
				return s;
			}
			++s;
		}
		return -1;
	}

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

	

	const int size = 0;
	//std::bitset<1> * bits;
	std::vector<bool> * bits;
};