#include "Filter.h"
#include "DataTypes.h"
#include "TestHeader.h"
#include <iostream>  
#include <cstring>  

//remove final newline in output file

int main(int argc, char* argv[])
{

	//catch case in which user is displaying help
	if (argv[1] == "--help" || argv[1] == "-h"){
		std::cout << "Order of arguments:  char:mode (p for positive, n for negative) int:keysize int:flex_factor str:genomeFilepath str:transcriptomeFilepath" << std::endl;
		std::cout << "See Github docs for additional descriptions of these arguments" << std::endl;

		return 0;
	}

	auto startProgramTimer = std::chrono::high_resolution_clock::now();
	if (argc != 6) { std::cout << "NOT ENOUGH ARGS -> NEED 5, GIVEN " << argc - 1 << std::endl; abort(); }

	
	//Program arguments
	const int keysize = std::stoi(argv[2]); //if(argc > 1) //arg -> keysize:18 -> split(:) if keysize -> 18
	const int flexfactor = std::stoi(argv[3]);
	const uint_fast64_t filterSize = 1000000000;
	const std::string genomeFile = argv[4];
	//const std::string transcriptomeFile = "SRR19897826_trim.fastq";
	const std::string transcriptomeFile = argv[6];

	//get positive or negative mode
	bool negativeMode = false;
	if (argv[1] == "n" || argv[1] == "negative") {

		negativeMode = true;
	}

	const std::string fileExtension = transcriptomeFile.substr(transcriptomeFile.size() - 6);
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
	//output name in just input_name_filtered.fastq
	const std::string finalout_name = transcriptomeFile.substr(0, transcriptomeFile.size() - 6) + "_filtered" + fileExtension;
	std::ofstream finalout_file;
	finalout_file.open(finalout_name);
	char* finalout_buffer = new char[transcriptome.STRINGBUFFER_SIZE];
	//std::cout << "NEW OUTPUT BUFFER CHUNK: " << transcriptome.STRINGBUFFER_SIZE << std::endl;

	//keep track of how many chunks we have processed, writing to the terminal each time
	int filechunkIter = 0;

	//keep processing chunks until we reach the end of the file
	while (!transcriptome.fileReader.eof())
	{
		transcriptome.ProcessNextChunk(); //Gets chunk
		data.emplace_back();

		std::vector<DataGroup>& currData = data[filechunkIter];
		currData.reserve(transcriptome.readcount);
		
		//printf("Reserving space for %d elements\n", transcriptome.readcount);
		RetrieveReads(&transcriptome, currData);

		memset(finalout_buffer, 0, transcriptome.GetRealSize());

		int holdover, endpoint, flex;
		BufferWriter bufferTool;
		bool found;
		for (int i = 0; i < currData.size(); ++i)
		{
			//initialize flex to be 0, keep iterating until we reach EoF or flex factor
			flex = 0;

			//initialize found as false, tells us if a k-mer was found in filter
			found = false;

			for (int j = currData[i].sequence_s; j <= currData[i].sequence_e - keysize + 1; )
			{
				uint_fast64_t conversion = 0;
				endpoint = j + keysize - 1;

				//use non-optimized conversion to 64-bit int as we need to grab non-overlapping secitons
				filter.ConvertSequenceToInt(&transcriptome.content, j, &endpoint, &conversion, &holdover);
				if (filter.Check(conversion))
				{
					//check to see if we are in negative mode, if not, we want to keep this read
					if (!negativeMode)
					{
						bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].metadata_s, currData[i].metadata_e);
						bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].sequence_s, currData[i].sequence_e);
						bufferTool.AddPlus(finalout_buffer);
						bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].quality_s, currData[i].quality_e);
					}
					found = true;
					break;
				}
				j += keysize;
				++flex;
				if (flex == flexfactor) { break; }
			}

			//if we are in negative mode, and none of the kmers were found, we want to keep this read
			if (negativeMode && !found)
			{
				bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].metadata_s, currData[i].metadata_e);
				bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].sequence_s, currData[i].sequence_e);
				bufferTool.AddPlus(finalout_buffer);
				bufferTool.Write(finalout_buffer, &transcriptome.content, currData[i].quality_s, currData[i].quality_e);
			}
		}

		//write what chunk we are on to terminal
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

