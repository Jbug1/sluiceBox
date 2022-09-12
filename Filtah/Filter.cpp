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
	(*bits)[HashMethodC(num) % size] = 1;
	(*bits)[HashMethodD(num) % size] = 1;

	return false;
}

bool Filter::Check(uint_fast64_t num)
{
	return	(*bits)[HashMethodA(num) % size] &&
			(*bits)[HashMethodB(num) % size] &&
			(*bits)[HashMethodC(num) % size] && 
			(*bits)[HashMethodD(num) % size];
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

	int numSequences = 0;
	std::ifstream fileReader(filename, std::ios::binary | std::ios::ate);
	std::string content;
	if (fileReader) {
		std::filesystem::path p{ filename };
		auto fileSize = std::filesystem::file_size(p);
		fileReader.seekg(std::ios::beg);
		content = std::string(fileSize, 0);
		fileReader.read(&content[0], fileSize);
	}
	else
	{
		std::cout << "File load error with: " << filename << std::endl;
		abort();
	}
	int beginning = 0;
	int end = 0;
	bool insideInfoLine = false;

	uint_fast64_t convertedSequence = 0;
	std::vector<uint_fast64_t> sequenceBreaks;
	int breakIndex = 0;
	int adjustments = 0;
	bool optActive = false;
	std::cout << "Filter iteration start" << std::endl;
	for (int i = 0; i < content.size(); ++i)
	{
		while (true)
		{
			if (content[i] != 'A' && content[i] != 'C' && content[i] != 'G' && content[i] != 'T')
			{
				++i; if (i == content.size()) { return; }
				optActive = false;
				continue;
			}
			else
			{
				beginning = i;
				end = beginning + keysize - 1;
				break;
			}
		}
		int result = optActive ?	ConvertSequenceToIntOPT(&content, end, &end, &convertedSequence, &adjustments)
								:	ConvertSequenceToInt(&content, beginning, &end, &convertedSequence, &adjustments);
		if (result != -1)
		{
			i = result;
		}
		else
		{
			beginning += 1 + adjustments;
			end = beginning + keysize - 1;

			Add(convertedSequence);
			optActive = true;
		}
	}
	std::cout << "Filter done" << std::endl;
}