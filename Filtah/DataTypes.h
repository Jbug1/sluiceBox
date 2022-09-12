#pragma once
#include "FileHandler.h"

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

void RetrieveReads(FileHandler * transcriptome, std::vector<DataGroup> * data)
{
	int meta_s, meta_e;
	int seq_s, seq_e;
	int qual_s, qual_e;
	int first = 0, counter = 0;
	int cacheSize = transcriptome->content.size();

	for (int i = 0; i < cacheSize; ++i)
	{
		bool eof = (i == cacheSize - 1);
		if (transcriptome->content[i] == 10 || transcriptome->content[i] == 13 || eof)
		{
			switch (counter)
			{
			case METADATA:
				meta_s = first; meta_e = i - 1; break;
			case SEQUENCE:
				seq_s = first;  seq_e = i - 1;  break;
			case QUALITY:
				qual_s = first;	qual_e = i - 1;
				data->emplace_back(meta_s, meta_e, seq_s, seq_e, qual_s, qual_e);
				break;
			default:
				break; //skip '+'
			}
			if (!eof)
			{
				++counter; counter %= 4;
				while (transcriptome->content[i + 1] == 10 || transcriptome->content[i + 1] == 13) { ++i; }
				first = i + 1;
			}
		}
	}
}