#include "Filter.h"
#include "DataTypes.h"


int BUFFERINDEX = 0;

void WriteFromBuffer(char* buffer_target, std::string* buffer_source, int s, int e)
{
	int j = s;
	for (int i = BUFFERINDEX; i <= BUFFERINDEX + (e - s + 1); ++i)
	{
		//run tests here for speed
		buffer_target[i] = (*buffer_source)[j];
		++j;
	}
	BUFFERINDEX += e - s + 1;
	buffer_target[BUFFERINDEX] = '\n';
	++BUFFERINDEX;
}

int main()
{
	auto startProgramTimer = std::chrono::high_resolution_clock::now();

	//Program arguments
	const int keysize = 18;
	const int flexfactor = 1;
	const uint_fast64_t filterSize = 1000000000;
	const std::string genomeFile = "genome.fna";
	const std::string transcriptomeFile = "SRR19897826.fastq";
	const std::string fileExtension = transcriptomeFile.substr(transcriptomeFile.size() - 6); //this is ass

	Filter filter(filterSize, genomeFile, keysize);
	FileHandler transcriptome(transcriptomeFile);

	std::vector<DataGroup> data;
	data.reserve(transcriptome.readcount);
	printf("Reserving space for %d elements\n", transcriptome.readcount);
	RetrieveReads(&transcriptome, &data);

	//This should inherit the file format from the input
	const std::string finalout_name = transcriptomeFile.substr(0, transcriptomeFile.size() - 6) + "_filtered" + fileExtension;
	std::ofstream finalout_file;
	finalout_file.open(finalout_name);
	char* finalout_buffer = new char[transcriptome.filesize];
	memset(finalout_buffer, 0, transcriptome.filesize);

	int holdover;
	int endpoint;
	int flex;
	for (int i = 0; i < data.size(); ++i)
	{
		flex = 0;
		for (int j = data[i].sequence_s; j <= data[i].sequence_e - keysize + 1; )
		{
			uint_fast64_t conversion = 0;
			endpoint = j + keysize - 1;
			filter.ConvertSequenceToInt(&transcriptome.content, j, &endpoint, &conversion, &holdover);
			if (filter.Check(conversion))
			{
				//std::cout << "----FOUND: " << conversion << " ";
				WriteFromBuffer(finalout_buffer, &transcriptome.content, data[i].metadata_s, data[i].metadata_e);
				WriteFromBuffer(finalout_buffer, &transcriptome.content, data[i].sequence_s, data[i].sequence_e);
				finalout_buffer[BUFFERINDEX] = '+';
				finalout_buffer[BUFFERINDEX + 1] = '\n';
				BUFFERINDEX += 2;
				WriteFromBuffer(finalout_buffer, &transcriptome.content, data[i].quality_s, data[i].quality_e);
				break;
			}
			j += keysize;
			++flex;
			if (flex == flexfactor) { break; }
		}
	}

	std::cout << "Dumping buffer to file" << std::endl;
	finalout_file << finalout_buffer;
	finalout_file.close();
	delete[] finalout_buffer;

	auto endProgramTimer = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(endProgramTimer - startProgramTimer).count() / 1000.0f;
	std::cout << time << std::endl;
	return 0;
}

//5,153,189