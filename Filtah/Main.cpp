#include "Filter.h"
#include "DataTypes.h"

//remove final newline in output file

int main(int argc, char* argv[])
{
	auto startProgramTimer = std::chrono::high_resolution_clock::now();

	//check to see if there are enough args
	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}
	if (argc != 6) { std::cout << "NOT ENOUGH ARGS -> NEED 5, GIVEN " << argc - 1 << std::endl; abort(); }

	//Program arguments
	const int keysize = std::stoi(argv[1]);
	const int flexfactor = std::stoi(argv[2]);
	const uint_fast64_t filterSize = std::stoll(argv[3]);
	const std::string genomeFile = argv[4];
	//const std::string transcriptomeFile = "SRR19897826_trim.fastq";
	const std::string transcriptomeFile = argv[5];

	const std::string fileExtension = transcriptomeFile.substr(transcriptomeFile.size() - 6); //this is ass
	std::cout << "Genome file initializing" << std::endl;
	Filter filter(filterSize, genomeFile, keysize);
	std::cout << "Genome file processed" << std::endl;
	std::cout << "Transcriptome file initializing" << std::endl;
	FileHandler transcriptome(FileType::TRANSCRIPTOME, transcriptomeFile);
	std::cout << "Transcriptome file processed" << std::endl;
	std::vector<std::vector<DataGroup>> data;
	data.reserve(32);
	//-------------------------------------------------------------------------------------------------
	//looping section
	//This should inherit the file format from the input
	const std::string finalout_name = transcriptomeFile.substr(0, transcriptomeFile.size() - 6) + "_filtered" + fileExtension;
	std::ofstream finalout_file;
	finalout_file.open(finalout_name);
	char* finalout_buffer = new char[transcriptome.STRINGBUFFER_SIZE];
	std::cout << "NEW OUTPUT BUFFER CHUNK: " << transcriptome.STRINGBUFFER_SIZE << std::endl;

	int filechunkIter = 0;

	while (!transcriptome.fileReader.eof())
	{
		transcriptome.ProcessNextChunk(); //Gets chunk
		data.emplace_back();

		std::vector<DataGroup>& currData = data[filechunkIter];
		currData.reserve(transcriptome.readcount);
		printf("Reserving space for %d elements\n", transcriptome.readcount);
		RetrieveReads(&transcriptome, currData);

		memset(finalout_buffer, 0, transcriptome.GetRealSize());

		int holdover, endpoint, flex;
		BufferWriter bufferTool;
		for (int i = 0; i < currData.size(); ++i)
		{
			flex = 0;
			for (int j = currData[i].sequence_s; j <= currData[i].sequence_e - keysize + 1; )
			{
				uint_fast64_t conversion = 0;
				endpoint = j + keysize - 1;
				filter.ConvertSequenceToInt(&transcriptome.content, j, &endpoint, &conversion, &holdover);
				if (filter.Check(conversion))
				{
					bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].metadata_s, currData[i].metadata_e);
					bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].sequence_s, currData[i].sequence_e);
					bufferTool.AddPlus(finalout_buffer);
					bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].quality_s, currData[i].quality_e);
					break;
				}
				j += keysize;
				++flex;
				if (flex == flexfactor) { break; }
			}
		}
		std::cout << "Dumping chunk " << filechunkIter << " to file" << std::endl;
		finalout_file << finalout_buffer;

		++filechunkIter;
	}

	finalout_file.close();

	auto endProgramTimer = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(endProgramTimer - startProgramTimer).count() / 1000.0f;
	std::cout << time << std::endl;
	return 0;
}

