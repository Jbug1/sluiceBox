#include "FileHandler.h"

FileHandler::FileHandler(FileType type, std::string transcriptome)
	: type(type)
{
	fileReader = std::ifstream(transcriptome, std::ios::binary);
	if (!fileReader) {
		std::cout << "File load error with: " << transcriptome << std::endl;
		abort();
	}
	else
	{
		//fileReader.seekg(0, std::ios::beg);
		std::filesystem::path p{ transcriptome };
		filesize = std::filesystem::file_size(p);
		filesizeIter = filesize;
		std::cout << "INIT WITH FILE SIZE: " << filesize << std::endl;
		singleRead = filesize < STRINGBUFFER_SIZE ? true : false;
	}
}

FileHandler::~FileHandler()
{
	fileReader.close();
}

void FileHandler::HandleChunk_Genome(uintmax_t trueSize)
{
	calcedBufferSize = cursor_end - cursor_start + 1;
	cursor_start = cursor_end + 1;
	filesizeIter -= trueSize;
}

void FileHandler::HandleChunk_Transcriptome(uintmax_t trueSize)
{
	if (!singleRead)
	{
		bool scanning = true;
		while (scanning)
		{
			if (content[cursor_end - cursor_start] == '@')
			{
				if (cursor_end == 0)
				{
					std::cout << "Sequence exceeds buffer, aborting" << std::endl;
					abort();
				}
				else
				{
					uintmax_t backwardsIter = cursor_end - cursor_start - 1;
					while (content[backwardsIter] == 10 || content[backwardsIter] == 13)
					{
						--backwardsIter;
					}
					if (content[backwardsIter] != '+')
					{
						--cursor_end;
						++filesizeIter;
						calcedBufferSize = cursor_end - cursor_start + 1;
						std::cout << "Starting new range: " << cursor_start << " | " << cursor_end << std::endl;
						cursor_start = cursor_end + 1;
						filesizeIter -= trueSize;
						scanning = false;
					}
				}
			}
			else
			{
				--cursor_end;
				++filesizeIter;
			}
		}
	}
	else
	{
		cursor_end++;
		calcedBufferSize = cursor_end - cursor_start;
		std::cout << "Starting new range on FINAL: " << cursor_start << " | " << cursor_end << std::endl;
	}

	for (int i = 0; i < GetRealSize(); ++i)
	{
		if (content[i] == '+') { ++readcount; }
	}
}

void FileHandler::ProcessNextChunk()
{
	//Set filereader to begin iteration at cursor_start, 
	//	where cursor_start is the number of characters 
	//	from the beginning
	fileReader.seekg(cursor_start, std::ios::beg);
	uintmax_t trueSize = singleRead ? filesize : STRINGBUFFER_SIZE;
	content = std::string(trueSize, 0);
	fileReader.read(&content[0], trueSize + 1);
	if (fileReader.eof()) 
	{ 
		singleRead = true; 
		trueSize = filesizeIter;
	}
	cursor_end = cursor_start + trueSize - 1;

	std::cout << "ProcessNextChunk(): " << trueSize << std::endl;
	std::cout << "TOTAL FILE: " << filesize << " | USING: " << trueSize << std::endl;
	//check flags for overshoot

	switch (type)
	{
	case FileType::TRANSCRIPTOME:
		HandleChunk_Transcriptome(trueSize);
		break;
	case FileType::GENOME:
		HandleChunk_Genome(trueSize);
		break;
	default:
		std::cout << "UNDEFINED TYPE OF FILEREADER" << std::endl;
		abort();
	}

}