#include "Filter.h"

Filter::Filter(int size, std::string genome, int keysize) :
	size(size),
	keysize(keysize)
{
	bits = new std::vector<bool>();
	bits->resize(size);
	std::cout << "Filter created, with size " << bits->size() << std::endl;
	PopulateFilter(genome, keysize);
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
	(*bits)[HashMethodA(num) % size] = 1;
	(*bits)[HashMethodB(num) % size] = 1;

	return false;
}

bool Filter::Check(uint_fast64_t num)
{
	//use both hash methods to check if 64 bit representation of sequence is present in filter
	return	(*bits)[HashMethodA(num) % size] &&
			(*bits)[HashMethodB(num) % size];
}

int Filter::ConvertSequenceToInt(std::string* file, int s, int* e, uint_fast64_t* ref, int* adjustments)
{	
	//if we are on an endline character, we want to ignore this and move forward
	//if we encounter a valid base, we want to add it to the int representation
	//if we encounter a non-endline, non-base character, we need to reset the int representation
	*ref = 0;
	*adjustments = 0;

	//badstart will be true if we are on an endline character
	bool badStart = (*file)[s] == 10 || (*file)[s] == 13;
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
			if (badStart) { ++(*adjustments); }
			*e += 1; break;
		default:
			
			return s;
		}
		++s;
	}
	return -1;
}
int Filter::ConvertSequenceToIntOPT(std::string* file, int s, uint_fast64_t* ref)
{	
	//if we are on an endline character, we want to ignore this and move forward
	//if we encounter a valid base, we want to add it to the int representation
	//if we encounter a non-endline, non-base character, we need to reset the int representation
	switch ((*file)[s])
	{
	case 'A':	*ref = (*ref) << 2; *ref = *ref & bitmask; *ref |= 0; return 0;
	case 'C':	*ref = (*ref) << 2; *ref = *ref & bitmask; *ref |= 1; return 0;
	case 'G':	*ref = (*ref) << 2; *ref = *ref & bitmask; *ref |= 2; return 0;
	case 'T':	*ref = (*ref) << 2; *ref = *ref & bitmask; *ref |= 3; return 0;
	case 10:
	case 13:
		return 1;
		break;
	default:
		return -1;
	}
}

void Filter::PopulateFilter(std::string filename, int keysize)
{
	std::cout << "Filter Start" << std::endl;
	bitmask = 0;
	int maskSize = keysize * 2;
	for (int i = 0; i < maskSize; ++i)
	{
		bitmask = bitmask << 1;
		if ((i + 2) < maskSize)
		{
			++bitmask;
		}
	}

	std::ofstream finalout_file;
	finalout_file.open("DEBUG_FILTERSEQUENCES.txt");

	FileHandler genome(FileType::GENOME, filename);

	uint_fast64_t convertedSequence = 0;
	int numAdded = 0;
	int shiftedIter = 0;
	std::cout << "Filter iteration start" << std::endl;

	while (!genome.fileReader.eof())
	{
		genome.ProcessNextChunk();
		for (int i = 0; i < genome.GetRealSize(); ++i)
		{
			int result = ConvertSequenceToIntOPT(&genome.content, i, &convertedSequence);
			if (result == -1)
			{
				convertedSequence = 0;
				shiftedIter = 0;
			}
			else if (result == 0)
			{
				++shiftedIter;
				if (shiftedIter == keysize)
				{
					--shiftedIter;
					Add(convertedSequence);
					++numAdded;
				}
			}
		}
	}
	std::cout << "Filter done, added " << numAdded << std::endl;
}