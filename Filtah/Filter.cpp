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
	(*bits)[HashMethodA(num) % size] = 1;
	(*bits)[HashMethodB(num) % size] = 1;

	return false;
}

bool Filter::Check(uint_fast64_t num)
{
	//std::cout << "RESULT: " << ((*bits)[HashMethodA(num) % size] & (*bits)[HashMethodA(num) % size]) << std::endl;
	return	(*bits)[HashMethodA(num) % size] &&
			(*bits)[HashMethodB(num) % size];
}

void Filter::PopulateFilter(std::string filename, int keysize)
{
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
	std::bitset<64> x(bitmask);
	//std::cout << x << '\n';

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
	}
	int beginning = 0;
	int end = 0;
	bool insideInfoLine = false;

	uint_fast64_t convertedSequence = 0;
	std::vector<uint_fast64_t> sequenceBreaks;
	int breakIndex = 0;
	int adjustments = 0;
	bool optActive = false;

	for (int i = 0; i < content.size(); ++i)
	{
		while (true)
		{
			if (content[i] != 'A' && content[i] != 'C' && content[i] != 'G' && content[i] != 'T')
			{
				//std::cout << "BAD CHAR: " << content[i] << std::endl;
				++i; if (i == content.size()) { return; }
				optActive = false;
				continue;
			}
			else
			{
				//std::cout << "NEW SEQUENCE" << std::endl;
				beginning = i;
				end = beginning + keysize - 1;
				break;
			}
		}
		int result = optActive	?	ConvertSequenceToIntOPT(&content, end, &end, &convertedSequence, &adjustments)
								:	ConvertSequenceToInt(&content, beginning, &end, &convertedSequence, &adjustments);
		if (result != -1)
		{
			i = result;
		}
		else
		{
			beginning += 1 + adjustments;
			end = beginning + keysize - 1;

			if (convertedSequence == 1856485284)
			{
				std::cout << "??????????????????????????????????";
			}
			Add(convertedSequence);
			//PrintSequence(convertedSequence, keysize);
			optActive = true;
		}
	}
	//std::cout << "RESULT: " << numSequences << std::endl;
}