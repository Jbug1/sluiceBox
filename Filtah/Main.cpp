#include "Filter.h"

enum DataFields
{
	METADATA = 0,
	SEQUENCE = 1,
	QUALITY = 3
};

struct DataGroup
{
	DataGroup(int a, int b, int c, int d, int e, int f) :
		metadata_s(a), metadata_e(b),
		sequence_s(c), sequence_e(d),
		quality_s(e), quality_e(f)
	{

	}
		
	int metadata_s, metadata_e;
	int sequence_s, sequence_e;
	int quality_s, quality_e;
};

int main()
{
	const int keysize = 18;

	Filter f(10000000);
	auto start = std::chrono::high_resolution_clock::now();
	f.PopulateFilter("genome.fna", keysize);
	auto end = std::chrono::high_resolution_clock::now();

	const std::string filename = "sampleinput.fastq";
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

	int segments = 0;
	for (int i = 0; i < content.size(); ++i)
	{
		if (content[i] == '+') { ++segments; }
	}

	std::vector<DataGroup> data;
	data.reserve(segments);
	int meta_s, meta_e;
	int seq_s, seq_e;
	int qual_s, qual_e;

	std::cout << "SIZE: " << data.size();

	int first = 0;
	int counter = 0;

	int cacheSize = content.size();
	for (int i = 0; i < cacheSize; ++i)
	{
		bool eof = (i == cacheSize - 1);
		if (content[i] == 10 || content[i] == 13 || eof)
		{
			switch (counter)
			{
			case METADATA:
				meta_s = first;
				meta_e = i - 1;
				break;
			case SEQUENCE:
				seq_s = first;
				seq_e = i - 1;
				break;
			case QUALITY:
				qual_s = first;
				qual_e = i - 1;

				data.emplace_back(meta_s, meta_e, seq_s, seq_e, qual_s, qual_e);
				break;
			default:
				//skip '+'
				break;
			}
			if (!eof)
			{
				++counter;
				counter %= 4;
				while (content[i] == 10 || content[i] == 13)
				{
					++i;
				}
				first = i;
				--i;
			}
		}
	}

	std::cout << "SIZE: " << data.size() << std::endl;

	int holdover;
	int endpoint;
	for (int i = 0; i < data.size(); ++i)
	{
		for (int j = data[i].sequence_s; j <= data[i].sequence_e - keysize + 1; )
		{
			uint_fast64_t conversion = 0;
			endpoint = j + keysize - 1;
			f.ConvertSequenceToInt(&content, j, &endpoint, &conversion, &holdover);
			if (f.Check(conversion)) 
			{ 
				std::cout << "----FOUND: "; 
			}
			else
			{
				std::cout << "NOT FOUND: " << conversion << " ";
			}
			f.PrintSequence(conversion, keysize);
			j += keysize;
		}
		std::cout << std::endl;
	}

	return 0;
}

//5,153,189