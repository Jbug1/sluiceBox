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
	return	(*bits)[HashMethodA(num) % size] &&
		(*bits)[HashMethodB(num) % size];
}

void Filter::PopulateFilter(std::string filename, int keysize)
{
	std::ofstream myfile;
	myfile.open("compareout.txt");



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

	uint_fast64_t convertedSequence;
	std::vector<uint_fast64_t> sequenceBreaks;
	int breakIndex = 0;

	for (int i = 0; i < content.size(); ++i)
	{
		if (insideInfoLine)
		{
			if (content[i] == 10 || content[i] == 13 || content[i] == 78)
			{
				continue;
			}
			else
			{
				beginning = i;
				end = beginning + keysize - 1;
				insideInfoLine = false;
			}
		}
		else
		{
			int result = ConvertSequenceToInt(&content, beginning, &end, &convertedSequence);
			if (result != -1)
			{
				i = result;
				insideInfoLine = true;
			}
			else
			{
				beginning += 1;
				end = beginning + keysize - 1;

				Add(convertedSequence);
				std::cout << "ADDING: " << convertedSequence << std::endl;
				++numSequences;
				myfile << GetSequenceAsString(convertedSequence, keysize) << std::endl;
				//std::bitset<64> rep(convertedSequence);
				//std::cout << "INSERTING: " << rep << " | ";
				//std::cout << convertedSequence << " | ";
				//PrintSequence(convertedSequence);
			}
		}
	}
	myfile.close();
	std::cout << "RESULT: " << numSequences << std::endl;
}