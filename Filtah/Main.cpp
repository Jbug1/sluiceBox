#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <chrono>
#include <bit>
#include <bitset>
#include <unordered_set>
#include <time.h>

struct Pair
{
	float a, b;
};

#include "Filter.h"

Pair RandPopulate()
{
	Filter filter(65536);
	std::srand(time(NULL));
	float accuracy = 0;
	const int numAdded = 10000;
	const int range = 200000000;
	std::unordered_set<int> addedNums;
	std::cout << "Starting..." << std::endl;
	for (int i = 0; i < numAdded; ++i)
	{
		int num = 0;
		while (addedNums.count(num) != 0)
		{
			num = std::rand() % range;
		}
		//std::cout << "Attempting to insert " << num << "..." << std::endl;
		bool result = filter.Add(num);
		addedNums.insert(num);
	}

	//std::cout << "ADDED NUMS: " << addedNums.size() << std::endl;

	for (int i = 0; i < range; ++i)
	{
		//std::cout << "Checking " << i << " | " << (addedNums.count(i) == 1) << " " << filter.Check(i) << std::endl;
		accuracy += ((addedNums.count(i) == 0) && filter.Check(i));
	}
	//std::cout << accuracy << std::endl;
	std::cout << "False: " << float(accuracy / (range - numAdded)) << std::endl;

	float sum = 0;
	for (int i = 0; i < filter.GetSize(); ++i)
	{
		sum += (*filter.Vec())[i];
	}
	//std::cout << sum << " " << filter.Vec()->size() << std::endl;
	//std::cout << "Bit ratio: " << float(sum / filter.Vec()->size()) << std::endl;

	return { float(accuracy / (range - numAdded)),float(sum / filter.Vec()->size()) };
}

int main()
{
	//filter.Print();
	std::cout << "Start" << std::endl;
	float accuracy = 0, bitalloc = 0;
	for (int i = 0; i < 5; ++i)
	{
		Pair p = RandPopulate();
		accuracy += p.a;
		bitalloc += p.b;
	}
	
	std::cout << "End: " << accuracy/5 << " | " << bitalloc/5 << std::endl;
	//filter.Print();

	return 0;
}