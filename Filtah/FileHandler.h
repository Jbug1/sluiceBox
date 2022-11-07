#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cassert>

enum class FileType
{
	TRANSCRIPTOME,
	GENOME
};

class FileHandler
{
public:
	FileHandler(FileType type, std::string transcriptome);
	~FileHandler();
	void ProcessNextChunk();

	std::string content;
	int readcount = 0;

	uintmax_t cursor_start = 0;
	uintmax_t cursor_end = 0;

	FileType type;

	inline uintmax_t GetRealSize() { return calcedBufferSize; }

	std::ifstream fileReader;
	//5283434
	const uintmax_t STRINGBUFFER_SIZE = 1000000000;
	bool singleRead = false;
private:
	void HandleChunk_Transcriptome(uintmax_t trueSize);
	void HandleChunk_Genome(uintmax_t trueSize);
	//file info
	uintmax_t filesize = 0;
	uintmax_t filesizeIter = 0;
	uintmax_t calcedBufferSize = 0;
};