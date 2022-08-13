#include "Filter.h"

struct Pair
{
	float a, b;
};

Filter filter(65000);

Pair RandPopulate()
{
	Filter filter(65536);
	std::srand(time(NULL));
	const int numAdded = 10000;
	const unsigned int range = 300000000;
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

	std::cout << "ADDED NUMS: " << addedNums.size() << std::endl;
	int accuracy = 0;
	for (int i = 0; i < range; ++i)
	{
		//std::cout << "Checking " << i << " | " << ((addedNums.count(i) == 0) && filter.Check(i)) << std::endl;
		int a = addedNums.count(i) == 0; int b = a & filter.Check(i);
		accuracy += b;
	}
	//std::cout << "False: " << float(float(accuracy) / (range - numAdded)) << std::endl;

	float sum = 0;
	for (int i = 0; i < filter.GetSize(); ++i)
	{
		sum += (*filter.Vec())[i];
	}
	//std::cout << sum << " " << filter.Vec()->size() << std::endl;
	//std::cout << "Bit ratio: " << float(sum / filter.Vec()->size()) << std::endl;

	return { float(float(accuracy) / (range - numAdded)),float(sum / filter.Vec()->size()) };
}
void BasicTest()
{
	//filter.Print();
	const int testRuns = 1;
	std::cout << "Start" << std::endl;
	float accuracy = 0, bitalloc = 0;
	for (int i = 0; i < testRuns; ++i)
	{
		Pair p = RandPopulate();
		accuracy += p.a;
		bitalloc += p.b;
	}
	std::cout << "End: " << accuracy / testRuns << " | " << bitalloc / testRuns << std::endl;
}
int main()
{
	//filter.Print();
	std::string test = "AATAAATGCCGGATG";
	uint_fast64_t num;
	int s = test.size();

	filter.ConvertSequenceToInt(&test, 0, &s, &num);
	//filter.PrintSequence(num);

	std::bitset<64> rep(num);
	std::cout << num << " " << rep << std::endl;

	filter.PopulateFilter("genome.fna", 15);

	std::cout << filter.Check(num) << std::endl;
	return 0;
}

//5,153,189