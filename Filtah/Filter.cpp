#include "Filter.h"

Filter::Filter(int s) :
	size(s)
{
	bits = new std::vector<bool>();
	bits->resize(size);
	std::cout << "Filter created, with size " << bits->size() << std::endl;
}

void Filter::Print()
{
	for (int i = 0; i < size; ++i)
	{
		std::cout << (*bits)[i] << " ";
	}
	std::cout << std::endl;
}

bool Filter::Add(uint_fast64_t num)
{
	bool val = (*bits)[num % size];

	(*bits)[HashMethodA(num) % size] = 1;
	(*bits)[HashMethodB(num) % size] = 1;
	
	return val;
}

bool Filter::Check(uint_fast64_t num)
{
	//std::cout << "RESULT: " << ((*bits)[HashMethodA(num) % size] & (*bits)[HashMethodA(num) % size]) << std::endl;
	return	(*bits)[HashMethodA(num) % size] & 
			(*bits)[HashMethodB(num) % size];
}